# Responses API scope

The first implementation targets a text-only subset of the OpenAI Responses API:

- `POST /v1/responses`
- JSON request body
- plain text `input`
- optional `instructions`
- `temperature`, `top_p`, `max_output_tokens`, and `stream`

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
