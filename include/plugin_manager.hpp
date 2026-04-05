/**
 * SPDX-FileComment: Plugin Manager Definition
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file plugin_manager.hpp
 * @brief Dynamic Plugin Loader and Manager using dlopen/dlsym
 * @version 0.4.2
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "plugin_type.hpp"
#include <string>
#include <string_view>
#include <expected>
#include <memory>
#include <vector>

namespace ai_plugin {

/**
 * @brief Manages loading and unloading of dynamic plugins.
 */
class PluginManager {
public:
    PluginManager() = default;
    ~PluginManager();

    // Disable copy
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;

    /**
     * @brief Load a plugin from a shared library (.so) file.
     * 
     * @param plugin_path Path to the shared library.
     * @return std::expected<Plugin*, std::string> Pointer to the loaded plugin or error message.
     */
    [[nodiscard]] std::expected<Plugin*, std::string> load_plugin(std::string_view plugin_path);

    /**
     * @brief Unload a specific plugin and close its shared library.
     * 
     * @param plugin Pointer to the plugin to unload.
     */
    void unload_plugin(Plugin* plugin);

    /**
     * @brief Get all loaded plugins.
     * @return const std::vector<Plugin*>& Vector of pointers to loaded plugins.
     */
    [[nodiscard]] const std::vector<Plugin*>& get_plugins() const { return m_plugins; }

private:
    struct PluginHandle {
        void* handle;
        Plugin* instance;
        void (*destroy_plugin)(Plugin*);
    };

    std::vector<PluginHandle> m_handles;
    std::vector<Plugin*> m_plugins;
};

} // namespace ai_plugin
