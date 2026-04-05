# AI Plugin System - Architektur

## Komponenten und Datenfluss

Das System besteht aus den folgenden Hauptkomponenten:

1. **CLI / Core Application (`src/main.cpp`)**: Initialisiert das System, liest Umgebungsvariablen (`.env`), instanziiert den HTTP-Client und lädt Plugins dynamisch.
2. **Plugin Interface (`include/plugin_type.hpp`)**: Definiert die Lebenszyklus-Hooks (`init`, `analyze`, `shutdown`) für alle Plugins.
3. **LLM Client Interface (`include/llm_client_type.hpp`)**: Definiert die Abstraktionsebene zur Kommunikation mit Large Language Models.
4. **HTTP LLM Client (`src/http_llm_client.cpp`)**: Konkrete Implementierung des LLM Clients mit Retry-Logik, Exponential Backoff, und Routing.
5. **Plugins (`plugins/`)**:
   - `text_analysis`: Analysiert Text (Sentiment, Kategorien).
   - `sbom_heuristic`: Analysiert SBOMs und Artefakte.

### Architekturdiagramm

```mermaid
graph TD
    A[CLI User / CI Pipeline] -->|Input + .env| B(Core CLI Application)
    B -->|Load Configuration| C[Config Manager]
    B -->|Initialize| D[Plugin Manager]
    
    D -->|dlopen/dlsym| E1[Text Analysis Plugin]
    D -->|dlopen/dlsym| E2[SBOM Heuristic Plugin]
    
    E1 -->|Query| F(LLM Client Interface)
    E2 -->|Query| F
    
    F -->|Implements| G[HTTP LLM Client]
    
    G -->|POST + Exponential Backoff| H1(Model: LLM_01)
    G -->|Fallback POST| H2(Model: LLM_02)
    G -->|Fallback POST| H3(Model: LLM_03)
    
    H1 -->|JSON Output| G
    H2 -->|JSON Output| G
    H3 -->|JSON Output| G
    
    G -->|std::expected| E1
    E1 -->|Strict JSON| B
    B -->|stdout / Log| A
```

## JSON Validierung und Structured Output

Plugins erhalten unstrukturierten oder strukturierten Text, formen daraus einen System Prompt, und erzwingen durch das LLM striktes JSON. 
Dieses Output-Format wird mit einem Schema verifiziert (in `data/schemas/`). Bei einem Schema-Bruch wird die Retry-Logik des HTTP-Clients (Exponential Backoff with Jitter) ausgelöst.

## Sicherheit und Secrets

- `OPENROUTER_API_KEY` und andere Secrets werden nur über `.env` oder Umgebungsvariablen geladen und existieren im RAM, niemals in Logs oder Dumps.
- Langfristig sollte eine Anbindung an HashiCorp Vault oder AWS Secrets Manager über das Config-Modul in Erwägung gezogen werden.
