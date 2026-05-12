#include "08_observability_bench/08_trace.h"

#include <sstream>

namespace dissected::observability {

void Trace::Mark(const std::string& name) {
  marks_.push_back({name, std::chrono::steady_clock::now()});
}

std::string Trace::Summary() const {
  std::ostringstream out;
  auto previous = start_;
  for (const auto& [name, time] : marks_) {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time - previous).count();
    out << name << "=" << ms << "ms ";
    previous = time;
  }
  return out.str();
}

}  // namespace dissected::observability
