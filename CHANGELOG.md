# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.4.2] - 2026-04-05

### Added
- **True Async Await**: Implemented a robust C++20 awaitable model using Symmetric Transfer. Coroutines now correctly suspend during network I/O and resume only when data is fully processed.
- **Fuzzy Plugin Lookup**: The `PluginRegistry` now supports case-insensitive matching and ignores dash/underscore differences (e.g., `qa-plugin` vs `qa_plugin`).
- **Enhanced Model Routing**: Support for up to 10 fallback models defined in `.env` (`LLM_01` to `LLM_10`).
- **OpenRouter Compliance**: Added mandatory headers (`HTTP-Referer`, `X-Title`) for better integration with OpenRouter API.

### Fixed
- **Memory Safety**: Resolved `nlohmann::json::parse_error` caused by dangling `std::string_view` references in async tasks. All core interfaces now use value-based parameter passing for async boundaries.
- **Empty Response Bug**: Fixed a race condition where `Task::get()` returned before the background HTTP polling thread could populate the result.
- **Compatibility**: Removed explicit `json_object` format flag to support older LLM models that do not officially support structured output but follow system instructions.

### Changed
- **Performance**: Optimized `CurlMultiManager` polling loop with `curl_multi_wakeup` for lower latency.

## [0.4.1] - 2026-04-05

### Fixed
- **Compiler Warnings**: Fixed `maybe-uninitialized` warnings in GCC when using structured bindings within coroutines by refactoring to explicit pair/struct access.
- **Header Fixes**: Added missing `<thread>` and `<chrono>` headers across various plugins.

## [0.4.0] - 2026-04-05

### Added
- **Plugin Registry**: Implemented automatic plugin discovery. The system now scans `./build/lib`, `./plugins`, and `PLUGINS_DIR` from `.env`.
- **New Command Line Interface**: Added `--list` option to view all discovered plugins and their descriptions.
- **Async Transformation**: Refactored the `Plugin` interface to return `Task<T>` for non-blocking analysis.

## [0.3.0] - 2026-04-05

### Added
- **JSON Schema Validation**: Integrated `valijson` for strict validation of LLM outputs against local schema files.
- **BasePlugin Architecture**: Introduced a base class to reduce plugin boilerplate (automated schema loading and validation).
- **Asynchronous HTTP**: Migrated `HttpLLMClient` to `libcurl` multi-interface for non-blocking network requests.
- **Caching**: Implemented a thread-safe LRU cache for analysis results.
- **Audit Logging**: Added structured JSON logging for all LLM transactions.

## [0.2.0] - 2026-04-05

### Added
- **C++20/23 Coroutines**: Initial support for asynchronous tasks and streaming generators.
- **Streaming Support**: Added `analyze_stream` to plugins and `query_stream` to LLM client for real-time token output.
- **Question-Answering Plugin**: New specialized plugin for context-based QA.
- **Text Optimization Plugin**: New specialized plugin for clarity and tone improvements.

## [0.1.0] - 2026-04-05

### Added
- **Initial MVP**: Core CLI application with dynamic plugin loading (dlopen/dlsym).
- **Plugin System**: Basic C-style interface for shared library plugins.
- **LLM Client**: Initial HTTP client with retry logic and exponential backoff.
- **Core Plugins**: Text Analysis, SBOM Heuristic, and Summarization plugins.
- **C++23 Foundation**: Project setup with CMake and strict compiler flags.
