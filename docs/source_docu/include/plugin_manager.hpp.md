# plugin_manager.hpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Plugin Manager Definition |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | Apache-2.0 |
| **File** | `plugin_manager.hpp` |
| **Description** | Dynamic Plugin Loader and Manager using dlopen/dlsym |
| **Version** | 0.4.2 |
| **Date** | 2026-04-05 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | Apache-2.0 |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `class PluginManager `

> Manages loading and unloading of dynamic plugins.

---

### `[[nodiscard]] std::expected<Plugin*, std::string> load_plugin(std::string_view plugin_path)`

> Load a plugin from a shared library (.so) file.

| Parameter | Description |
| --- | --- |
| `plugin_path` | Path to the shared library. |

**Returns:** std::expected<Plugin*, std::string> Pointer to the loaded plugin or error message.

---

### `void unload_plugin(Plugin* plugin)`

> Unload a specific plugin and close its shared library.

| Parameter | Description |
| --- | --- |
| `plugin` | Pointer to the plugin to unload. |

---

### `[[nodiscard]] const std::vector<Plugin*>& get_plugins() const `

> Get all loaded plugins.

**Returns:** const std::vector<Plugin*>& Vector of pointers to loaded plugins.

---

