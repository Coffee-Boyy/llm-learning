#pragma once

#include <string>

#include "01_gguf_loading/01_gguf_format.h"

namespace dissected::gguf {

struct QuantTypeInfo {
  std::string name;
  std::string role;
  std::string cuda_kernel_family;
};

QuantTypeInfo DescribeQuantType(TensorType type);

}  // namespace dissected::gguf
