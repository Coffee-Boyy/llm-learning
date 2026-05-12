#pragma once

#include <string>
#include <vector>

namespace dissected::hf {

struct ModelPreset {
  std::string name;
  std::string repo_id;
  std::string filename;
  std::string quantization;
  std::string notes;
};

std::vector<ModelPreset> Qwen3Presets();
ModelPreset DefaultQwen3Preset();
const ModelPreset* FindPreset(const std::string& name);

}  // namespace dissected::hf
