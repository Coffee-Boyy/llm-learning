#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "00_common/status.h"

namespace dissected::gguf {

enum class MetadataType : uint32_t {
  UInt8 = 0,
  Int8 = 1,
  UInt16 = 2,
  Int16 = 3,
  UInt32 = 4,
  Int32 = 5,
  Float32 = 6,
  Bool = 7,
  String = 8,
  Array = 9,
  UInt64 = 10,
  Int64 = 11,
  Float64 = 12,
};

enum class TensorType : uint32_t {
  F32 = 0,
  F16 = 1,
  Q4_0 = 2,
  Q4_1 = 3,
  Q5_0 = 6,
  Q5_1 = 7,
  Q8_0 = 8,
  Q8_1 = 9,
  Q2_K = 10,
  Q3_K = 11,
  Q4_K = 12,
  Q5_K = 13,
  Q6_K = 14,
  Q8_K = 15,
  IQ2_XXS = 16,
  IQ2_XS = 17,
  IQ3_XXS = 18,
  IQ1_S = 19,
  IQ4_NL = 20,
  IQ3_S = 21,
  IQ2_S = 22,
  IQ4_XS = 23,
  I8 = 24,
  I16 = 25,
  I32 = 26,
  I64 = 27,
  F64 = 28,
  IQ1_M = 29,
  BF16 = 30,
  Unknown = 0xffffffffu,
};

struct MetadataValue;
using MetadataArray = std::vector<MetadataValue>;
using MetadataScalar = std::variant<uint64_t, int64_t, double, bool, std::string, MetadataArray>;

struct MetadataValue {
  MetadataType type = MetadataType::String;
  MetadataScalar value = std::string();
};

struct TensorInfo {
  std::string name;
  std::vector<uint64_t> dimensions;
  TensorType type = TensorType::Unknown;
  uint64_t offset = 0;
};

struct GgufFile {
  uint32_t version = 0;
  uint64_t tensor_count = 0;
  uint64_t metadata_count = 0;
  std::map<std::string, MetadataValue> metadata;
  std::vector<TensorInfo> tensors;
  uint64_t tensor_data_start = 0;
};

Result<GgufFile> LoadGgufMetadata(const std::string& path);
std::string MetadataTypeName(MetadataType type);
std::string TensorTypeName(TensorType type);
std::string MetadataValueToString(const MetadataValue& value);
std::string ArchitectureHint(const GgufFile& file);

}  // namespace dissected::gguf
