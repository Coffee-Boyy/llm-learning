#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>

#include "00_common/status.h"

namespace dissected {

class Json {
 public:
  using Array = std::vector<Json>;
  using Object = std::map<std::string, Json>;
  using Value = std::variant<std::nullptr_t, bool, double, std::string, Array, Object>;

  Json() : value_(nullptr) {}
  Json(std::nullptr_t) : value_(nullptr) {}
  Json(bool value) : value_(value) {}
  Json(double value) : value_(value) {}
  Json(std::string value) : value_(std::move(value)) {}
  Json(Array value) : value_(std::move(value)) {}
  Json(Object value) : value_(std::move(value)) {}

  bool is_null() const { return std::holds_alternative<std::nullptr_t>(value_); }
  bool is_bool() const { return std::holds_alternative<bool>(value_); }
  bool is_number() const { return std::holds_alternative<double>(value_); }
  bool is_string() const { return std::holds_alternative<std::string>(value_); }
  bool is_array() const { return std::holds_alternative<Array>(value_); }
  bool is_object() const { return std::holds_alternative<Object>(value_); }

  bool as_bool(bool fallback = false) const;
  double as_number(double fallback = 0) const;
  const std::string& as_string() const;
  const Array& as_array() const;
  const Object& as_object() const;

  const Json* Get(const std::string& key) const;
  std::string Dump() const;

 private:
  Value value_;
};

Result<Json> ParseJson(const std::string& input);

}  // namespace dissected
