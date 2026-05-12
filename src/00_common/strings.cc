#include "00_common/strings.h"

#include <cctype>
#include <sstream>

namespace dissected {

std::string Trim(std::string value) {
  while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front()))) {
    value.erase(value.begin());
  }
  while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back()))) {
    value.pop_back();
  }
  return value;
}

std::string ShellQuote(const std::string& value) {
  std::string out = "'";
  for (char ch : value) {
    if (ch == '\'') {
      out += "'\\''";
    } else {
      out += ch;
    }
  }
  out += "'";
  return out;
}

std::vector<std::string> SplitWhitespace(const std::string& value) {
  std::istringstream input(value);
  std::vector<std::string> parts;
  std::string part;
  while (input >> part) {
    parts.push_back(part);
  }
  return parts;
}

bool StartsWith(const std::string& value, const std::string& prefix) {
  return value.rfind(prefix, 0) == 0;
}

std::string ReplaceAll(std::string value, const std::string& from, const std::string& to) {
  if (from.empty()) {
    return value;
  }
  std::size_t pos = 0;
  while ((pos = value.find(from, pos)) != std::string::npos) {
    value.replace(pos, from.size(), to);
    pos += to.size();
  }
  return value;
}

std::string JsonEscape(const std::string& value) {
  std::string out;
  for (unsigned char ch : value) {
    switch (ch) {
      case '"': out += "\\\""; break;
      case '\\': out += "\\\\"; break;
      case '\b': out += "\\b"; break;
      case '\f': out += "\\f"; break;
      case '\n': out += "\\n"; break;
      case '\r': out += "\\r"; break;
      case '\t': out += "\\t"; break;
      default:
        if (ch < 0x20) {
          const char* hex = "0123456789abcdef";
          out += "\\u00";
          out += hex[(ch >> 4) & 0x0f];
          out += hex[ch & 0x0f];
        } else {
          out += static_cast<char>(ch);
        }
    }
  }
  return out;
}

}  // namespace dissected
