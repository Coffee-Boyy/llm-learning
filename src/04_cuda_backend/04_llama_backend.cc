#include "04_cuda_backend/04_llama_backend.h"

#include <filesystem>
#include <sstream>

#include "00_common/strings.h"

namespace dissected::cuda_backend {

LlamaBackend::LlamaBackend(LlamaBackendConfig config) : config_(std::move(config)) {}

Status LlamaBackend::Validate() const {
  if (config_.model_path.empty()) {
    return Status::Error("model path is required");
  }
  if (!std::filesystem::exists(config_.model_path)) {
    return Status::Error("model file does not exist: " + config_.model_path);
  }
  if (!config_.llama_cli_path.empty() && !std::filesystem::exists(config_.llama_cli_path)) {
    return Status::Error("llama-cli does not exist: " + config_.llama_cli_path);
  }
  return Status::Ok();
}

std::string LlamaBackend::BuildCliCommand(const std::string& prompt, int max_tokens,
                                          double temperature, double top_p, int top_k,
                                          double min_p, double presence_penalty) const {
  std::ostringstream command;
  command << ShellQuote(config_.llama_cli_path.empty() ? "llama-cli" : config_.llama_cli_path)
          << " -m " << ShellQuote(config_.model_path)
          << " -p " << ShellQuote(prompt)
          << " -n " << max_tokens
          << " -c " << config_.context_size
          << " -ngl " << config_.gpu_layers
          << " --temp " << temperature
          << " --top-p " << top_p
          << " --top-k " << top_k
          << " --min-p " << min_p
          << " --presence-penalty " << presence_penalty
          << " --no-display-prompt";
  if (config_.flash_attention) {
    command << " -fa";
  }
  if (!config_.split_mode.empty()) {
    command << " -sm " << ShellQuote(config_.split_mode);
  }
  return command.str();
}

}  // namespace dissected::cuda_backend
