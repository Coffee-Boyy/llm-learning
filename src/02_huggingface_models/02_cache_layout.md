# Stage 02 cache layout

Model files are not committed. Keep them below `models/`:

```text
models/
  qwen3-8b-gguf/
    Qwen3-8B-Q4_K_M.gguf
```

Download default:

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
