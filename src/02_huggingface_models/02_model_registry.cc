#include "02_huggingface_models/02_model_registry.h"

namespace dissected::hf {

std::vector<ModelPreset> Qwen3Presets() {
  return {
      {"qwen3-8b-q4_k_m", "Qwen/Qwen3-8B-GGUF", "Qwen3-8B-Q4_K_M.gguf", "Q4_K_M",
       "balanced default for local GPU experimentation"},
      {"qwen3-8b-q5_0", "Qwen/Qwen3-8B-GGUF", "Qwen3-8B-Q5_0.gguf", "Q5_0",
       "legacy 5-bit quantization"},
      {"qwen3-8b-q5_k_m", "Qwen/Qwen3-8B-GGUF", "Qwen3-8B-Q5_K_M.gguf", "Q5_K_M",
       "higher quality K-quant"},
      {"qwen3-8b-q6_k", "Qwen/Qwen3-8B-GGUF", "Qwen3-8B-Q6_K.gguf", "Q6_K",
       "higher memory, higher quality"},
      {"qwen3-8b-q8_0", "Qwen/Qwen3-8B-GGUF", "Qwen3-8B-Q8_0.gguf", "Q8_0",
       "largest listed quantized preset, useful for quality comparisons"},
  };
}

ModelPreset DefaultQwen3Preset() {
  return Qwen3Presets().front();
}

const ModelPreset* FindPreset(const std::string& name) {
  static const std::vector<ModelPreset> presets = Qwen3Presets();
  for (const auto& preset : presets) {
    if (preset.name == name || preset.quantization == name || preset.filename == name) {
      return &preset;
    }
  }
  return nullptr;
}

}  // namespace dissected::hf
