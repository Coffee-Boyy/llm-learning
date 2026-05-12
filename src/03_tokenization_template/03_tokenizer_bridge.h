#pragma once

#include <string>
#include <vector>

namespace dissected::prompt {

struct TokenDebugView {
  std::vector<int> token_ids;
  std::string note;
};

TokenDebugView EstimateTokenDebugView(const std::string& text);

}  // namespace dissected::prompt
