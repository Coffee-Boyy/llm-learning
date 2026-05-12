#pragma once

#include <string>
#include <vector>

namespace dissected::prompt {

struct ChatMessage {
  std::string role;
  std::string content;
};

enum class ThinkingMode {
  Auto,
  Thinking,
  NoThinking,
};

ThinkingMode DetectThinkingMode(const std::string& text);
std::string ApplyQwen3Template(const std::vector<ChatMessage>& messages, ThinkingMode mode);

}  // namespace dissected::prompt
