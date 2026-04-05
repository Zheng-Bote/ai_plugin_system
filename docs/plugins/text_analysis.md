# Text Analysis Plugin

Provides core text analysis capabilities including sentiment detection, entity recognition, and intent classification.

## Parameters

| Name | Type | Description |
|------|------|-------------|
| `text` | string | The input text to analyze. |

## Goals
- Detect the emotional tone of the text (positive, neutral, negative).
- Categorize the text into predefined or inferred domains.
- Identify key entities mentioned in the input.

## Usage Example

```bash
./build/bin/ai_cli .env text-analysis-plugin "I really enjoy working with C++23 coroutines!"
```

## Output Schema

```json
{
  "summary": "string",
  "sentiment": {
    "label": "positive | neutral | negative",
    "score": 0.0
  },
  "categories": ["string"],
  "confidence": 0.0
}
```
