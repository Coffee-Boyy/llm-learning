#pragma once

#include <map>
#include <string>
#include <vector>

#include "01_gguf_loading/01_gguf_format.h"

namespace dissected::gguf {

class TensorIndex {
 public:
  explicit TensorIndex(const std::vector<TensorInfo>& tensors);

  const TensorInfo* Find(const std::string& name) const;
  std::map<std::string, std::size_t> CountByType() const;
  std::vector<TensorInfo> Prefix(const std::string& prefix, std::size_t limit) const;

 private:
  std::map<std::string, TensorInfo> tensors_;
};

std::string TensorShapeString(const TensorInfo& tensor);

}  // namespace dissected::gguf
