#include <iostream>

#include "05_transformer_runtime/05_session.h"
#include "06_sampling_decode/06_sampler.h"
#include "08_observability_bench/08_cuda_memory.h"
#include "08_observability_bench/08_trace.h"

int main(int argc, char** argv) {
  std::string model = argc > 1 ? argv[1] : "";
  std::string llama_cli = argc > 2 ? argv[2] : "";
  dissected::runtime::ModelContext context({model, "qwen3-8b-q4_k_m", llama_cli, 99, 32768});
  dissected::runtime::GenerationSession session(context);
  auto sampling = dissected::sampling::DefaultsForThinkingMode(dissected::prompt::ThinkingMode::NoThinking);
  sampling.max_output_tokens = 64;

  dissected::observability::Trace trace;
  auto result = session.Generate({"Explain GPU prefill and decode in two sentences /no_think", "", sampling});
  trace.Mark("generate");
  if (!result.ok()) {
    std::cerr << result.status().message() << "\n";
    return 1;
  }
  std::cout << "Output:\n" << result.value().text << "\n\n";
  std::cout << "Usage estimate: input=" << result.value().input_tokens
            << " output=" << result.value().output_tokens << "\n";
  std::cout << "Trace: " << trace.Summary() << "\n";
  std::cout << dissected::observability::CudaMemoryNote() << "\n";
  return 0;
}
