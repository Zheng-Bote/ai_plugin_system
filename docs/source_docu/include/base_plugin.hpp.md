# base_plugin.hpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Base Plugin Class |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | Apache-2.0 |
| **File** | `base_plugin.hpp` |
| **Description** | Abstract base class for all plugins to reduce boilerplate |
| **Version** | 0.4.2 |
| **Date** | 2026-04-05 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | Apache-2.0 |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `class BasePlugin : public Plugin `

> Base class for plugins providing common functionalities like schema loading and validation.

---

### `[[nodiscard]] std::expected<void, std::string> init(std::string_view config_json) override `

> Common initialization logic. Loads schema if file exists.

---

### `[[nodiscard]] std::expected<void, std::string> validate_output(const nlohmann::json& data) const `

> Validates the LLM response against the plugin's schema.

---

### `[[nodiscard]] virtual std::expected<void, std::string> init_impl() `

> Optional implementation-specific initialization.

---

### `[[nodiscard]] virtual std::string get_schema_path() const = 0`

> Derived classes must provide the path to their schema file.

---

### `[[nodiscard]] std::pair<std::string, std::string> parse_input(std::string_view input_json, std::string_view default_mode) const `

> Helper to parse input JSON or return it as text if invalid.

---

### `[[nodiscard]] std::string get_schema_instruction() const `

> Generates a string instruction for the LLM to follow the JSON schema.

---

