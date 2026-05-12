# External optimized backend

This project references llama.cpp/ggml as the optimized inference backend rather
than rewriting CUDA kernels.

Recommended setup:

```bash
git clone https://github.com/ggerganov/llama.cpp external/llama.cpp
cmake -S external/llama.cpp -B external/llama.cpp/build -DGGML_CUDA=ON
cmake --build external/llama.cpp/build -j
```

Then pass the built CLI to the server:

```bash
./build/dissected-llm-server \
  --model models/qwen3-8b-gguf/Qwen3-8B-Q4_K_M.gguf \
  --llama-cli external/llama.cpp/build/bin/llama-cli \
  --gpu-layers 99
```

The stage 04 kernel registry points to the ggml CUDA source files that implement
operations such as quantized matrix multiplication, RoPE, normalization, and
attention. Keeping llama.cpp external makes it easy to update to the latest Qwen3
GGUF support.
