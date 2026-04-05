# Embedding Generator Plugin

Generates high-dimensional vector representations (embeddings) for text data, enabling semantic search and retrieval.

## Parameters

| Name | Type | Description |
|------|------|-------------|
| `text` | string | The text to vectorize. |

## Goals
- Convert unstructured text into a numerical format for vector databases.
- Enable semantic similarity comparisons between different documents.
- Support downstream tasks like RAG (Retrieval-Augmented Generation).

## Usage Example

```bash
./build/bin/ai_cli .env embeddings-plugin "This is a sample text for semantic indexing."
```

## Output Schema

```json
{
  "embedding": [0.1, -0.2, 0.5],
  "dimensions": 1536,
  "model": "string",
  "confidence": 1.0
}
```
