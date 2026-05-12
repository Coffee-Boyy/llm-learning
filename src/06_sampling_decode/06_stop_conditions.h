#pragma once

#include <string>
#include <vector>

namespace dissected::sampling {

bool ContainsStopString(const std::string& text, const std::vector<std::string>& stops);
std::string TruncateAtStopString(const std::string& text, const std::vector<std::string>& stops);

}  // namespace dissected::sampling
