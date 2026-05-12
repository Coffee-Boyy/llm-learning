#include "01_gguf_loading/01_gguf_format.h"

#include <cstring>
#include <fstream>
#include <sstream>
#include <variant>

namespace dissected::gguf {
namespace {

class Reader {
 public:
  explicit Reader(const std::string& path) : input_(path, std::ios::binary) {}

  bool ok() const { return input_.good(); }
  uint64_t position() { return static_cast<uint64_t>(input_.tellg()); }

  template <typename T>
  Result<T> ReadPod() {
    T value{};
    input_.read(reinterpret_cast<char*>(&value), sizeof(T));
    if (!input_) {
      return Status::Error("unexpected end of GGUF file");
    }
    return value;
  }

  Result<std::string> ReadString() {
    auto len = ReadPod<uint64_t>();
    if (!len.ok()) return len.status();
    std::string value(static_cast<std::size_t>(len.value()), '\0');
    input_.read(value.data(), static_cast<std::streamsize>(value.size()));
    if (!input_) {
      return Status::Error("unexpected end while reading GGUF string");
    }
    return value;
  }

 private:
  std::ifstream input_;
};

Result<MetadataValue> ReadMetadataValue(Reader& reader, MetadataType type);

Result<MetadataValue> ReadArray(Reader& reader) {
  auto element_type_raw = reader.ReadPod<uint32_t>();
  if (!element_type_raw.ok()) return element_type_raw.status();
  auto count = reader.ReadPod<uint64_t>();
  if (!count.ok()) return count.status();

  MetadataType element_type = static_cast<MetadataType>(element_type_raw.value());
  MetadataArray array;
  array.reserve(static_cast<std::size_t>(count.value()));
  for (uint64_t i = 0; i < count.value(); ++i) {
    auto value = ReadMetadataValue(reader, element_type);
    if (!value.ok()) return value.status();
    array.push_back(value.value());
  }
  return MetadataValue{MetadataType::Array, array};
}

Result<MetadataValue> ReadMetadataValue(Reader& reader, MetadataType type) {
  switch (type) {
    case MetadataType::UInt8: {
      auto value = reader.ReadPod<uint8_t>();
      if (!value.ok()) return value.status();
      return MetadataValue{type, static_cast<uint64_t>(value.value())};
    }
    case MetadataType::Int8: {
      auto value = reader.ReadPod<int8_t>();
      if (!value.ok()) return value.status();
      return MetadataValue{type, static_cast<int64_t>(value.value())};
    }
    case MetadataType::UInt16: {
      auto value = reader.ReadPod<uint16_t>();
      if (!value.ok()) return value.status();
      return MetadataValue{type, static_cast<uint64_t>(value.value())};
    }
    case MetadataType::Int16: {
      auto value = reader.ReadPod<int16_t>();
      if (!value.ok()) return value.status();
      return MetadataValue{type, static_cast<int64_t>(value.value())};
    }
    case MetadataType::UInt32: {
      auto value = reader.ReadPod<uint32_t>();
      if (!value.ok()) return value.status();
      return MetadataValue{type, static_cast<uint64_t>(value.value())};
    }
    case MetadataType::Int32: {
      auto value = reader.ReadPod<int32_t>();
      if (!value.ok()) return value.status();
      return MetadataValue{type, static_cast<int64_t>(value.value())};
    }
    case MetadataType::Float32: {
      auto value = reader.ReadPod<float>();
      if (!value.ok()) return value.status();
      return MetadataValue{type, static_cast<double>(value.value())};
    }
    case MetadataType::Bool: {
      auto value = reader.ReadPod<uint8_t>();
      if (!value.ok()) return value.status();
      return MetadataValue{type, value.value() != 0};
    }
    case MetadataType::String: {
      auto value = reader.ReadString();
      if (!value.ok()) return value.status();
      return MetadataValue{type, value.value()};
    }
    case MetadataType::Array:
      return ReadArray(reader);
    case MetadataType::UInt64: {
      auto value = reader.ReadPod<uint64_t>();
      if (!value.ok()) return value.status();
      return MetadataValue{type, value.value()};
    }
    case MetadataType::Int64: {
      auto value = reader.ReadPod<int64_t>();
      if (!value.ok()) return value.status();
      return MetadataValue{type, value.value()};
    }
    case MetadataType::Float64: {
      auto value = reader.ReadPod<double>();
      if (!value.ok()) return value.status();
      return MetadataValue{type, value.value()};
    }
  }
  return Status::Error("unsupported GGUF metadata type " + std::to_string(static_cast<uint32_t>(type)));
}

}  // namespace

Result<GgufFile> LoadGgufMetadata(const std::string& path) {
  Reader reader(path);
  if (!reader.ok()) {
    return Status::Error("failed to open GGUF file: " + path);
  }

  auto magic = reader.ReadPod<uint32_t>();
  if (!magic.ok()) return magic.status();
  if (magic.value() != 0x46554747u) {
    return Status::Error("not a GGUF file: bad magic");
  }

  auto version = reader.ReadPod<uint32_t>();
  auto tensor_count = reader.ReadPod<uint64_t>();
  auto metadata_count = reader.ReadPod<uint64_t>();
  if (!version.ok()) return version.status();
  if (!tensor_count.ok()) return tensor_count.status();
  if (!metadata_count.ok()) return metadata_count.status();

  GgufFile file;
  file.version = version.value();
  file.tensor_count = tensor_count.value();
  file.metadata_count = metadata_count.value();

  for (uint64_t i = 0; i < file.metadata_count; ++i) {
    auto key = reader.ReadString();
    if (!key.ok()) return key.status();
    auto type_raw = reader.ReadPod<uint32_t>();
    if (!type_raw.ok()) return type_raw.status();
    auto value = ReadMetadataValue(reader, static_cast<MetadataType>(type_raw.value()));
    if (!value.ok()) return value.status();
    file.metadata[key.value()] = value.value();
  }

  for (uint64_t i = 0; i < file.tensor_count; ++i) {
    auto name = reader.ReadString();
    if (!name.ok()) return name.status();
    auto n_dims = reader.ReadPod<uint32_t>();
    if (!n_dims.ok()) return n_dims.status();
    TensorInfo tensor;
    tensor.name = name.value();
    for (uint32_t dim = 0; dim < n_dims.value(); ++dim) {
      auto value = reader.ReadPod<uint64_t>();
      if (!value.ok()) return value.status();
      tensor.dimensions.push_back(value.value());
    }
    auto type = reader.ReadPod<uint32_t>();
    auto offset = reader.ReadPod<uint64_t>();
    if (!type.ok()) return type.status();
    if (!offset.ok()) return offset.status();
    tensor.type = static_cast<TensorType>(type.value());
    tensor.offset = offset.value();
    file.tensors.push_back(tensor);
  }

  file.tensor_data_start = reader.position();
  return file;
}

std::string MetadataTypeName(MetadataType type) {
  switch (type) {
    case MetadataType::UInt8: return "uint8";
    case MetadataType::Int8: return "int8";
    case MetadataType::UInt16: return "uint16";
    case MetadataType::Int16: return "int16";
    case MetadataType::UInt32: return "uint32";
    case MetadataType::Int32: return "int32";
    case MetadataType::Float32: return "float32";
    case MetadataType::Bool: return "bool";
    case MetadataType::String: return "string";
    case MetadataType::Array: return "array";
    case MetadataType::UInt64: return "uint64";
    case MetadataType::Int64: return "int64";
    case MetadataType::Float64: return "float64";
  }
  return "unknown";
}

std::string TensorTypeName(TensorType type) {
  switch (type) {
    case TensorType::F32: return "F32";
    case TensorType::F16: return "F16";
    case TensorType::Q4_0: return "Q4_0";
    case TensorType::Q4_1: return "Q4_1";
    case TensorType::Q5_0: return "Q5_0";
    case TensorType::Q5_1: return "Q5_1";
    case TensorType::Q8_0: return "Q8_0";
    case TensorType::Q8_1: return "Q8_1";
    case TensorType::Q2_K: return "Q2_K";
    case TensorType::Q3_K: return "Q3_K";
    case TensorType::Q4_K: return "Q4_K";
    case TensorType::Q5_K: return "Q5_K";
    case TensorType::Q6_K: return "Q6_K";
    case TensorType::Q8_K: return "Q8_K";
    case TensorType::I8: return "I8";
    case TensorType::I16: return "I16";
    case TensorType::I32: return "I32";
    case TensorType::I64: return "I64";
    case TensorType::F64: return "F64";
    case TensorType::BF16: return "BF16";
    default: return "quant/unknown";
  }
}

std::string MetadataValueToString(const MetadataValue& value) {
  std::ostringstream out;
  std::visit([&](const auto& item) {
    using T = std::decay_t<decltype(item)>;
    if constexpr (std::is_same_v<T, bool>) {
      out << (item ? "true" : "false");
    } else if constexpr (std::is_same_v<T, std::string>) {
      out << item;
    } else if constexpr (std::is_same_v<T, MetadataArray>) {
      out << "[";
      for (std::size_t i = 0; i < item.size(); ++i) {
        if (i) out << ", ";
        out << MetadataValueToString(item[i]);
      }
      out << "]";
    } else {
      out << item;
    }
  }, value.value);
  return out.str();
}

std::string ArchitectureHint(const GgufFile& file) {
  auto it = file.metadata.find("general.architecture");
  if (it != file.metadata.end() && it->second.type == MetadataType::String) {
    if (const auto* name = std::get_if<std::string>(&it->second.value)) {
      return *name;
    }
  }
  for (const auto& tensor : file.tensors) {
    if (tensor.name.find("blk.") == 0) {
      return "transformer-block (no general.architecture string)";
    }
  }
  return "unknown";
}

}  // namespace dissected::gguf
