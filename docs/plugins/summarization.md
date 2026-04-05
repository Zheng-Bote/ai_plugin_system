# Summarization Plugin

Automatically shorten and structure long texts in various styles.

## Parameters

| Name | Type | Description |
|------|------|-------------|
| `text` | string | The input text to summarize. |
| `mode` | enum | Summary style: `short`, `bullet`, `executive`, `technical`. |

## Modes

- **short**: Brief overview (1-3 sentences).
- **bullet**: Key points extracted as a list.
- **executive**: Focus on strategic implications for management.
- **technical**: Focus on implementation details and specifications.

## Usage Example

```bash
./build/bin/ai_cli .env ./build/lib/libsummarization_plugin.so \
  '{"text": "Long technical document about C++23...", "mode": "technical"}'
```

## Output Schema

```json
{
  "summary": "string",
  "bullet_points": ["string"],
  "length": "short | medium | long",
  "confidence": 0.0
}
```
