#include "07_responses_api/07_responses_types.h"

#include <chrono>

#include "00_common/strings.h"

namespace dissected::api {
namespace {

long CreatedAt() {
  return std::chrono::duration_cast<std::chrono::seconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

}  // namespace

std::string ResponseJson(const ResponsesRequest& request,
                         const runtime::GenerationResult& generation) {
  std::string model = request.model.empty() ? "qwen3-8b-q4_k_m" : request.model;
  std::string id = "resp_" + std::to_string(CreatedAt());
  int total = generation.input_tokens + generation.output_tokens;
  return "{"
         "\"id\":\"" + JsonEscape(id) + "\","
         "\"object\":\"response\","
         "\"created_at\":" + std::to_string(CreatedAt()) + ","
         "\"model\":\"" + JsonEscape(model) + "\","
         "\"status\":\"completed\","
         "\"output\":[{\"type\":\"message\",\"role\":\"assistant\",\"content\":[{\"type\":\"output_text\",\"text\":\"" +
         JsonEscape(generation.text) + "\"}]}],"
         "\"usage\":{\"input_tokens\":" + std::to_string(generation.input_tokens) +
         ",\"output_tokens\":" + std::to_string(generation.output_tokens) +
         ",\"total_tokens\":" + std::to_string(total) + "},"
         "\"metadata\":{\"trace\":\"" + JsonEscape(generation.trace) + "\"}"
         "}";
}

}  // namespace dissected::api
