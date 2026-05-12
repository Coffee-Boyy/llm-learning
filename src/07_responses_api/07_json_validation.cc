#include "07_responses_api/07_json_validation.h"

#include "00_common/json.h"

namespace dissected::api {

bool HasUnsupportedHostedFeature(const std::string& body, std::string* feature) {
  for (const auto& name : {"tools", "background", "conversation", "modalities", "image", "file_search"}) {
    if (body.find(std::string("\"") + name + "\"") != std::string::npos) {
      if (feature) *feature = name;
      return true;
    }
  }
  return false;
}

Result<ResponsesRequest> ParseResponsesRequest(const std::string& body) {
  auto parsed = ParseJson(body);
  if (!parsed.ok()) return parsed.status();
  if (!parsed.value().is_object()) {
    return Status::Error("Responses request must be a JSON object");
  }

  ResponsesRequest request;
  if (auto model = parsed.value().Get("model"); model && model->is_string()) {
    request.model = model->as_string();
  }
  if (auto input = parsed.value().Get("input")) {
    if (input->is_string()) {
      request.input = input->as_string();
    } else if (input->is_array()) {
      for (const auto& item : input->as_array()) {
        if (item.is_string()) {
          request.input += item.as_string();
        } else if (auto content = item.Get("content"); content && content->is_string()) {
          request.input += content->as_string();
        }
        request.input += "\n";
      }
    }
  }
  if (auto instructions = parsed.value().Get("instructions"); instructions && instructions->is_string()) {
    request.instructions = instructions->as_string();
  }
  if (auto temperature = parsed.value().Get("temperature"); temperature && temperature->is_number()) {
    request.temperature = temperature->as_number();
  }
  if (auto top_p = parsed.value().Get("top_p"); top_p && top_p->is_number()) {
    request.top_p = top_p->as_number();
  }
  if (auto max_tokens = parsed.value().Get("max_output_tokens"); max_tokens && max_tokens->is_number()) {
    request.max_output_tokens = static_cast<int>(max_tokens->as_number());
  }
  if (auto stream = parsed.value().Get("stream"); stream && stream->is_bool()) {
    request.stream = stream->as_bool();
  }
  if (request.input.empty()) {
    return Status::Error("Responses request requires text input");
  }
  return request;
}

}  // namespace dissected::api
