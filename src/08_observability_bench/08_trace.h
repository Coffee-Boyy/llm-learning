#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace dissected::observability {

class Trace {
 public:
  void Mark(const std::string& name);
  std::string Summary() const;

 private:
  std::chrono::steady_clock::time_point start_ = std::chrono::steady_clock::now();
  std::vector<std::pair<std::string, std::chrono::steady_clock::time_point>> marks_;
};

}  // namespace dissected::observability
