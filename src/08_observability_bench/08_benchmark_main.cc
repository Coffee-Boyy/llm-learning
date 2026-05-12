#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

#include "05_transformer_runtime/05_model_context.h"
#include "05_transformer_runtime/05_session.h"
#include "06_sampling_decode/06_sampler.h"
#include "08_observability_bench/08_cuda_memory.h"
#include "08_observability_bench/08_trace.h"

namespace {

void Usage() {
  std::cerr << "dissected-llm-bench [--iterations N] [--json-log] MODEL_GGUF [LLAMA_CLI]\n"
            << "Measures wall time, approximate tokens/sec, and time-to-first-chunk when streaming.\n"
            << "Environment: DISSECTED_LLM_MODEL, DISSECTED_LLM_LLAMA_CLI (defaults if args omitted).\n";
}

struct BenchArgs {
  std::string model;
  std::string llama_cli;
  int iterations = 1;
  bool json_log = false;
};

BenchArgs ParseBenchArgs(int argc, char** argv) {
  BenchArgs out;
  if (const char* env_model = std::getenv("DISSECTED_LLM_MODEL")) {
    out.model = env_model;
  }
  if (const char* env_cli = std::getenv("DISSECTED_LLM_LLAMA_CLI")) {
    out.llama_cli = env_cli;
  }
  std::string positional_model;
  std::string positional_cli;
  for (int i = 1; i < argc; ++i) {
    std::string key = argv[i];
    auto next = [&]() -> std::string { return i + 1 < argc ? argv[++i] : ""; };
    if (key == "-h" || key == "--help") {
      Usage();
      std::exit(0);
    }
    if (key == "--iterations") {
      out.iterations = std::stoi(next());
    } else if (key == "--json-log") {
      out.json_log = true;
    } else if (key.rfind("--", 0) == 0) {
      std::cerr << "unknown flag: " << key << "\n";
      Usage();
      std::exit(2);
    } else if (positional_model.empty()) {
      positional_model = key;
    } else if (positional_cli.empty()) {
      positional_cli = key;
    } else {
      std::cerr << "unexpected positional argument: " << key << "\n";
      Usage();
      std::exit(2);
    }
  }
  if (!positional_model.empty()) {
    out.model = positional_model;
  }
  if (!positional_cli.empty()) {
    out.llama_cli = positional_cli;
  }
  return out;
}

}  // namespace

int main(int argc, char** argv) {
  BenchArgs args = ParseBenchArgs(argc, argv);
  if (args.model.empty()) {
    Usage();
    return 2;
  }

  dissected::runtime::ModelContext context(
      {args.model, "qwen3-8b-q4_k_m", args.llama_cli, 99, 32768});
  dissected::runtime::GenerationSession session(context);
  auto sampling = dissected::sampling::DefaultsForThinkingMode(dissected::prompt::ThinkingMode::NoThinking);
  sampling.max_output_tokens = 64;

  long long total_ms = 0;
  long long first_chunk_ms = -1;
  int sum_output_tokens = 0;
  int last_input_tokens = 0;
  std::string last_text;

  dissected::observability::Trace trace;

  for (int i = 0; i < args.iterations; ++i) {
    auto wall_start = std::chrono::steady_clock::now();
    bool first = true;
    auto streamed = session.GenerateStreaming(
        {"Explain GPU prefill and decode in two sentences /no_think", "", sampling},
        [&](std::string_view chunk) {
          if (first && !chunk.empty()) {
            first_chunk_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::steady_clock::now() - wall_start)
                                 .count();
            first = false;
          }
        });
    auto wall_end = std::chrono::steady_clock::now();
    total_ms += std::chrono::duration_cast<std::chrono::milliseconds>(wall_end - wall_start).count();

    if (!streamed.ok()) {
      std::cerr << streamed.status().message() << "\n";
      return 1;
    }
    sum_output_tokens += streamed.value().output_tokens;
    last_input_tokens = streamed.value().input_tokens;
    last_text = streamed.value().text;
  }

  trace.Mark("streaming_bench");

  const double avg_wall_s =
      static_cast<double>(total_ms) / 1000.0 / static_cast<double>(args.iterations);
  const double avg_output_tokens =
      static_cast<double>(sum_output_tokens) / static_cast<double>(args.iterations);
  const double tok_per_sec = avg_wall_s > 0 ? avg_output_tokens / avg_wall_s : 0.0;

  if (args.json_log) {
    std::cout << "{\"event\":\"benchmark_complete\",\"iterations\":" << args.iterations
              << ",\"input_tokens\":" << last_input_tokens << ",\"avg_output_tokens\":" << avg_output_tokens
              << ",\"time_to_first_chunk_ms\":" << first_chunk_ms << ",\"wall_ms_total\":" << total_ms
              << ",\"wall_ms_avg\":" << (static_cast<double>(total_ms) / static_cast<double>(args.iterations))
              << ",\"tokens_per_sec\":" << tok_per_sec << "}\n";
  } else {
    std::cout << "Iterations: " << args.iterations << "\n";
    std::cout << "Input tokens (last): " << last_input_tokens << "\n";
    std::cout << "Output tokens (sum / avg): " << sum_output_tokens << " / " << avg_output_tokens << "\n";
    std::cout << "Time to first streamed chunk (first iter): " << first_chunk_ms << " ms\n";
    std::cout << "Wall time total: " << total_ms << " ms\n";
    std::cout << "Approx tokens/sec (avg output / avg wall): " << tok_per_sec << "\n";
    std::cout << "Trace: " << trace.Summary() << "\n";
    std::cout << "Last output:\n-----\n" << last_text << "\n-----\n";
    std::cout << dissected::observability::CudaMemoryNote() << "\n";
  }
  return 0;
}
