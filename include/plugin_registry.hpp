/**
 * SPDX-FileComment: Plugin Registry Definition
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file plugin_registry.hpp
 * @brief Automatic discovery and registration of plugins
 * @version 0.1.0
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "plugin_manager.hpp"
#include <string>
#include <vector>
#include <filesystem>
#include <map>

namespace ai_plugin {

/**
 * @brief Discovers and manages a collection of plugins from a directory.
 */
class PluginRegistry {
public:
    explicit PluginRegistry(PluginManager& manager) : m_manager(manager) {}

    /**
     * @brief Scans a directory for .so files and attempts to load them as plugins.
     * @param directory The path to scan.
     * @return Number of plugins successfully loaded.
     */
    size_t scan_directory(const std::filesystem::path& directory) {
        if (!std::filesystem::exists(directory)) return 0;

        size_t loaded = 0;
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.path().extension() == ".so") {
                auto res = m_manager.load_plugin(entry.path().string());
                if (res) {
                    m_registry[std::string(res.value()->get_name())] = res.value();
                    loaded++;
                }
            }
        }
        return loaded;
    }

    /**
     * @brief Finds a plugin by its name.
     */
    Plugin* get_plugin(const std::string& name) {
        if (m_registry.contains(name)) return m_registry[name];
        return nullptr;
    }

    const std::map<std::string, Plugin*>& get_all_plugins() const { return m_registry; }

private:
    PluginManager& m_manager;
    std::map<std::string, Plugin*> m_registry;
};

} // namespace ai_plugin
