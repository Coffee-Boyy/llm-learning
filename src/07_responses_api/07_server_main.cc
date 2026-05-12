#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "03_tokenization_template/03_qwen3_template.h"
#include "00_common/strings.h"
#include "05_transformer_runtime/05_session.h"
#include "06_sampling_decode/06_sampler.h"
#include "07_responses_api/07_error_shapes.h"
#include "07_responses_api/07_json_validation.h"
#include "07_responses_api/07_responses_types.h"
#include "07_responses_api/07_sse_stream.h"

namespace {

struct Args {
  std::string model_path;
  std::string model_name = "qwen3-8b-q4_k_m";
  std::string llama_cli;
  std::string host = "127.0.0.1";
  int port = 8000;
  int gpu_layers = 99;
};

Args ParseArgs(int argc, char** argv) {
  Args args;
  if (const char* env_model = std::getenv("DISSECTED_LLM_MODEL")) {
    args.model_path = env_model;
  }
  if (const char* env_cli = std::getenv("DISSECTED_LLM_LLAMA_CLI")) {
    args.llama_cli = env_cli;
  }
  for (int i = 1; i < argc; ++i) {
    std::string key = argv[i];
    auto next = [&]() -> std::string { return i + 1 < argc ? argv[++i] : ""; };
    if (key == "--model") args.model_path = next();
    else if (key == "--model-name") args.model_name = next();
    else if (key == "--llama-cli") args.llama_cli = next();
    else if (key == "--host") args.host = next();
    else if (key == "--port") args.port = std::stoi(next());
    else if (key == "--gpu-layers") args.gpu_layers = std::stoi(next());
  }
  return args;
}

std::string HttpResponse(int code, const std::string& content_type, const std::string& body) {
  std::string reason = code == 200 ? "OK" : (code == 501 ? "Not Implemented" : "Bad Request");
  return "HTTP/1.1 " + std::to_string(code) + " " + reason + "\r\n"
         "Content-Type: " + content_type + "\r\n"
         "Access-Control-Allow-Origin: *\r\n"
         "Content-Length: " + std::to_string(body.size()) + "\r\n"
         "Connection: close\r\n\r\n" + body;
}

std::string BodyFromRequest(const std::string& request) {
  auto pos = request.find("\r\n\r\n");
  if (pos == std::string::npos) return "";
  return request.substr(pos + 4);
}

bool IsResponsesPost(const std::string& request) {
  return request.rfind("POST /v1/responses ", 0) == 0 || request.rfind("POST /responses ", 0) == 0;
}

bool WriteAll(int fd, std::string_view data) {
  while (!data.empty()) {
    ssize_t n = write(fd, data.data(), data.size());
    if (n <= 0) {
      return false;
    }
    data.remove_prefix(static_cast<std::size_t>(n));
  }
  return true;
}

}  // namespace

int main(int argc, char** argv) {
  Args args = ParseArgs(argc, argv);
  dissected::runtime::ModelContext context({args.model_path, args.model_name, args.llama_cli,
                                            args.gpu_layers, 32768});
  dissected::runtime::GenerationSession session(context);

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    std::cerr << "failed to create socket\n";
    return 1;
  }
  int reuse = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_port = htons(args.port);
  address.sin_addr.s_addr = args.host == "0.0.0.0" ? INADDR_ANY : inet_addr(args.host.c_str());
  if (bind(server_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
    std::cerr << "failed to bind " << args.host << ":" << args.port << "\n";
    return 1;
  }
  if (listen(server_fd, 16) < 0) {
    std::cerr << "failed to listen\n";
    return 1;
  }
  std::cout << "dissected-llm-server listening on " << args.host << ":" << args.port << "\n";

  while (true) {
    int client = accept(server_fd, nullptr, nullptr);
    if (client < 0) continue;
    char buffer[65536];
    std::memset(buffer, 0, sizeof(buffer));
    ssize_t n = read(client, buffer, sizeof(buffer) - 1);
    std::string response;
    if (n <= 0) {
      close(client);
      continue;
    }
    std::string request(buffer, static_cast<std::size_t>(n));
    if (!IsResponsesPost(request)) {
      response = HttpResponse(400, "application/json", dissected::api::ErrorJson("bad_request", "use POST /v1/responses"));
    } else {
      std::string body = BodyFromRequest(request);
      std::string feature;
      if (dissected::api::HasUnsupportedHostedFeature(body, &feature)) {
        response = HttpResponse(501, "application/json",
                                dissected::api::ErrorJson("unsupported_feature", "unsupported field: " + feature));
      } else {
        auto parsed = dissected::api::ParseResponsesRequest(body);
        if (!parsed.ok()) {
          response = HttpResponse(400, "application/json",
                                  dissected::api::ErrorJson("bad_request", parsed.status().message()));
        } else {
          auto req = parsed.value();
          auto mode = dissected::prompt::DetectThinkingMode(req.instructions + "\n" + req.input);
          auto sampling = dissected::sampling::DefaultsForThinkingMode(mode);
          if (req.temperature >= 0) sampling.temperature = req.temperature;
          if (req.top_p >= 0) sampling.top_p = req.top_p;
          sampling.max_output_tokens = req.max_output_tokens;
          dissected::runtime::GenerationRequest gen_req{req.input, req.instructions, sampling};
          const std::string model_id = req.model.empty() ? args.model_name : req.model;
          if (req.stream) {
            auto ready = session.ValidateBackend();
            if (!ready.ok()) {
              response = HttpResponse(400, "application/json",
                                        dissected::api::ErrorJson("generation_error", ready.message()));
            } else {
              std::string header =
                  "HTTP/1.1 200 OK\r\n"
                  "Content-Type: text/event-stream\r\n"
                  "Cache-Control: no-cache\r\n"
                  "Access-Control-Allow-Origin: *\r\n"
                  "Connection: close\r\n"
                  "\r\n";
              if (!WriteAll(client, header)) {
                close(client);
                continue;
              }
              auto streamed = session.GenerateStreaming(gen_req, [&](std::string_view chunk) {
                if (chunk.empty()) {
                  return;
                }
                std::string block =
                    dissected::api::FormatSseTextDelta(model_id, std::string(chunk));
                WriteAll(client, block);
              });
              if (!streamed.ok()) {
                std::string err_line =
                    "event: error\n"
                    "data: {\"error\":\"" +
                    dissected::JsonEscape(streamed.status().message()) + "\"}\n\n";
                WriteAll(client, err_line);
              } else {
                WriteAll(client, dissected::api::FormatSseCompleted());
                WriteAll(client, dissected::api::FormatSseDone());
              }
              close(client);
              continue;
            }
          } else {
            auto generated = session.Generate(gen_req);
            if (!generated.ok()) {
              response = HttpResponse(400, "application/json",
                                      dissected::api::ErrorJson("generation_error", generated.status().message()));
            } else {
              response = HttpResponse(200, "application/json",
                                        dissected::api::ResponseJson(req, generated.value()));
            }
          }
        }
      }
    }
    write(client, response.data(), response.size());
    close(client);
  }
}
