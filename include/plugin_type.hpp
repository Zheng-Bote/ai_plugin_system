/**
 * SPDX-FileComment: Plugin Interface Definition
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file plugin_type.hpp
 * @brief Definition of the Plugin Interface and Lifecycle Hooks
 * @version 0.1.0
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "coro_type.hpp"
#include <string>
#include <string_view>
#include <expected>
#include <memory>

namespace ai_plugin {

class LLMClient;

/**
 * @brief The generic Plugin Interface.
 * Every dynamically loaded plugin must implement this interface and export a creation function.
 */
class Plugin {
public:
    virtual ~Plugin() = default;

    /**
     * @brief Initialize the plugin with specific configuration.
     * 
     * @param config_json JSON formatted configuration string.
     * @return std::expected<void, std::string> Success or error message.
     */
    [[nodiscard]] virtual std::expected<void, std::string> init(std::string_view config_json) = 0;

    /**
     * @brief Perform the core analysis of the plugin.
     * 
     * @param input_json The structured input data in JSON format.
     * @param llm_client Pointer to the LLM client to use for queries.
     * @return std::expected<std::string, std::string> The strict JSON output or error message.
     */
    [[nodiscard]] virtual std::expected<std::string, std::string> analyze(std::string_view input_json, LLMClient* llm_client) = 0;

    /**
     * @brief Perform the core analysis of the plugin as a stream.
     * 
     * @param input_json The structured input data in JSON format.
     * @param llm_client Pointer to the LLM client to use for streaming queries.
     * @return Generator<std::string> A generator yielding tokens or JSON fragments.
     */
    [[nodiscard]] virtual Generator<std::string> analyze_stream(std::string_view input_json, LLMClient* llm_client) = 0;

    /**
     * @brief Shutdown the plugin and release resources.
     */
    virtual void shutdown() = 0;

    /**
     * @brief Get the name of the plugin.
     * @return std::string_view The plugin name.
     */
    [[nodiscard]] virtual std::string_view get_name() const = 0;

    /**
     * @brief Get the version of the plugin.
     * @return std::string_view The plugin version in semver format.
     */
    [[nodiscard]] virtual std::string_view get_version() const = 0;
};

// C API for dynamic loading of the plugin
extern "C" {
    /**
     * @brief Factory function to create a plugin instance.
     * @return Plugin* Pointer to the newly created plugin instance.
     */
    Plugin* create_plugin();

    /**
     * @brief Function to destroy a plugin instance.
     * @param plugin Pointer to the plugin instance to destroy.
     */
    void destroy_plugin(Plugin* plugin);
}

} // namespace ai_plugin
