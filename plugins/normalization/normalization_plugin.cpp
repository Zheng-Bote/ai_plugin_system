/**
 * SPDX-FileComment: Text Normalization Plugin Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file normalization_plugin.cpp
 * @brief Production-ready text normalization
 * @version 0.1.0
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "plugin_type.hpp"
#include "llm_client_type.hpp"
#include <nlohmann/json.hpp>
#include <print>
#include <string>
#include <fstream>
#include <vector>

using json = nlohmann::json;

namespace ai_plugin {

class NormalizationPlugin : public Plugin {
public:
    [[nodiscard]] std::expected<void, std::string> init(std::string_view config_json) override {
        try {
            auto config = json::parse(config_json);
            m_config = config;
            
            std::ifstream schema_file("data/schemas/normalization.schema.json");
            if (schema_file.is_open()) {
                m_schema = json::parse(schema_file);
            }
        } catch (const std::exception& e) {
            return std::unexpected(std::format("NormalizationPlugin init fail: {}", e.what()));
        }
        return {};
    }

    [[nodiscard]] std::expected<std::string, std::string> analyze(std::string_view input_json, LLMClient* llm_client) override {
        if (!llm_client) return std::unexpected("LLM client not provided");

        std::string text = parse_input(input_json);
        
        LLMQuery query;
        query.system_prompt = "Normalisiere den folgenden Text. Entferne Sonderzeichen, vereinheitliche Datumsformate und entferne HTML/Markdown. Antworte in striktem JSON gemäß Schema.";
        query.prompt = text;
        query.json_schema = m_schema.dump();
        query.task_type = TaskType::ANALYSIS;

        auto result = llm_client->query(query);
        if (!result) return std::unexpected(result.error());

        return result->content;
    }

    [[nodiscard]] Generator<std::string> analyze_stream(std::string_view input_json, LLMClient* llm_client) override {
        if (!llm_client) {
            co_yield "Error: LLM client not provided";
            co_return;
        }

        std::string text = parse_input(input_json);
        LLMQuery query;
        query.system_prompt = "Normalisiere den Text (Cleaning). Antworte als Stream.";
        query.prompt = text;
        query.task_type = TaskType::ANALYSIS;

        for (const auto& token : llm_client->query_stream(query)) {
            co_yield token;
        }
    }

    void shutdown() override {}

    [[nodiscard]] std::string_view get_name() const override { return "normalization-plugin"; }
    [[nodiscard]] std::string_view get_version() const override { return "0.1.0"; }

private:
    json m_config;
    json m_schema;

    std::string parse_input(std::string_view input_json) {
        try {
            auto input = json::parse(input_json);
            return input.value("text", std::string(input_json));
        } catch (...) {
            return std::string(input_json);
        }
    }
};

extern "C" {
    Plugin* create_plugin() { return new NormalizationPlugin(); }
    void destroy_plugin(Plugin* plugin) { delete plugin; }
}

} // namespace ai_plugin
