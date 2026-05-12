#include <iostream>

#include "01_gguf_loading/01_gguf_format.h"
#include "01_gguf_loading/01_quant_types.h"
#include "01_gguf_loading/01_tensor_index.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "usage: dissected-gguf-inspect <model.gguf>\n";
    return 2;
  }

  auto loaded = dissected::gguf::LoadGgufMetadata(argv[1]);
  if (!loaded.ok()) {
    std::cerr << "error: " << loaded.status().message() << "\n";
    return 1;
  }

  const auto& file = loaded.value();
  std::cout << "GGUF version: " << file.version << "\n";
  std::cout << "Metadata entries: " << file.metadata_count << "\n";
  std::cout << "Tensors: " << file.tensor_count << "\n";
  std::cout << "Tensor data starts at byte: " << file.tensor_data_start << "\n\n";

  std::cout << "Selected metadata:\n";
  for (const auto& key : {"general.architecture", "general.name", "tokenizer.chat_template",
                          "qwen3.context_length", "llama.context_length"}) {
    auto it = file.metadata.find(key);
    if (it != file.metadata.end()) {
      std::cout << "  " << key << " = " << dissected::gguf::MetadataValueToString(it->second) << "\n";
    }
  }

  dissected::gguf::TensorIndex index(file.tensors);
  std::cout << "\nTensor type counts:\n";
  for (const auto& [type, count] : index.CountByType()) {
    std::cout << "  " << type << ": " << count << "\n";
  }

  std::cout << "\nFirst tensors:\n";
  for (std::size_t i = 0; i < file.tensors.size() && i < 12; ++i) {
    const auto& tensor = file.tensors[i];
    auto quant = dissected::gguf::DescribeQuantType(tensor.type);
    std::cout << "  " << tensor.name << " " << dissected::gguf::TensorShapeString(tensor)
              << " " << quant.name << " offset=" << tensor.offset << "\n";
  }

  return 0;
}
