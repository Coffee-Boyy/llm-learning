cmake_minimum_required(VERSION 3.20)

project(dissected_llm_provider LANGUAGES CXX)

option(DISSECTED_LLM_CUDA "Document and enable CUDA-oriented backend options" ON)
option(DISSECTED_LLM_BUILD_TESTS "Build tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(dissected_llm_core
  src/00_common/json.cc
  src/00_common/process.cc
  src/00_common/strings.cc
  src/01_gguf_loading/01_gguf_format.cc
  src/01_gguf_loading/01_quant_types.cc
  src/01_gguf_loading/01_tensor_index.cc
  src/02_huggingface_models/02_hf_download.cc
  src/02_huggingface_models/02_model_registry.cc
  src/03_tokenization_template/03_qwen3_template.cc
  src/03_tokenization_template/03_tokenizer_bridge.cc
  src/04_cuda_backend/04_kernel_registry.cc
  src/04_cuda_backend/04_llama_backend.cc
  src/05_transformer_runtime/05_kv_cache.cc
  src/05_transformer_runtime/05_model_context.cc
  src/05_transformer_runtime/05_prefill_decode.cc
  src/05_transformer_runtime/05_session.cc
  src/06_sampling_decode/06_sampler.cc
  src/06_sampling_decode/06_stop_conditions.cc
  src/06_sampling_decode/06_streaming_text.cc
  src/07_responses_api/07_error_shapes.cc
  src/07_responses_api/07_json_validation.cc
  src/07_responses_api/07_responses_types.cc
  src/07_responses_api/07_sse_stream.cc
  src/08_observability_bench/08_cuda_memory.cc
  src/08_observability_bench/08_trace.cc
)

target_include_directories(dissected_llm_core PUBLIC src)

if(DISSECTED_LLM_CUDA)
  target_compile_definitions(dissected_llm_core PUBLIC DISSECTED_LLM_CUDA=1)
endif()

add_executable(dissected-gguf-inspect
  src/01_gguf_loading/01_gguf_inspect_main.cc
)
target_link_libraries(dissected-gguf-inspect PRIVATE dissected_llm_core)

add_executable(dissected-hf-download
  src/02_huggingface_models/02_hf_download_main.cc
)
target_link_libraries(dissected-hf-download PRIVATE dissected_llm_core)

add_executable(dissected-llm-server
  src/07_responses_api/07_server_main.cc
)
target_link_libraries(dissected-llm-server PRIVATE dissected_llm_core)

add_executable(dissected-llm-bench
  src/08_observability_bench/08_benchmark_main.cc
)
target_link_libraries(dissected-llm-bench PRIVATE dissected_llm_core)

if(DISSECTED_LLM_BUILD_TESTS)
  enable_testing()

  add_executable(test-gguf-parser
    tests/01_gguf_loading/test_gguf_parser.cc
  )
  target_link_libraries(test-gguf-parser PRIVATE dissected_llm_core)
  add_test(NAME gguf_parser COMMAND test-gguf-parser)

  add_executable(test-responses-api
    tests/07_responses_api/test_responses_api.cc
  )
  target_link_libraries(test-responses-api PRIVATE dissected_llm_core)
  add_test(NAME responses_api COMMAND test-responses-api)
endif()
