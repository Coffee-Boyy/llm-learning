#include "00_common/process.h"

#include <array>
#include <cstdio>
#include <sys/wait.h>

#include "00_common/strings.h"

namespace dissected {

Result<ProcessResult> RunCommandCapture(const std::string& command) {
  std::array<char, 4096> buffer{};
  std::string output;
  FILE* pipe = popen((command + " 2>&1").c_str(), "r");
  if (pipe == nullptr) {
    return Status::Error("failed to start command: " + command);
  }
  while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
    output += buffer.data();
  }
  int rc = pclose(pipe);
  int exit_code = WIFEXITED(rc) ? WEXITSTATUS(rc) : rc;
  return ProcessResult{exit_code, output};
}

std::string JoinCommand(const std::vector<std::string>& args) {
  std::string command;
  for (const auto& arg : args) {
    if (!command.empty()) {
      command += " ";
    }
    command += ShellQuote(arg);
  }
  return command;
}

}  // namespace dissected
