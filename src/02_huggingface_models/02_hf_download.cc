#include "02_huggingface_models/02_hf_download.h"

#include "00_common/process.h"

namespace dissected::hf {

Result<std::string> BuildDownloadCommand(const DownloadRequest& request) {
  if (request.preset.repo_id.empty() || request.preset.filename.empty()) {
    return Status::Error("download request is missing repo_id or filename");
  }
  return JoinCommand({"hf", "download", request.preset.repo_id, request.preset.filename,
                      "--local-dir", request.local_dir});
}

Status DownloadModel(const DownloadRequest& request) {
  auto command = BuildDownloadCommand(request);
  if (!command.ok()) return command.status();
  auto result = RunCommandCapture(command.value());
  if (!result.ok()) return result.status();
  if (result.value().exit_code != 0) {
    return Status::Error("hf download failed: " + result.value().output);
  }
  return Status::Ok();
}

}  // namespace dissected::hf
