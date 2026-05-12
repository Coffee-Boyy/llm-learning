#pragma once

#include "03_tokenization_template/03_qwen3_template.h"

namespace dissected::sampling {

struct SamplingConfig {
  double temperature = 0.7;
  double top_p = 0.8;
  int top_k = 20;
  double min_p = 0.0;
  double presence_penalty = 1.5;
  int max_output_tokens = 256;
  unsigned seed = 0;
};

SamplingConfig DefaultsForThinkingMode(prompt::ThinkingMode mode);

}  // namespace dissected::sampling
