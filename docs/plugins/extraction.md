# Preprocessing and Basic Extraction Plugins

These plugins handle text preparation and metadata extraction.

## Normalization Plugin
- **Goal**: Clean raw data (remove HTML, unify date formats, remove special characters).
- **Usage**: `./build/bin/ai_cli .env ./build/lib/libnormalization_plugin.so "Raw text..."`

## Keyword Extraction Plugin
- **Goal**: Extract topics, entities, and tags. Ideal for logs and documentation.
- **Usage**: `./build/bin/ai_cli .env ./build/lib/libkeywords_plugin.so "Long text about AI and C++..."`

## Prompt Optimization Plugin
- **Goal**: Improve user prompts for better LLM results in other plugins.
- **Usage**: `./build/bin/ai_cli .env ./build/lib/libprompt_opt_plugin.so "Summarize this text."`

## Embedding Generator Plugin
- **Goal**: Generate high-dimensional vectors for semantic search.
- **Usage**: `./build/bin/ai_cli .env ./build/lib/libembeddings_plugin.so "Semantic search example."`
