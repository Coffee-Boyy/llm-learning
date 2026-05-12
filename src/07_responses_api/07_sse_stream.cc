#include "07_responses_api/07_sse_stream.h"

#include <sstream>

#include "00_common/strings.h"

namespace dissected::api {

std::vector<std::string> SplitForSse(const std::string& text) {
  std::vector<std::string> chunks;
  constexpr std::size_t kChunk = 64;
  for (std::size_t i = 0; i < text.size(); i += kChunk) {
    chunks.push_back(text.substr(i, kChunk));
  }
  if (chunks.empty()) chunks.push_back("");
  return chunks;
}

std::string BuildSseResponse(const std::string& model, const std::string& text) {
  std::ostringstream out;
  for (const auto& chunk : SplitForSse(text)) {
    out << "event: response.output_text.delta\n";
    out << "data: {\"type\":\"response.output_text.delta\",\"model\":\"" << JsonEscape(model)
        << "\",\"delta\":\"" << JsonEscape(chunk) << "\"}\n\n";
  }
  out << "event: response.completed\n";
  out << "data: {\"type\":\"response.completed\"}\n\n";
  out << "data: [DONE]\n\n";
  return out.str();
}

}  // namespace dissected::api
