#include "05_transformer_runtime/05_session.h"

#include <sstream>

#include "00_common/process.h"
#include "03_tokenization_template/03_qwen3_template.h"
#include "03_tokenization_template/03_tokenizer_bridge.h"
#include "05_transformer_runtime/05_kv_cache.h"
#include "05_transformer_runtime/05_prefill_decode.h"
#include "06_sampling_decode/06_streaming_text.h"

namespace dissected::runtime {

GenerationSession::GenerationSession(ModelContext context) : context_(std::move(context)) {}

Status GenerationSession::ValidateBackend() const {
  if (context_.config().llama_cli_path.empty()) {
    return Status::Ok();
  }
  auto backend = context_.Backend();
  return backend.Validate();
}

Result<GenerationResult> GenerationSession::Generate(const GenerationRequest& request) const {
  std::vector<prompt::ChatMessage> messages;
  if (!request.instructions.empty()) {
    messages.push_back({"system", request.instructions});
  }
  messages.push_back({"user", request.input});

  auto mode = prompt::DetectThinkingMode(request.instructions + "\n" + request.input);
  std::string rendered_prompt = prompt::ApplyQwen3Template(messages, mode);
  auto tokens = prompt::EstimateTokenDebugView(rendered_prompt);

  PrefillDecodeTrace prefill{rendered_prompt, static_cast<int>(tokens.token_ids.size()),
                             request.sampling.max_output_tokens};
  std::ostringstream trace;
  trace << prefill.Summary() << "\n"
        << ExplainKvCache({context_.config().context_size, prefill.estimated_prompt_tokens, 0});

  GenerationResult result;
  result.input_tokens = prefill.estimated_prompt_tokens;
  result.trace = trace.str();

  if (!context_.config().llama_cli_path.empty()) {
    auto backend = context_.Backend();
    auto status = backend.Validate();
    if (!status.ok()) return status;
    auto command = backend.BuildCliCommand(rendered_prompt, request.sampling.max_output_tokens,
                                           request.sampling.temperature, request.sampling.top_p,
                                           request.sampling.top_k, request.sampling.min_p,
                                           request.sampling.presence_penalty);
    auto run = RunCommandCapture(command);
    if (!run.ok()) return run.status();
    if (run.value().exit_code != 0) {
      return Status::Error("llama.cpp generation failed: " + run.value().output);
    }
    result.text = run.value().output;
  } else {
    result.text = "Educational runtime path is active. Configure --llama-cli to run Qwen3 GGUF "
                  "through llama.cpp CUDA kernels. Prompt bytes=" +
                  std::to_string(rendered_prompt.size()) + ".";
  }

  result.output_tokens = static_cast<int>(prompt::EstimateTokenDebugView(result.text).token_ids.size());
  return result;
}

Result<GenerationResult> GenerationSession::GenerateStreaming(
    const GenerationRequest& request,
    const std::function<void(std::string_view utf8_chunk)>& on_utf8_chunk) const {
  std::vector<prompt::ChatMessage> messages;
  if (!request.instructions.empty()) {
    messages.push_back({"system", request.instructions});
  }
  messages.push_back({"user", request.input});

  auto mode = prompt::DetectThinkingMode(request.instructions + "\n" + request.input);
  std::string rendered_prompt = prompt::ApplyQwen3Template(messages, mode);
  auto tokens = prompt::EstimateTokenDebugView(rendered_prompt);

  PrefillDecodeTrace prefill{rendered_prompt, static_cast<int>(tokens.token_ids.size()),
                             request.sampling.max_output_tokens};
  std::ostringstream trace;
  trace << prefill.Summary() << "\n"
        << ExplainKvCache({context_.config().context_size, prefill.estimated_prompt_tokens, 0});

  GenerationResult result;
  result.input_tokens = prefill.estimated_prompt_tokens;
  result.trace = trace.str();

  if (!context_.config().llama_cli_path.empty()) {
    auto backend = context_.Backend();
    auto status = backend.Validate();
    if (!status.ok()) return status;
    auto command = backend.BuildCliCommand(rendered_prompt, request.sampling.max_output_tokens,
                                           request.sampling.temperature, request.sampling.top_p,
                                           request.sampling.top_k, request.sampling.min_p,
                                           request.sampling.presence_penalty);
    sampling::Utf8Chunker chunker;
    auto stream_status = RunCommandStream(command, [&](std::string_view chunk) {
      result.text.append(chunk.data(), chunk.size());
      for (const auto& piece : chunker.Push(std::string(chunk))) {
        on_utf8_chunk(piece);
      }
    });
    if (!stream_status.ok()) return stream_status;
    for (const auto& piece : chunker.Flush()) {
      on_utf8_chunk(piece);
    }
  } else {
    result.text = "Educational runtime path is active. Configure --llama-cli to run Qwen3 GGUF "
                  "through llama.cpp CUDA kernels. Prompt bytes=" +
                  std::to_string(rendered_prompt.size()) + ".";
    on_utf8_chunk(result.text);
  }

  result.output_tokens = static_cast<int>(prompt::EstimateTokenDebugView(result.text).token_ids.size());
  return result;
}

}  // namespace dissected::runtime
