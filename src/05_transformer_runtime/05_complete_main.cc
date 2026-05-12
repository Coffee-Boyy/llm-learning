#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "03_tokenization_template/03_qwen3_template.h"
#include "00_common/strings.h"
#include "05_transformer_runtime/05_model_context.h"
#include "05_transformer_runtime/05_session.h"
#include "06_sampling_decode/06_sampler.h"

namespace {

void Usage() {
  std::cerr
      << "dissected-llm-complete [--model PATH] [--llama-cli PATH] [-p PROMPT|--prompt PROMPT]\n"
      << "  [--instructions TEXT] [--max-tokens N] [--temp T] [--top-p P] [--top-k K]\n"
      << "  [--min-p M] [--presence-penalty P] [--stream] [--gpu-layers N] [--model-name NAME]\n"
      << "Environment: DISSECTED_LLM_MODEL, DISSECTED_LLM_LLAMA_CLI\n"
      << "If no prompt flag is set, the full stdin buffer is used as the user message.\n";
}

}  // namespace

int main(int argc, char** argv) {
  std::string model_path;
  std::string llama_cli;
  std::string prompt_flag;
  std::string instructions;
  std::string model_name = "qwen3-8b-q4_k_m";
  int max_tokens = 128;
  double temperature = -1;
  double top_p = -1;
  int top_k = -1;
  double min_p = -1;
  double presence_penalty = -1;
  bool stream = false;
  int gpu_layers = 99;

  if (const char* env_model = std::getenv("DISSECTED_LLM_MODEL")) {
    model_path = env_model;
  }
  if (const char* env_cli = std::getenv("DISSECTED_LLM_LLAMA_CLI")) {
    llama_cli = env_cli;
  }

  for (int i = 1; i < argc; ++i) {
    std::string key = argv[i];
    auto next = [&]() -> std::string { return i + 1 < argc ? argv[++i] : ""; };
    if (key == "-h" || key == "--help") {
      Usage();
      return 0;
    }
    if (key == "--model") {
      model_path = next();
    } else if (key == "--llama-cli") {
      llama_cli = next();
    } else if (key == "-p" || key == "--prompt") {
      prompt_flag = next();
    } else if (key == "--instructions") {
      instructions = next();
    } else if (key == "--max-tokens") {
      max_tokens = std::stoi(next());
    } else if (key == "--temp") {
      temperature = std::stod(next());
    } else if (key == "--top-p") {
      top_p = std::stod(next());
    } else if (key == "--top-k") {
      top_k = std::stoi(next());
    } else if (key == "--min-p") {
      min_p = std::stod(next());
    } else if (key == "--presence-penalty") {
      presence_penalty = std::stod(next());
    } else if (key == "--stream") {
      stream = true;
    } else if (key == "--gpu-layers") {
      gpu_layers = std::stoi(next());
    } else if (key == "--model-name") {
      model_name = next();
    } else {
      std::cerr << "unknown argument: " << key << "\n";
      Usage();
      return 2;
    }
  }

  std::string input = prompt_flag;
  if (input.empty()) {
    std::ostringstream buffer;
    buffer << std::cin.rdbuf();
    input = buffer.str();
  }
  dissected::Trim(input);

  if (input.empty()) {
    std::cerr << "empty prompt (use -p or stdin)\n";
    return 2;
  }
  if (model_path.empty()) {
    std::cerr << "model path required (--model or DISSECTED_LLM_MODEL)\n";
    return 2;
  }

  dissected::runtime::ModelContext context(
      {model_path, model_name, llama_cli, gpu_layers, 32768});
  dissected::runtime::GenerationSession session(context);

  auto mode = dissected::prompt::DetectThinkingMode(instructions + "\n" + input);
  auto sampling = dissected::sampling::DefaultsForThinkingMode(mode);
  if (temperature >= 0) {
    sampling.temperature = temperature;
  }
  if (top_p >= 0) {
    sampling.top_p = top_p;
  }
  if (top_k >= 0) {
    sampling.top_k = top_k;
  }
  if (min_p >= 0) {
    sampling.min_p = min_p;
  }
  if (presence_penalty >= 0) {
    sampling.presence_penalty = presence_penalty;
  }
  sampling.max_output_tokens = max_tokens;

  dissected::runtime::GenerationRequest request{input, instructions, sampling};

  if (stream) {
    auto generated = session.GenerateStreaming(request, [](std::string_view chunk) {
      std::cout << std::string(chunk) << std::flush;
    });
    if (!generated.ok()) {
      std::cerr << generated.status().message() << "\n";
      return 1;
    }
    std::cout << "\n";
    return 0;
  }

  auto generated = session.Generate(request);
  if (!generated.ok()) {
    std::cerr << generated.status().message() << "\n";
    return 1;
  }
  std::cout << generated.value().text << "\n";
  return 0;
}
