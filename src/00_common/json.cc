#include "00_common/json.h"

#include <charconv>
#include <cctype>
#include <sstream>

#include "00_common/strings.h"

namespace dissected {
namespace {

const std::string kEmptyString;
const Json::Array kEmptyArray;
const Json::Object kEmptyObject;

class Parser {
 public:
  explicit Parser(const std::string& input) : input_(input) {}

  Result<Json> Parse() {
    SkipWs();
    auto value = ParseValue();
    if (!value.ok()) {
      return value.status();
    }
    SkipWs();
    if (pos_ != input_.size()) {
      return Status::Error("unexpected trailing JSON at byte " + std::to_string(pos_));
    }
    return value;
  }

 private:
  Result<Json> ParseValue() {
    SkipWs();
    if (pos_ >= input_.size()) {
      return Status::Error("unexpected end of JSON");
    }
    char ch = input_[pos_];
    if (ch == '"') return ParseString();
    if (ch == '{') return ParseObject();
    if (ch == '[') return ParseArray();
    if (ch == 't') return ParseLiteral("true", Json(true));
    if (ch == 'f') return ParseLiteral("false", Json(false));
    if (ch == 'n') return ParseLiteral("null", Json(nullptr));
    if (ch == '-' || std::isdigit(static_cast<unsigned char>(ch))) return ParseNumber();
    return Status::Error("unexpected JSON token at byte " + std::to_string(pos_));
  }

  Result<Json> ParseLiteral(const std::string& literal, Json value) {
    if (input_.compare(pos_, literal.size(), literal) != 0) {
      return Status::Error("invalid JSON literal at byte " + std::to_string(pos_));
    }
    pos_ += literal.size();
    return value;
  }

  Result<Json> ParseString() {
    ++pos_;
    std::string value;
    while (pos_ < input_.size()) {
      char ch = input_[pos_++];
      if (ch == '"') {
        return Json(value);
      }
      if (ch == '\\') {
        if (pos_ >= input_.size()) return Status::Error("unterminated JSON escape");
        char esc = input_[pos_++];
        switch (esc) {
          case '"': value += '"'; break;
          case '\\': value += '\\'; break;
          case '/': value += '/'; break;
          case 'b': value += '\b'; break;
          case 'f': value += '\f'; break;
          case 'n': value += '\n'; break;
          case 'r': value += '\r'; break;
          case 't': value += '\t'; break;
          case 'u':
            if (pos_ + 4 > input_.size()) return Status::Error("short JSON unicode escape");
            value += '?';
            pos_ += 4;
            break;
          default:
            return Status::Error("unsupported JSON escape");
        }
      } else {
        value += ch;
      }
    }
    return Status::Error("unterminated JSON string");
  }

  Result<Json> ParseNumber() {
    std::size_t start = pos_;
    if (input_[pos_] == '-') ++pos_;
    while (pos_ < input_.size() && std::isdigit(static_cast<unsigned char>(input_[pos_]))) ++pos_;
    if (pos_ < input_.size() && input_[pos_] == '.') {
      ++pos_;
      while (pos_ < input_.size() && std::isdigit(static_cast<unsigned char>(input_[pos_]))) ++pos_;
    }
    if (pos_ < input_.size() && (input_[pos_] == 'e' || input_[pos_] == 'E')) {
      ++pos_;
      if (pos_ < input_.size() && (input_[pos_] == '+' || input_[pos_] == '-')) ++pos_;
      while (pos_ < input_.size() && std::isdigit(static_cast<unsigned char>(input_[pos_]))) ++pos_;
    }
    double number = 0;
    auto text = input_.substr(start, pos_ - start);
    auto [ptr, ec] = std::from_chars(text.data(), text.data() + text.size(), number);
    if (ec != std::errc()) {
      return Status::Error("invalid JSON number at byte " + std::to_string(start));
    }
    return Json(number);
  }

  Result<Json> ParseArray() {
    ++pos_;
    Json::Array values;
    SkipWs();
    if (Consume(']')) return Json(values);
    while (true) {
      auto value = ParseValue();
      if (!value.ok()) return value.status();
      values.push_back(value.value());
      SkipWs();
      if (Consume(']')) return Json(values);
      if (!Consume(',')) return Status::Error("expected ',' or ']' in JSON array");
    }
  }

  Result<Json> ParseObject() {
    ++pos_;
    Json::Object object;
    SkipWs();
    if (Consume('}')) return Json(object);
    while (true) {
      SkipWs();
      if (pos_ >= input_.size() || input_[pos_] != '"') {
        return Status::Error("expected JSON object key");
      }
      auto key = ParseString();
      if (!key.ok()) return key.status();
      SkipWs();
      if (!Consume(':')) return Status::Error("expected ':' after JSON object key");
      auto value = ParseValue();
      if (!value.ok()) return value.status();
      object[key.value().as_string()] = value.value();
      SkipWs();
      if (Consume('}')) return Json(object);
      if (!Consume(',')) return Status::Error("expected ',' or '}' in JSON object");
    }
  }

  void SkipWs() {
    while (pos_ < input_.size() && std::isspace(static_cast<unsigned char>(input_[pos_]))) {
      ++pos_;
    }
  }

  bool Consume(char ch) {
    if (pos_ < input_.size() && input_[pos_] == ch) {
      ++pos_;
      return true;
    }
    return false;
  }

  const std::string& input_;
  std::size_t pos_ = 0;
};

}  // namespace

bool Json::as_bool(bool fallback) const {
  return is_bool() ? std::get<bool>(value_) : fallback;
}

double Json::as_number(double fallback) const {
  return is_number() ? std::get<double>(value_) : fallback;
}

const std::string& Json::as_string() const {
  return is_string() ? std::get<std::string>(value_) : kEmptyString;
}

const Json::Array& Json::as_array() const {
  return is_array() ? std::get<Array>(value_) : kEmptyArray;
}

const Json::Object& Json::as_object() const {
  return is_object() ? std::get<Object>(value_) : kEmptyObject;
}

const Json* Json::Get(const std::string& key) const {
  if (!is_object()) return nullptr;
  const auto& object = std::get<Object>(value_);
  auto it = object.find(key);
  return it == object.end() ? nullptr : &it->second;
}

std::string Json::Dump() const {
  if (is_null()) return "null";
  if (is_bool()) return std::get<bool>(value_) ? "true" : "false";
  if (is_number()) {
    std::ostringstream out;
    out << std::get<double>(value_);
    return out.str();
  }
  if (is_string()) return "\"" + JsonEscape(std::get<std::string>(value_)) + "\"";
  if (is_array()) {
    std::string out = "[";
    bool first = true;
    for (const auto& value : std::get<Array>(value_)) {
      if (!first) out += ",";
      first = false;
      out += value.Dump();
    }
    out += "]";
    return out;
  }
  std::string out = "{";
  bool first = true;
  for (const auto& [key, value] : std::get<Object>(value_)) {
    if (!first) out += ",";
    first = false;
    out += "\"" + JsonEscape(key) + "\":" + value.Dump();
  }
  out += "}";
  return out;
}

Result<Json> ParseJson(const std::string& input) {
  return Parser(input).Parse();
}

}  // namespace dissected
