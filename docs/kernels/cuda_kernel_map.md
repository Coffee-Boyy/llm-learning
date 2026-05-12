# CUDA kernel map

The runtime stage calls into llama.cpp through `llama-cli` by default. A direct
C API adapter can be added behind the same stage 04 boundary later, but the
learning map stays the same.

| Transformer operation | Educational stage | llama.cpp/ggml CUDA area to inspect |
| --- | --- | --- |
| Quantized linear layers | `04_cuda_backend`, `05_transformer_runtime` | `ggml/src/ggml-cuda/mmq.cu`, `mmv.cu`, `mul-mat*.cu` |
| RMSNorm | `04_cuda_backend` | `ggml/src/ggml-cuda/norm.cu` |
| RoPE | `03_tokenization_template`, `05_transformer_runtime` | `ggml/src/ggml-cuda/rope.cu` |
| Attention / flash attention | `05_transformer_runtime` | `ggml/src/ggml-cuda/fattn*.cu` |
| KV cache operations | `05_transformer_runtime` | llama.cpp KV cache code plus ggml CUDA copy/contiguous kernels |
| Token logits sampling | `06_sampling_decode` | llama.cpp sampler chain; mostly CPU-side after logits are produced |

The project intentionally keeps production tensor math in llama.cpp. The staged
code explains the data flow around those kernels and records which kernel family
is expected to run for each high-level operation.
