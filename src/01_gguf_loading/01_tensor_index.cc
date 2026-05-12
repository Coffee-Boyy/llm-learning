#include "01_gguf_loading/01_tensor_index.h"

#include <sstream>

namespace dissected::gguf {

TensorIndex::TensorIndex(const std::vector<TensorInfo>& tensors) {
  for (const auto& tensor : tensors) {
    tensors_[tensor.name] = tensor;
  }
}

const TensorInfo* TensorIndex::Find(const std::string& name) const {
  auto it = tensors_.find(name);
  return it == tensors_.end() ? nullptr : &it->second;
}

std::map<std::string, std::size_t> TensorIndex::CountByType() const {
  std::map<std::string, std::size_t> counts;
  for (const auto& [_, tensor] : tensors_) {
    counts[TensorTypeName(tensor.type)]++;
  }
  return counts;
}

std::vector<TensorInfo> TensorIndex::Prefix(const std::string& prefix, std::size_t limit) const {
  std::vector<TensorInfo> matches;
  for (const auto& [name, tensor] : tensors_) {
    if (name.rfind(prefix, 0) == 0) {
      matches.push_back(tensor);
      if (matches.size() >= limit) break;
    }
  }
  return matches;
}

std::string TensorShapeString(const TensorInfo& tensor) {
  std::ostringstream out;
  out << "[";
  for (std::size_t i = 0; i < tensor.dimensions.size(); ++i) {
    if (i) out << " x ";
    out << tensor.dimensions[i];
  }
  out << "]";
  return out.str();
}

}  // namespace dissected::gguf
