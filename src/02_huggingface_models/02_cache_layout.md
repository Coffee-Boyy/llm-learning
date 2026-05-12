# Stage 02 cache layout

Model files are not committed. Keep them below `models/`:

```text
models/
  qwen3-8b-gguf/
    Qwen3-8B-Q4_K_M.gguf
```

## Hugging Face Hub layout

The bundled downloader shells out to the Hugging Face CLI (`hf download ...`). The CLI respects the usual Hub environment variables:

- `HF_HOME` — root for Hub configuration and default cache roots.
- `HF_HUB_CACHE` — explicit cache directory for downloaded blobs (falls back under `HF_HOME`).
- `HF_TOKEN` / `HUGGING_FACE_HUB_TOKEN` — access tokens for gated models (Qwen3 GGUF repos are generally open, but keep this handy).

With defaults, a file typically lands under `~/.cache/huggingface/hub/...` while `--local-dir` (used by `dissected-hf-download`) mirrors the filename into your chosen tree (for example `models/qwen3-8b-gguf/`).

## Download commands

Download default preset (`qwen3-8b-q4_k_m`):

```bash
./build/dissected-hf-download
```

List presets:

```bash
./build/dissected-hf-download --list
```

Download a larger quantization:

```bash
./build/dissected-hf-download qwen3-8b-q8_0 models/qwen3-8b-gguf
```

Direct CLI equivalent (useful in CI or air-gapped mirrors):

```bash
hf download Qwen/Qwen3-8B-GGUF Qwen3-8B-Q4_K_M.gguf --local-dir models/qwen3-8b-gguf
```

## Direct URL pattern

Hub files resolve to versioned URLs of the form:

`https://huggingface.co/Qwen/Qwen3-8B-GGUF/resolve/main/Qwen3-8B-Q4_K_M.gguf`

Prefer `hf download` so resume, checksums, and cache deduplication stay aligned with the Hub client.

## Runtime environment variables

Downstream binaries (`dissected-llm-server`, `dissected-llm-complete`, `dissected-llm-bench`) read:

- `DISSECTED_LLM_MODEL` — default GGUF path when `--model` is omitted.
- `DISSECTED_LLM_LLAMA_CLI` — path to `llama-cli` from your CUDA-enabled llama.cpp build.
