#pragma once

#include <string>

namespace dissected::runtime {

struct KvCachePlan {
  int context_tokens = 32768;
  int prompt_tokens = 0;
  int generated_tokens = 0;
};

std::string ExplainKvCache(const KvCachePlan& plan);

}  // namespace dissected::runtime
