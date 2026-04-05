# AI Plugin System

Ein modulares, erweiterbares CLI/Library-Stack mit Plug-In System und intelligentem Modell-Routing, implementiert in C++23.

:arrow_right: <mark>:warning: still under construction :warning:</mark> :arrow_left:

## Dokumentation

Die vollständige Dokumentation der Architektur und Designentscheidungen befindet sich im Ordner `docs/`.

- [Architektur & Datenfluss](docs/architecture/architecture.md)
- [System-Optimierungen (v3+)](docs/architecture/optimizations.md)

## Features

- **C++23 & CMake**: Nutzt moderne C++-Features wie `std::expected` und `<print>`.
- **Plugin-System**: C-Interface zum dynamischen Laden von `.so` Shared Libraries mit klaren Lebenszyklus-Hooks (`init`, `analyze`, `shutdown`).
- **Intelligentes Modell-Routing**: Fallback-Logik über mehrere konfigurierte LLM-Endpoints (`LLM_01`, `LLM_02`, etc.).
- **Exponential Backoff**: Mit Jitter für robuste Retries bei Netzwerk- und JSON-Schema-Fehlern.
- **Structured Output**: Plugins liefern striktes JSON zurück, welches gegen `data/schemas/` validiert werden kann.

## Build

Vorraussetzungen:

- GCC 13+, Clang 16+ oder MSVC (C++23 Support)
- CMake 3.28+

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Ausführen

Kopiere `.env.example` zu `.env` und setze deinen API Key.

```bash
# Plugin bauen (Beispiel Text Analyse)
# Das Plugin befindet sich nach dem Build in build/lib/libtext_analysis_plugin.so

# Ausführen mit Text-Input
./build/bin/ai_cli .env ./build/lib/libtext_analysis_plugin.so "Das ist ein Test-Text für die Analyse."

# Ausführen mit JSON-Input
./build/bin/ai_cli .env ./build/lib/libtext_analysis_plugin.so '{"text": "C++23 ist fantastisch!", "custom_param": 123}'

# Ausführen der Text-Zusammenfassung (Executive Summary)
./build/bin/ai_cli .env ./build/lib/libsummarization_plugin.so '{"text": "Hier steht ein sehr langer technischer Text...", "mode": "executive"}'

# Ausführen der SBOM-Heuristik
./build/bin/ai_cli .env ./build/lib/libsbom_heuristic_plugin.so '{"component": "libcurl-8.0", "type": "library"}'

# Ausführen der Text-Optimierung (Klarheit)
./build/bin/ai_cli .env ./build/lib/libtext_optimization_plugin.so '{"text": "Der Text ist nicht so gut zu lesen.", "mode": "clarity"}'

# Ausführen von Fragestellungen (QA)
./build/bin/ai_cli .env ./build/lib/libqa_plugin.so '{"text": "C++23 wurde im Jahr 2023 veröffentlicht.", "question": "Wann wurde C++23 veröffentlicht?", "mode": "direct"}'

# Weitere Plugins:
# ./build/bin/ai_cli .env ./build/lib/libnormalization_plugin.so "Text mit <html> und Sonderzeichen..."
# ./build/bin/ai_cli .env ./build/lib/libkeywords_plugin.so "Langer Text über KI, C++ und Linux."
# ./build/bin/ai_cli .env ./build/lib/libprompt_opt_plugin.so "Fasse den Text kurz zusammen."
# ./build/bin/ai_cli .env ./build/lib/libembeddings_plugin.so "Semantischer Beispieltext."
```

## Roadmap

- **MVP**: Grundlegendes Plugin-System, Hardcoded Routing, 2 Mock Plugins, C++23. _(Erledigt)_
- **v1**: Echter HTTP Client (libcurl), nlohmann::json Validierung, `.env`-Parser (via EnvUtil), dlopen/dlsym Plugin-Loader. _(Erledigt)_
- **v2**: Asynchrones Interface (Coroutines), Streaming-Support. _(Erledigt)_
- **v3**: JSON-Schema Validierung (valijson), `curl_multi` Async-Performance, `BasePlugin`-Refactoring. _(Erledigt)_
- **v4**: WASM-Export, plugin sandboxing, Secret Manager.
