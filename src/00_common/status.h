#pragma once

#include <string>
#include <utility>

namespace dissected {

class Status {
 public:
  Status() = default;
  static Status Ok() { return Status(); }
  static Status Error(std::string message) {
    Status status;
    status.ok_ = false;
    status.message_ = std::move(message);
    return status;
  }

  bool ok() const { return ok_; }
  const std::string& message() const { return message_; }

 private:
  bool ok_ = true;
  std::string message_;
};

template <typename T>
class Result {
 public:
  Result(T value) : status_(Status::Ok()), value_(std::move(value)) {}
  Result(Status status) : status_(std::move(status)) {}

  bool ok() const { return status_.ok(); }
  const Status& status() const { return status_; }
  const T& value() const { return value_; }
  T& value() { return value_; }

 private:
  Status status_;
  T value_{};
};

}  // namespace dissected
