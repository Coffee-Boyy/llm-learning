#pragma once

#include <string>
#include <vector>

namespace dissected::api {

std::string BuildSseResponse(const std::string& model, const std::string& text);
std::vector<std::string> SplitForSse(const std::string& text);

}  // namespace dissected::api
