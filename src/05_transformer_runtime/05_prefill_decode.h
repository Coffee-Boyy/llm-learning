#pragma once

#include <string>

namespace dissected::runtime {

struct PrefillDecodeTrace {
  std::string prompt;
  int estimated_prompt_tokens = 0;
  int max_decode_tokens = 0;
  std::string Summary() const;
};

}  // namespace dissected::runtime
