#include "04_cuda_backend/04_kernel_registry.h"

#include <sstream>

namespace dissected::cuda_backend {

std::vector<KernelReference> KernelRegistry() {
  return {
      {"quantized matmul", "04/05", "ggml/src/ggml-cuda/mmq.cu, mmv.cu, mul-mat*.cu",
       "dominates transformer block runtime for GGUF quantized weights"},
      {"RMSNorm", "04", "ggml/src/ggml-cuda/norm.cu",
       "normalizes activations before attention and MLP projections"},
      {"RoPE", "03/05", "ggml/src/ggml-cuda/rope.cu",
       "applies rotary positional embeddings to query/key vectors"},
      {"flash attention", "05", "ggml/src/ggml-cuda/fattn*.cu",
       "used when llama.cpp selects flash-attention-capable CUDA paths"},
      {"KV cache movement", "05", "llama.cpp KV cache code plus ggml CUDA copy kernels",
       "keeps decoded key/value tensors available across generated tokens"},
      {"sampling", "06", "llama.cpp sampler chain",
       "usually CPU-side over logits after GPU forward pass"},
  };
}

std::string KernelRegistryMarkdown() {
  std::ostringstream out;
  out << "| Operation | Stage | llama.cpp/ggml source | Notes |\n";
  out << "| --- | --- | --- | --- |\n";
  for (const auto& ref : KernelRegistry()) {
    out << "| " << ref.operation << " | " << ref.stage << " | " << ref.llama_cpp_source
        << " | " << ref.notes << " |\n";
  }
  return out.str();
}

}  // namespace dissected::cuda_backend
