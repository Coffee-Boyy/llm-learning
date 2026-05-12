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

std::string FormatSseTextDelta(const std::string& model, const std::string& delta) {
  std::ostringstream out;
  out << "event: response.output_text.delta\n";
  out << "data: {\"type\":\"response.output_text.delta\",\"model\":\"" << JsonEscape(model)
      << "\",\"delta\":\"" << JsonEscape(delta) << "\"}\n\n";
  return out.str();
}

std::string FormatSseCompleted() {
  return "event: response.completed\n"
         "data: {\"type\":\"response.completed\"}\n\n";
}

std::string FormatSseDone() {
  return "data: [DONE]\n\n";
}

std::string BuildSseResponse(const std::string& model, const std::string& text) {
  std::ostringstream out;
  for (const auto& chunk : SplitForSse(text)) {
    out << FormatSseTextDelta(model, chunk);
  }
  out << FormatSseCompleted();
  out << FormatSseDone();
  return out.str();
}

}  // namespace dissected::api
