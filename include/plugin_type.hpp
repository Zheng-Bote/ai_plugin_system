/**
 * SPDX-FileComment: Plugin Interface Definition (Async Task Support)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file plugin_type.hpp
 * @brief Definition of the Plugin Interface and Lifecycle Hooks
 * @version 0.4.2
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
 */
class Plugin {
public:
    virtual ~Plugin() = default;

    /**
     * @brief Initialize the plugin with specific configuration.
     */
    [[nodiscard]] virtual std::expected<void, std::string> init(std::string_view config_json) = 0;

    /**
     * @brief Perform the core analysis of the plugin. Now returns a Task for async awaiting.
     */
    [[nodiscard]] virtual Task<std::expected<std::string, std::string>> analyze(std::string input_json, LLMClient* llm_client) = 0;

    /**
     * @brief Perform the core analysis of the plugin as a stream.
     */
    [[nodiscard]] virtual Generator<std::string> analyze_stream(std::string input_json, LLMClient* llm_client) = 0;

    virtual void shutdown() = 0;

    [[nodiscard]] virtual std::string_view get_name() const = 0;
    [[nodiscard]] virtual std::string_view get_version() const = 0;
    [[nodiscard]] virtual std::string_view get_description() const { return "No description provided."; }
};

extern "C" {
    Plugin* create_plugin();
    void destroy_plugin(Plugin* plugin);
}

} // namespace ai_plugin
