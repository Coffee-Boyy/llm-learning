#pragma once

#include <string>

#include "00_common/status.h"

namespace dissected::cuda_backend {

struct LlamaBackendConfig {
  std::string model_path;
  std::string llama_cli_path;
  int gpu_layers = 99;
  int context_size = 32768;
  bool flash_attention = true;
  std::string split_mode = "row";
};

class LlamaBackend {
 public:
  explicit LlamaBackend(LlamaBackendConfig config);

  const LlamaBackendConfig& config() const { return config_; }
  Status Validate() const;
  std::string BuildCliCommand(const std::string& prompt, int max_tokens, double temperature,
                              double top_p, int top_k, double min_p,
                              double presence_penalty) const;

 private:
  LlamaBackendConfig config_;
};

}  // namespace dissected::cuda_backend
