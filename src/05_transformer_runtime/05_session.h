#pragma once

#include <functional>
#include <string>
#include <string_view>

#include "05_transformer_runtime/05_model_context.h"
#include "06_sampling_decode/06_sampler.h"
#include "00_common/status.h"

namespace dissected::runtime {

struct GenerationRequest {
  std::string input;
  std::string instructions;
  sampling::SamplingConfig sampling;
};

struct GenerationResult {
  std::string text;
  int input_tokens = 0;
  int output_tokens = 0;
  std::string trace;
};

class GenerationSession {
 public:
  explicit GenerationSession(ModelContext context);
  Status ValidateBackend() const;
  Result<GenerationResult> Generate(const GenerationRequest& request) const;
  Result<GenerationResult> GenerateStreaming(
      const GenerationRequest& request,
      const std::function<void(std::string_view utf8_chunk)>& on_utf8_chunk) const;

 private:
  ModelContext context_;
};

}  // namespace dissected::runtime
