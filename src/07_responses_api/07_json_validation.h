#pragma once

#include <string>

#include "00_common/status.h"
#include "07_responses_api/07_responses_types.h"

namespace dissected::api {

Result<ResponsesRequest> ParseResponsesRequest(const std::string& body);
bool HasUnsupportedHostedFeature(const std::string& body, std::string* feature);

}  // namespace dissected::api
