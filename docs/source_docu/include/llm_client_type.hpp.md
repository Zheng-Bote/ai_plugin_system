# llm_client_type.hpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | LLM Client Interface Definition (Coroutine Optimized) |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | Apache-2.0 |
| **File** | `llm_client_type.hpp` |
| **Description** | Definition of the generic LLM Client Interface and Data Types |
| **Version** | 0.4.2 |
| **Date** | 2026-04-05 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | Apache-2.0 |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `class LLMClient `

> The Generic LLMClient Interface.

---

### `[[nodiscard]] virtual Task<std::expected<LLMResponse, std::string>> query(LLMQuery query) = 0`

> Sends a structured prompt to the LLM. Now a Task for true async awaiting.

---

### `[[nodiscard]] virtual Generator<std::string> query_stream(LLMQuery query) = 0`

> Sends a streaming request to the LLM.

---

