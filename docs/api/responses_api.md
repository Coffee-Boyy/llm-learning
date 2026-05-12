# Responses API scope

The first implementation targets a text-only subset of the OpenAI Responses API:

- `POST /v1/responses`
- JSON request body
- plain text `input`
- optional `instructions`
- `temperature`, `top_p`, `max_output_tokens`, and `stream`

## Streaming (`stream: true`)

When `stream` is `true` and a llama.cpp CLI is configured, the server validates
model paths, writes an HTTP `text/event-stream` response **without**
`Content-Length`, and forwards UTF-8 safe chunks as they arrive from the
subprocess stdout pipe. Each chunk is emitted as an SSE `response.output_text.delta`
event. Completion uses `response.completed` followed by the `[DONE]` sentinel.

If validation fails before generation starts, the server falls back to a normal
JSON error response (HTTP 400) instead of SSE.

The server returns a Responses-style object:

```json
{
  "id": "resp_...",
  "object": "response",
  "created_at": 1710000000,
  "model": "qwen3-8b-q4_k_m",
  "status": "completed",
  "output": [
    {
      "type": "message",
      "role": "assistant",
      "content": [
        {"type": "output_text", "text": "..."}
      ]
    }
  ],
  "usage": {
    "input_tokens": 0,
    "output_tokens": 0,
    "total_tokens": 0
  }
}
```

Unsupported hosted tools and multimodal features return:

```json
{
  "error": {
    "type": "unsupported_feature",
    "message": "..."
  }
}
```
