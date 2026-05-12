#pragma once

#include <string>

#include "05_transformer_runtime/05_session.h"

namespace dissected::api {

struct ResponsesRequest {
  std::string model;
  std::string input;
  std::string instructions;
  double temperature = -1;
  double top_p = -1;
  int max_output_tokens = 256;
  bool stream = false;
};

std::string ResponseJson(const ResponsesRequest& request,
                         const runtime::GenerationResult& generation);

}  // namespace dissected::api
