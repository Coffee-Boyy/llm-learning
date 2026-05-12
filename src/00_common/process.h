#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <vector>

#include "00_common/status.h"

namespace dissected {

struct ProcessResult {
  int exit_code = -1;
  std::string output;
};

Result<ProcessResult> RunCommandCapture(const std::string& command);
Status RunCommandStream(const std::string& command,
                        const std::function<void(std::string_view chunk)>& on_chunk);
std::string JoinCommand(const std::vector<std::string>& args);

}  // namespace dissected
