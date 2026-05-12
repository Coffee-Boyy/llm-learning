#include "05_transformer_runtime/05_model_context.h"

namespace dissected::runtime {

ModelContext::ModelContext(ModelContextConfig config) : config_(std::move(config)) {}

cuda_backend::LlamaBackend ModelContext::Backend() const {
  return cuda_backend::LlamaBackend({config_.model_path, config_.llama_cli_path, config_.gpu_layers,
                                     config_.context_size, true, "row"});
}

}  // namespace dissected::runtime
