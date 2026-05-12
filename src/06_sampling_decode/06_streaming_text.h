#pragma once

#include <string>
#include <vector>

namespace dissected::sampling {

class Utf8Chunker {
 public:
  std::vector<std::string> Push(std::string bytes);
  std::vector<std::string> Flush();

 private:
  std::string pending_;
};

}  // namespace dissected::sampling
