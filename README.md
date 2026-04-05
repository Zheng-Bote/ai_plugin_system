# AI Plugin System

A modular, extensible CLI/Library stack with a plugin system and intelligent model routing, implemented in modern C++23.

## Documentation

The full documentation of the architecture and design decisions is located in the `docs/` folder.

- [Architecture & Data Flow](docs/architecture/architecture.md)
- [System Optimizations (v3+)](docs/architecture/optimizations.md)
- [Plugin Documentation](docs/plugins/)

## Key Features

- **C++23 & CMake**: Utilizes modern C++ features like `std::expected`, `<print>`, and coroutines.
- **Plugin System**: C-Interface for dynamic loading of `.so` shared libraries with lifecycle hooks (`init`, `analyze`, `shutdown`).
- **Asynchronous I/O**: High-performance HTTP client based on `libcurl` multi-interface.
- **Intelligent Routing**: Fallback logic across multiple LLM endpoints.
- **Schema Validation**: Strict validation of LLM responses against JSON schemas using `valijson`.
- **Result Caching**: Thread-safe LRU cache to reduce latency and API costs.
- **Audit Logging**: Structured JSON logging for monitoring and auditing.

## Requirements

- **Compiler**: GCC 13+, Clang 16+, or MSVC (full C++23 support required).
- **Build System**: CMake 3.28+.
- **Libraries**:
  - `libcurl`: For HTTP communication.
  - `nlohmann_json`: For JSON processing.
  - `valijson`: For JSON schema validation.

## Build Instructions

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Plugin Overview

| Plugin Name | Description | Key Parameters |
|-------------|-------------|----------------|
| `text-analysis-plugin` | Sentiment analysis and classification. | `text` |
| `summarization-plugin` | Auto-summarize long texts in various styles. | `text`, `mode` (short, bullet, executive, technical) |
| `text-optimization-plugin` | Improve text clarity, tone, and grammar. | `text`, `mode` (clarity, formal, technical, etc.) |
| `qa-plugin` | Question-Answering with source references. | `text`, `question`, `mode` |
| `normalization-plugin` | Preprocessing (remove HTML, unify dates). | `text` |
| `keywords-plugin` | Extract topics, entities, and tags. | `text` |
| `prompt-opt-plugin` | Optimize LLM prompts for better results. | `text` |
| `embeddings-plugin` | Generate high-dimensional vector representations. | `text` |
| `sbom-heuristic-plugin` | Analyze Software Bill of Materials for origin patterns. | `component`, `context` |

## Usage

Copy `.env.example` to `.env` and set your API Key.

```bash
# Run a specific plugin via CLI
./build/bin/ai_cli .env ./build/lib/libsummarization_plugin.so '{"text": "Very long text...", "mode": "executive"}'
```

## Roadmap

- **v1 - v3**: Core Architecture, Async I/O, Schema Validation, BasePlugin Refactoring. *(Completed)*
- **v4**: WASM-Sandboxing, Plugin discovery, Secret Manager integration.
