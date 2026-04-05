# Text Optimization Plugin

Improve text quality—clearer, more precise, shorter, or more professional.

## Parameters

| Name | Type | Description |
|------|------|-------------|
| `text` | string | Input text to optimize. |
| `mode` | enum | `clarity`, `readability`, `formal`, `technical`, `friendly`, `concise`. |

## Usage Example

```bash
./build/bin/ai_cli .env ./build/lib/libtext_optimization_plugin.so \
  '{"text": "The implementation is not good.", "mode": "technical"}'
```

## Output Schema

```json
{
  "optimized_text": "string",
  "changes": [
    {
      "type": "clarity | grammar | structure | tone",
      "before": "string",
      "after": "string"
    }
  ],
  "confidence": 0.0
}
```
