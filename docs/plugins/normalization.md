# Normalization Plugin

Standardizes and cleans raw text data, making it suitable for further analysis or LLM processing.

## Parameters

| Name | Type | Description |
|------|------|-------------|
| `text` | string | The raw input text to normalize. |

## Goals
- Remove HTML tags and Markdown formatting.
- Standardize date and time formats.
- Strip unnecessary special characters or boilerplate noise.

## Usage Example

```bash
./build/bin/ai_cli .env normalization-plugin "Raw text with <b>HTML</b> and dates like 05/04/26."
```

## Output Schema

```json
{
  "normalized_text": "string",
  "operations_performed": ["string"],
  "confidence": 0.0
}
```
