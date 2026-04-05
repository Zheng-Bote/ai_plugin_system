# Prompt Optimization Plugin

Refines and improves user-provided prompts to achieve better and more consistent results from LLMs.

## Parameters

| Name | Type | Description |
|------|------|-------------|
| `text` | string | The initial prompt to optimize. |

## Goals
- Enhance clarity and precision of instructions.
- Structure prompts for better "Chain of Thought" reasoning.
- Suggest optimal LLM parameters (e.g., temperature).

## Usage Example

```bash
./build/bin/ai_cli .env prompt-opt-plugin "Explain quantum physics."
```

## Output Schema

```json
{
  "optimized_prompt": "string",
  "suggested_parameters": {
    "temperature": 0.0,
    "max_tokens": 0
  },
  "reasoning": "string",
  "confidence": 0.0
}
```
