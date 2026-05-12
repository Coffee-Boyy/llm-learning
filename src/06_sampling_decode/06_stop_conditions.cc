#include "06_sampling_decode/06_stop_conditions.h"

namespace dissected::sampling {

bool ContainsStopString(const std::string& text, const std::vector<std::string>& stops) {
  for (const auto& stop : stops) {
    if (!stop.empty() && text.find(stop) != std::string::npos) {
      return true;
    }
  }
  return false;
}

std::string TruncateAtStopString(const std::string& text, const std::vector<std::string>& stops) {
  std::size_t best = std::string::npos;
  for (const auto& stop : stops) {
    if (stop.empty()) continue;
    auto pos = text.find(stop);
    if (pos != std::string::npos && (best == std::string::npos || pos < best)) {
      best = pos;
    }
  }
  return best == std::string::npos ? text : text.substr(0, best);
}

}  // namespace dissected::sampling
