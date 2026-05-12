# Stage 04 CUDA notes

This stage is the boundary between educational code and optimized tensor math.

The project does not reimplement CUDA kernels for production inference. Instead,
it constructs llama.cpp invocations that use:

- `-ngl 99` for full layer offload where GPU memory allows
- `-fa` for llama.cpp flash-attention-capable execution
- `-sm row` for row split mode, matching Qwen's public llama.cpp example
- recent llama.cpp builds so Qwen3 GGUF metadata and chat templates remain
  compatible

The kernel registry in `04_kernel_registry.*` maps transformer concepts to the
ggml CUDA source files worth reading while learning how inference runs on GPUs.
