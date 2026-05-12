#include "06_sampling_decode/06_streaming_text.h"

namespace dissected::sampling {
namespace {

bool EndsWithPartialUtf8(const std::string& text, std::size_t* safe_size) {
  *safe_size = text.size();
  if (text.empty()) return false;
  std::size_t start = text.size() - 1;
  while (start > 0 && (static_cast<unsigned char>(text[start]) & 0xc0) == 0x80) {
    --start;
  }
  unsigned char lead = static_cast<unsigned char>(text[start]);
  std::size_t expected = 1;
  if ((lead & 0xe0) == 0xc0) expected = 2;
  else if ((lead & 0xf0) == 0xe0) expected = 3;
  else if ((lead & 0xf8) == 0xf0) expected = 4;
  std::size_t have = text.size() - start;
  if (have < expected) {
    *safe_size = start;
    return true;
  }
  return false;
}

}  // namespace

std::vector<std::string> Utf8Chunker::Push(std::string bytes) {
  pending_ += bytes;
  std::size_t safe = pending_.size();
  EndsWithPartialUtf8(pending_, &safe);
  if (safe == 0) return {};
  std::string out = pending_.substr(0, safe);
  pending_.erase(0, safe);
  return {out};
}

std::vector<std::string> Utf8Chunker::Flush() {
  if (pending_.empty()) return {};
  std::string out = pending_;
  pending_.clear();
  return {out};
}

}  // namespace dissected::sampling
