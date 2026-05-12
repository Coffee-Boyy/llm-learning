#include "05_transformer_runtime/05_kv_cache.h"

#include <sstream>

namespace dissected::runtime {

std::string ExplainKvCache(const KvCachePlan& plan) {
  std::ostringstream out;
  out << "KV cache budget: context=" << plan.context_tokens
      << " prompt=" << plan.prompt_tokens
      << " generated=" << plan.generated_tokens
      << ". Prefill writes keys/values for prompt tokens; decode appends one token at a time.";
  return out.str();
}

}  // namespace dissected::runtime
