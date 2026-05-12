#include "08_observability_bench/08_cuda_memory.h"

namespace dissected::observability {

std::string CudaMemoryNote() {
#ifdef DISSECTED_LLM_CUDA
  return "CUDA build path enabled. Detailed allocator stats are emitted by llama.cpp at runtime.";
#else
  return "CUDA build path disabled. Reconfigure with -DDISSECTED_LLM_CUDA=ON for GPU-focused runs.";
#endif
}

}  // namespace dissected::observability
