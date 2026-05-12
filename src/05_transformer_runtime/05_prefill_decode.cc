#include "05_transformer_runtime/05_prefill_decode.h"

#include <sstream>

namespace dissected::runtime {

std::string PrefillDecodeTrace::Summary() const {
  std::ostringstream out;
  out << "prefill estimated_tokens=" << estimated_prompt_tokens
      << ", decode max_tokens=" << max_decode_tokens;
  return out.str();
}

}  // namespace dissected::runtime
