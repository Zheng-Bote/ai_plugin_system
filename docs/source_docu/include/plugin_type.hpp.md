# plugin_type.hpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Plugin Interface Definition (Async Task Support) |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | Apache-2.0 |
| **File** | `plugin_type.hpp` |
| **Description** | Definition of the Plugin Interface and Lifecycle Hooks |
| **Version** | 0.4.2 |
| **Date** | 2026-04-05 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | Apache-2.0 |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `class Plugin `

> The generic Plugin Interface.

---

### `[[nodiscard]] virtual std::expected<void, std::string> init(std::string_view config_json) = 0`

> Initialize the plugin with specific configuration.

---

### `[[nodiscard]] virtual Task<std::expected<std::string, std::string>> analyze(std::string input_json, LLMClient* llm_client) = 0`

> Perform the core analysis of the plugin. Now returns a Task for async awaiting.

---

### `[[nodiscard]] virtual Generator<std::string> analyze_stream(std::string input_json, LLMClient* llm_client) = 0`

> Perform the core analysis of the plugin as a stream.

---

