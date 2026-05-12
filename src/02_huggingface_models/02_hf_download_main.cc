#include <iostream>

#include "02_huggingface_models/02_hf_download.h"

int main(int argc, char** argv) {
  if (argc >= 2 && std::string(argv[1]) == "--list") {
    for (const auto& preset : dissected::hf::Qwen3Presets()) {
      std::cout << preset.name << " -> " << preset.repo_id << "/" << preset.filename
                << " (" << preset.notes << ")\n";
    }
    return 0;
  }

  std::string preset_name = argc >= 2 ? argv[1] : dissected::hf::DefaultQwen3Preset().name;
  std::string local_dir = argc >= 3 ? argv[2] : "models/qwen3-8b-gguf";
  const auto* preset = dissected::hf::FindPreset(preset_name);
  if (preset == nullptr) {
    std::cerr << "unknown preset: " << preset_name << "\n";
    std::cerr << "run with --list to see presets\n";
    return 2;
  }

  dissected::hf::DownloadRequest request{*preset, local_dir};
  auto command = dissected::hf::BuildDownloadCommand(request);
  if (!command.ok()) {
    std::cerr << command.status().message() << "\n";
    return 1;
  }
  std::cout << "running: " << command.value() << "\n";
  auto status = dissected::hf::DownloadModel(request);
  if (!status.ok()) {
    std::cerr << status.message() << "\n";
    return 1;
  }
  return 0;
}
