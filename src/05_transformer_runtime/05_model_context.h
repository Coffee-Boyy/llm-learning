#pragma once

#include <string>

#include "04_cuda_backend/04_llama_backend.h"

namespace dissected::runtime {

struct ModelContextConfig {
  std::string model_path;
  std::string model_name = "qwen3-8b-q4_k_m";
  std::string llama_cli_path;
  int gpu_layers = 99;
  int context_size = 32768;
};

class ModelContext {
 public:
  explicit ModelContext(ModelContextConfig config);

  const ModelContextConfig& config() const { return config_; }
  cuda_backend::LlamaBackend Backend() const;

 private:
  ModelContextConfig config_;
};

}  // namespace dissected::runtime
