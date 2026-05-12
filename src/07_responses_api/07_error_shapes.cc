#include "07_responses_api/07_error_shapes.h"

#include "00_common/strings.h"

namespace dissected::api {

std::string ErrorJson(const std::string& type, const std::string& message) {
  return "{\"error\":{\"type\":\"" + JsonEscape(type) + "\",\"message\":\"" +
         JsonEscape(message) + "\"}}";
}

}  // namespace dissected::api
