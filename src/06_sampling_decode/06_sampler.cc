#include "06_sampling_decode/06_sampler.h"

namespace dissected::sampling {

SamplingConfig DefaultsForThinkingMode(prompt::ThinkingMode mode) {
  SamplingConfig config;
  if (mode == prompt::ThinkingMode::Thinking) {
    config.temperature = 0.6;
    config.top_p = 0.95;
  } else {
    config.temperature = 0.7;
    config.top_p = 0.8;
  }
  config.top_k = 20;
  config.min_p = 0.0;
  config.presence_penalty = 1.5;
  return config;
}

}  // namespace dissected::sampling
