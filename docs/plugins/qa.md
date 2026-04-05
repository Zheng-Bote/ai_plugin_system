# Question-Answering (QA) Plugin

Answer questions about arbitrary texts with source references.

## Parameters

| Name | Type | Description |
|------|------|-------------|
| `text` | string | The context text to analyze. |
| `question` | string | The specific question to answer. |
| `mode` | enum | `direct`, `reasoned`, `quotes`, `uncertainty`. |

## Modes

- **direct**: Precise and concise answer.
- **reasoned**: Includes a detailed explanation.
- **quotes**: Heavily references original text snippets.
- **uncertainty**: Analyzes potential gaps in the provided information.

## Usage Example

```bash
./build/bin/ai_cli .env ./build/lib/libqa_plugin.so \
  '{"text": "C++23 introduces std::print.", "question": "What is new in C++23?", "mode": "direct"}'
```

## Output Schema

```json
{
  "answer": "string",
  "supporting_facts": ["string"],
  "confidence": 0.0,
  "source_spans": [
    { "start": 0, "end": 10, "text": "string" }
  ]
}
```
