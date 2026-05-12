#pragma once

#include <string>
#include <vector>

namespace dissected::cuda_backend {

struct KernelReference {
  std::string operation;
  std::string stage;
  std::string llama_cpp_source;
  std::string notes;
};

std::vector<KernelReference> KernelRegistry();
std::string KernelRegistryMarkdown();

}  // namespace dissected::cuda_backend
