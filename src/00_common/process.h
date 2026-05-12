#pragma once

#include <string>
#include <vector>

#include "00_common/status.h"

namespace dissected {

struct ProcessResult {
  int exit_code = -1;
  std::string output;
};

Result<ProcessResult> RunCommandCapture(const std::string& command);
std::string JoinCommand(const std::vector<std::string>& args);

}  // namespace dissected
