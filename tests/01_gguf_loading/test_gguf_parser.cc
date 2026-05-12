#include <cassert>
#include <cstdint>
#include <fstream>
#include <string>

#include "01_gguf_loading/01_gguf_format.h"
#include "01_gguf_loading/01_tensor_index.h"

namespace {

template <typename T>
void Write(std::ofstream& out, T value) {
  out.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

void WriteString(std::ofstream& out, const std::string& value) {
  Write<uint64_t>(out, value.size());
  out.write(value.data(), static_cast<std::streamsize>(value.size()));
}

}  // namespace

int main() {
  const std::string path = "tiny-test.gguf";
  {
    std::ofstream out(path, std::ios::binary);
    Write<uint32_t>(out, 0x46554747u);
    Write<uint32_t>(out, 3);
    Write<uint64_t>(out, 1);
    Write<uint64_t>(out, 1);
    WriteString(out, "general.architecture");
    Write<uint32_t>(out, static_cast<uint32_t>(dissected::gguf::MetadataType::String));
    WriteString(out, "qwen3");
    WriteString(out, "blk.0.attn_q.weight");
    Write<uint32_t>(out, 2);
    Write<uint64_t>(out, 128);
    Write<uint64_t>(out, 256);
    Write<uint32_t>(out, static_cast<uint32_t>(dissected::gguf::TensorType::Q4_K));
    Write<uint64_t>(out, 0);
  }

  auto loaded = dissected::gguf::LoadGgufMetadata(path);
  assert(loaded.ok());
  assert(loaded.value().version == 3);
  assert(loaded.value().tensor_count == 1);
  assert(dissected::gguf::ArchitectureHint(loaded.value()) == "qwen3");
  assert(loaded.value().metadata.at("general.architecture").type == dissected::gguf::MetadataType::String);
  assert(loaded.value().tensors[0].name == "blk.0.attn_q.weight");
  assert(dissected::gguf::TensorTypeName(loaded.value().tensors[0].type) == "Q4_K");
  dissected::gguf::TensorIndex index(loaded.value().tensors);
  assert(index.Find("blk.0.attn_q.weight") != nullptr);
  assert(index.CountByType().at("Q4_K") == 1);
  assert(dissected::gguf::TensorShapeString(loaded.value().tensors[0]) == "[128 x 256]");
  std::remove(path.c_str());
  return 0;
}
