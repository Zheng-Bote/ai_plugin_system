# Keywords Extraction Plugin

Extracts significant terms, topics, and entities from a given text.

## Parameters

| Name | Type | Description |
|------|------|-------------|
| `text` | string | The input text to extract keywords from. |

## Goals
- Identify high-level topics and themes.
- Extract named entities (people, places, organizations).
- Generate a list of relevant tags for indexing or search.

## Usage Example

```bash
./build/bin/ai_cli .env keywords-plugin "Large language models are transforming software engineering in 2026."
```

## Output Schema

```json
{
  "topics": ["string"],
  "entities": ["string"],
  "tags": ["string"],
  "confidence": 0.0
}
```
