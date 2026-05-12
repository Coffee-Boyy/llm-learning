#pragma once

#include <string>
#include <vector>

namespace dissected {

std::string Trim(std::string value);
std::string ShellQuote(const std::string& value);
std::vector<std::string> SplitWhitespace(const std::string& value);
bool StartsWith(const std::string& value, const std::string& prefix);
std::string ReplaceAll(std::string value, const std::string& from, const std::string& to);
std::string JsonEscape(const std::string& value);

}  // namespace dissected
