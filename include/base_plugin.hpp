/**
 * SPDX-FileComment: Base Plugin Class
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file base_plugin.hpp
 * @brief Abstract base class for all plugins to reduce boilerplate
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
#include "schema_validator_util.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <print>

namespace ai_plugin {

/**
 * @brief Base class for plugins providing common functionalities like schema loading and validation.
 */
class BasePlugin : public Plugin {
public:
    virtual ~BasePlugin() = default;

    /**
     * @brief Common initialization logic. Loads schema if file exists.
     */
    [[nodiscard]] std::expected<void, std::string> init(std::string_view config_json) override {
        try {
            m_config = nlohmann::json::parse(config_json);
            
            std::string schema_path = get_schema_path();
            if (!schema_path.empty() && std::filesystem::exists(schema_path)) {
                std::ifstream f(schema_path);
                m_schema = nlohmann::json::parse(f);
            }
            
            return init_impl();
        } catch (const std::exception& e) {
            return std::unexpected(std::format("Plugin init failed: {}", e.what()));
        }
    }

    /**
     * @brief Validates the LLM response against the plugin's schema.
     */
    [[nodiscard]] std::expected<void, std::string> validate_output(const nlohmann::json& data) const {
        if (m_schema.empty()) return {};
        return SchemaValidator::validate(data, m_schema);
    }

protected:
    nlohmann::json m_config;
    nlohmann::json m_schema;

    /**
     * @brief Optional implementation-specific initialization.
     */
    [[nodiscard]] virtual std::expected<void, std::string> init_impl() { return {}; }

    /**
     * @brief Derived classes must provide the path to their schema file.
     */
    [[nodiscard]] virtual std::string get_schema_path() const = 0;

    /**
     * @brief Helper to parse input JSON or return it as text if invalid.
     */
    [[nodiscard]] std::pair<std::string, std::string> parse_input(std::string_view input_json, std::string_view default_mode) const {
        try {
            auto j = nlohmann::json::parse(input_json);
            return {j.value("text", std::string(input_json)), j.value("mode", std::string(default_mode))};
        } catch (...) {
            return {std::string(input_json), std::string(default_mode)};
        }
    }

    /**
     * @brief Generates a string instruction for the LLM to follow the JSON schema.
     */
    [[nodiscard]] std::string get_schema_instruction() const {
        if (m_schema.empty()) return "";
        return "\nYour response MUST be a valid JSON object matching this schema: " + m_schema.dump() + 
               "\nEnsure all required fields are present and use the exact keys defined.";
    }
};

} // namespace ai_plugin
