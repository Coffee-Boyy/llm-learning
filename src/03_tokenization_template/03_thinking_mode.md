# Qwen3 thinking mode

Qwen3 supports switching between reasoning-heavy thinking mode and efficient
dialogue mode using `/think` and `/no_think` in the prompt or system message.

Recommended defaults from Qwen's GGUF card:

- thinking: temperature `0.6`, top-p `0.95`, top-k `20`, min-p `0`, presence
  penalty `1.5`
- non-thinking: temperature `0.7`, top-p `0.8`, top-k `20`, min-p `0`,
  presence penalty `1.5`

For future multi-turn support, keep only the final assistant answer in history.
Do not feed the internal `<think>...</think>` reasoning block back into the
conversation context.
