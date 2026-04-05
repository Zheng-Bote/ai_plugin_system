# plugin_registry.hpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Plugin Registry Definition |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | Apache-2.0 |
| **File** | `plugin_registry.hpp` |
| **Description** | Automatic discovery and registration of plugins |
| **Version** | 0.4.2 |
| **Date** | 2026-04-05 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | Apache-2.0 |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `class PluginRegistry `

> Discovers and manages a collection of plugins from a directory.

---

### `size_t scan_directory(const std::filesystem::path& directory) `

> Scans a directory for .so files and attempts to load them as plugins.

| Parameter | Description |
| --- | --- |
| `directory` | The path to scan. |

**Returns:** Number of plugins successfully loaded.

---

### `Plugin* get_plugin(const std::string& name) `

> Finds a plugin by its name. Tolerates case and dash/underscore differences.

---

