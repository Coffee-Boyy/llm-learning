#include "03_tokenization_template/03_qwen3_template.h"

namespace dissected::prompt {

ThinkingMode DetectThinkingMode(const std::string& text) {
  auto think = text.rfind("/think");
  auto no_think = text.rfind("/no_think");
  if (think == std::string::npos && no_think == std::string::npos) {
    return ThinkingMode::Auto;
  }
  if (no_think != std::string::npos && (think == std::string::npos || no_think > think)) {
    return ThinkingMode::NoThinking;
  }
  return ThinkingMode::Thinking;
}

std::string ApplyQwen3Template(const std::vector<ChatMessage>& messages, ThinkingMode mode) {
  std::string prompt;
  for (const auto& message : messages) {
    prompt += "<|im_start|>" + message.role + "\n" + message.content + "<|im_end|>\n";
  }
  prompt += "<|im_start|>assistant\n";
  if (mode == ThinkingMode::NoThinking) {
    // Qwen3's public template emits empty thinking delimiters for no-thinking mode.
    prompt += "<think>\n\n</think>\n\n";
  }
  return prompt;
}

}  // namespace dissected::prompt
