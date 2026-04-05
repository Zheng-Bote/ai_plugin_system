# SBOM Heuristic Plugin

Analyzes Software Bill of Materials (SBOM) data and artifacts to identify likely origins, build contexts, and security-relevant metadata.

## Parameters

| Name | Type | Description |
|------|------|-------------|
| `component` | string | Name and version of the software component. |
| `context` | string | (Optional) Additional build or environment details. |

## Goals
- Infer the likely source repository or vendor of a binary/component.
- Detect build environments (compiler versions, OS).
- Provide evidence-based confidence scores for origin mapping.

## Usage Example

```bash
./build/bin/ai_cli .env sbom-heuristic-plugin '{"component": "libcurl-7.88.1", "context": "Ubuntu 22.04 binary"}'
```

## Output Schema

```json
{
  "component": "string",
  "origin": {
    "likely_source": "string",
    "confidence": 0.0,
    "evidence": ["string"]
  },
  "build_context": "string",
  "confidence": 0.0
}
```
