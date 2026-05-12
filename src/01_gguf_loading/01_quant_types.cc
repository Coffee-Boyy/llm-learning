#include "01_gguf_loading/01_quant_types.h"

namespace dissected::gguf {

QuantTypeInfo DescribeQuantType(TensorType type) {
  switch (type) {
    case TensorType::Q4_K:
      return {"Q4_K", "4-bit K-quant family used by Q4_K_M model tensors", "ggml-cuda/mmq.cu and dequantize paths"};
    case TensorType::Q5_0:
      return {"Q5_0", "legacy 5-bit block quantization", "ggml-cuda/mmv.cu and dequantize paths"};
    case TensorType::Q5_K:
      return {"Q5_K", "5-bit K-quant family used by Q5_K_M tensors", "ggml-cuda/mmq.cu and dequantize paths"};
    case TensorType::Q6_K:
      return {"Q6_K", "6-bit K-quant family for higher quality quantized tensors", "ggml-cuda/mmq.cu and dequantize paths"};
    case TensorType::Q8_0:
      return {"Q8_0", "8-bit block quantization often used for high-quality GGUF", "ggml-cuda/mmv.cu, mmq.cu"};
    case TensorType::F16:
      return {"F16", "half-precision dense tensor", "ggml-cuda/mul-mat*.cu"};
    case TensorType::F32:
      return {"F32", "single-precision dense tensor", "ggml-cuda/mul-mat*.cu"};
    default:
      return {TensorTypeName(type), "other llama.cpp-supported GGUF tensor type", "see ggml/src/ggml-cuda"};
  }
}

}  // namespace dissected::gguf
