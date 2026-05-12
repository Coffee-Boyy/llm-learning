#include <cassert>
#include <string>

#include "07_responses_api/07_json_validation.h"
#include "07_responses_api/07_responses_types.h"
#include "07_responses_api/07_sse_stream.h"

int main() {
  auto parsed = dissected::api::ParseResponsesRequest(
      "{\"model\":\"qwen3-8b-q4_k_m\",\"input\":\"hello /no_think\",\"stream\":true,"
      "\"temperature\":0.7,\"max_output_tokens\":12}");
  assert(parsed.ok());
  assert(parsed.value().model == "qwen3-8b-q4_k_m");
  assert(parsed.value().input == "hello /no_think");
  assert(parsed.value().stream);
  assert(parsed.value().max_output_tokens == 12);

  std::string feature;
  assert(dissected::api::HasUnsupportedHostedFeature("{\"input\":\"x\",\"tools\":[]}", &feature));
  assert(feature == "tools");

  auto sse = dissected::api::BuildSseResponse("model", "hello");
  assert(sse.find("response.output_text.delta") != std::string::npos);
  assert(sse.find("[DONE]") != std::string::npos);
  return 0;
}
