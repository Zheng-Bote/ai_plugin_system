/**
 * SPDX-FileComment: Plugin Manager Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file plugin_manager.cpp
 * @brief Dynamic Plugin Loader using dlopen/dlsym
 * @version 0.4.2
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "plugin_manager.hpp"
#include <dlfcn.h>
#include <print>
#include <algorithm>

namespace ai_plugin {

PluginManager::~PluginManager() {
    for (auto& ph : m_handles) {
        if (ph.instance && ph.destroy_plugin) {
            ph.instance->shutdown();
            ph.destroy_plugin(ph.instance);
        }
        if (ph.handle) {
            dlclose(ph.handle);
        }
    }
}

std::expected<Plugin*, std::string> PluginManager::load_plugin(std::string_view plugin_path) {
    void* handle = dlopen(plugin_path.data(), RTLD_NOW);
    if (!handle) {
        return std::unexpected(std::format("dlopen failed for {}: {}", plugin_path, dlerror()));
    }

    auto create_func = reinterpret_cast<Plugin* (*)()>(dlsym(handle, "create_plugin"));
    auto destroy_func = reinterpret_cast<void (*)(Plugin*)>(dlsym(handle, "destroy_plugin"));

    if (!create_func || !destroy_func) {
        dlclose(handle);
        return std::unexpected(std::format("dlsym failed for {}: {}", plugin_path, dlerror()));
    }

    Plugin* instance = create_func();
    if (!instance) {
        dlclose(handle);
        return std::unexpected(std::format("Plugin creation failed for {}", plugin_path));
    }

    m_handles.push_back({handle, instance, destroy_func});
    m_plugins.push_back(instance);

    return instance;
}

void PluginManager::unload_plugin(Plugin* plugin) {
    auto it = std::find_if(m_handles.begin(), m_handles.end(), [plugin](const auto& ph) {
        return ph.instance == plugin;
    });

    if (it != m_handles.end()) {
        it->instance->shutdown();
        it->destroy_plugin(it->instance);
        dlclose(it->handle);
        
        auto p_it = std::find(m_plugins.begin(), m_plugins.end(), plugin);
        if (p_it != m_plugins.end()) {
            m_plugins.erase(p_it);
        }
        m_handles.erase(it);
    }
}

} // namespace ai_plugin
