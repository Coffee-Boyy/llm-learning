# External optimized backend

This project references llama.cpp/ggml as the optimized inference backend rather
than rewriting CUDA kernels.

## Quick clone + CUDA build (Linux)

NVIDIA CUDA toolkit and a supported GPU driver must be present. Apple Silicon
macOS does not use this path; use upstream Metal builds instead of `GGML_CUDA`.

From the repository root:

```bash
chmod +x scripts/build-llama-cuda.sh
./scripts/build-llama-cuda.sh
```

Environment overrides:

- `LLAMA_CPP_DIR` — source checkout location (default `external/llama.cpp`).
- `LLAMA_BUILD_DIR` — CMake build directory (default `${LLAMA_CPP_DIR}/build`).
- `CMAKE_BUILD_PARALLEL_LEVEL` — passed through to `cmake --build -j`.

Manual equivalent:

```bash
git clone --depth 1 https://github.com/ggml-org/llama.cpp external/llama.cpp
cmake -S external/llama.cpp -B external/llama.cpp/build -DCMAKE_BUILD_TYPE=Release -DGGML_CUDA=ON
cmake --build external/llama.cpp/build -j
```

If your toolchain expects the legacy flag name, try `-DGGML_CUDA=ON` first, then
consult the upstream CMake options list (`cmake -L external/llama.cpp/build`)
for renames such as `LLAMA_CUDA` on older tags.

### Optional git submodule

If you prefer a pinned submodule instead of a script-managed clone, add
`llama.cpp` as a submodule pointing at `external/llama.cpp`, then run the same
CMake configure line against that path.

### Verification

After building, run:

```bash
file external/llama.cpp/build/bin/llama-cli
ldd external/llama.cpp/build/bin/llama-cli | grep -i cuda || true
```

You should see a dynamically linked binary whose dependency list includes CUDA
user-space libraries when CUDA is enabled correctly.

## Wire-up with this repository

```bash
export DISSECTED_LLM_MODEL="$(pwd)/models/qwen3-8b-gguf/Qwen3-8B-Q4_K_M.gguf"
export DISSECTED_LLM_LLAMA_CLI="$(pwd)/external/llama.cpp/build/bin/llama-cli"

./build/dissected-llm-server --host 0.0.0.0 --port 8000 --gpu-layers 99
```

CMake preset for the learning repo itself (CPU build flags; CUDA remains inside
llama.cpp): `cmake --preset linux-release`.

The stage 04 kernel registry points to the ggml CUDA source files that implement
operations such as quantized matrix multiplication, RoPE, normalization, and
attention. Keeping llama.cpp external makes it easy to update to the latest Qwen3
GGUF support.
