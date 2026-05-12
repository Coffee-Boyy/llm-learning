#pragma once

#include <string>

#include "00_common/status.h"
#include "02_huggingface_models/02_model_registry.h"

namespace dissected::hf {

struct DownloadRequest {
  ModelPreset preset;
  std::string local_dir = "models/qwen3-8b-gguf";
};

Result<std::string> BuildDownloadCommand(const DownloadRequest& request);
Status DownloadModel(const DownloadRequest& request);

}  // namespace dissected::hf
