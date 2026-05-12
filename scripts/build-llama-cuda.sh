#!/usr/bin/env bash
# Reproducible llama.cpp build with CUDA on Linux (NVIDIA toolkit required).
# macOS does not ship CUDA in the same way; use Metal builds upstream instead.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
LLAMA_DIR="${LLAMA_CPP_DIR:-${ROOT}/external/llama.cpp}"
BUILD_DIR="${LLAMA_BUILD_DIR:-${LLAMA_DIR}/build}"
JOBS="${CMAKE_BUILD_PARALLEL_LEVEL:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)}"

if [[ ! -f "${LLAMA_DIR}/CMakeLists.txt" ]]; then
  echo "Cloning llama.cpp into ${LLAMA_DIR} (set LLAMA_CPP_DIR to override)"
  mkdir -p "$(dirname "${LLAMA_DIR}")"
  git clone --depth 1 https://github.com/ggml-org/llama.cpp.git "${LLAMA_DIR}"
fi

echo "Configuring ${LLAMA_DIR} -> ${BUILD_DIR} with GGML_CUDA=ON"
cmake -S "${LLAMA_DIR}" -B "${BUILD_DIR}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DGGML_CUDA=ON

echo "Building llama.cpp (${JOBS} jobs)"
cmake --build "${BUILD_DIR}" -j "${JOBS}"

CLI="${BUILD_DIR}/bin/llama-cli"
if [[ -x "${CLI}" ]]; then
  echo "Built: ${CLI}"
else
  echo "warning: expected ${CLI} not found; check upstream binary names in bin/"
  ls -la "${BUILD_DIR}/bin" || true
fi
