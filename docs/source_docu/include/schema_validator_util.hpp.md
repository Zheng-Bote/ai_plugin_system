# schema_validator_util.hpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | JSON Schema Validation Utility using valijson |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | Apache-2.0 |
| **File** | `schema_validator_util.hpp` |
| **Description** | Helper to validate nlohmann::json against a schema file |
| **Version** | 0.4.2 |
| **Date** | 2026-04-05 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | Apache-2.0 |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `class SchemaValidator `

> Validates a JSON object against a JSON schema using valijson.

---

### `static std::expected<void, std::string> validate(const nlohmann::json& data, const nlohmann::json& schema_json) `

> Validate JSON data against a JSON schema.

| Parameter | Description |
| --- | --- |
| `data` | The JSON data to validate. |
| `schema_json` | The JSON schema to validate against. |

**Returns:** std::expected<void, std::string> Success or validation error description.

---

