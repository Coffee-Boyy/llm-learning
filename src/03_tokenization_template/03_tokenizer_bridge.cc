#include "03_tokenization_template/03_tokenizer_bridge.h"

#include <sstream>

namespace dissected::prompt {

TokenDebugView EstimateTokenDebugView(const std::string& text) {
  std::istringstream input(text);
  std::string piece;
  TokenDebugView view;
  int id = 0;
  while (input >> piece) {
    view.token_ids.push_back(id++);
  }
  view.note = "Educational estimate only. Production tokenization is delegated to llama.cpp.";
  return view;
}

}  // namespace dissected::prompt
