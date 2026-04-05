/**
 * SPDX-FileComment: Text Summarization Plugin Implementation (Optimized)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file summarization_plugin.cpp
 * @brief Production-ready text summarization with LLM integration
 * @version 0.2.0
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

using json = nlohmann::json;

namespace ai_plugin {

class SummarizationPlugin : public Plugin {
public:
    [[nodiscard]] std::expected<void, std::string> init(std::string_view config_json) override {
        try {
            auto config = json::parse(config_json);
            m_default_mode = config.value("mode", "short");
            
            if (config.contains("custom_prompts")) {
                m_custom_prompts = config["custom_prompts"].get<std::unordered_map<std::string, std::string>>();
            }

            // Load schema for validation reference (optional in C++, crucial for LLM)
            std::ifstream schema_file("data/schemas/summarization.schema.json");
            if (schema_file.is_open()) {
                m_schema = json::parse(schema_file);
            }

            std::println("SummarizationPlugin v0.2.0 initialized.");
        } catch (const std::exception& e) {
            return std::unexpected(std::format("Init failed: {}", e.what()));
        }
        return {};
    }

    [[nodiscard]] std::expected<std::string, std::string> analyze(std::string_view input_json, LLMClient* llm_client) override {
        if (!llm_client) return std::unexpected("LLM client not provided");

        const auto input_pair = parse_input(input_json);
        const auto& text = input_pair.first;
        const auto& mode = input_pair.second;
        
        LLMQuery query;
        query.system_prompt = get_system_prompt(mode);
        query.prompt = text;
        query.json_schema = m_schema.dump();
        query.task_type = TaskType::SUMMARIZATION;
        query.temperature = 0.3;

        auto result = llm_client->query(query);
        if (!result) return std::unexpected(result.error());

        // Basic validation of the LLM response structure
        try {
            auto res_json = json::parse(result->content);
            if (!res_json.contains("summary") || !res_json.contains("bullet_points")) {
                return std::unexpected("LLM response missing required fields");
            }
            return res_json.dump(2);
        } catch (const std::exception& e) {
            return std::unexpected(std::format("Invalid JSON from LLM: {}", e.what()));
        }
    }

    [[nodiscard]] Generator<std::string> analyze_stream(std::string_view input_json, LLMClient* llm_client) override {
        if (!llm_client) {
            co_yield "Error: LLM client not provided";
            co_return;
        }

        const auto input_pair = parse_input(input_json);
        const auto& text = input_pair.first;
        const auto& mode = input_pair.second;
        
        LLMQuery query;
        query.system_prompt = get_system_prompt(mode);
        query.prompt = text;
        query.task_type = TaskType::SUMMARIZATION;

        for (const auto& token : llm_client->query_stream(query)) {
            co_yield token;
        }
    }

    void shutdown() override {
        std::println("SummarizationPlugin shutting down.");
    }

    [[nodiscard]] std::string_view get_name() const override { return "summarization-plugin"; }
    [[nodiscard]] std::string_view get_version() const override { return "0.2.0"; }

private:
    std::string m_default_mode = "short";
    std::unordered_map<std::string, std::string> m_custom_prompts;
    json m_schema;

    std::pair<std::string, std::string> parse_input(std::string_view input_json) {
        try {
            auto input = json::parse(input_json);
            return {input.value("text", std::string(input_json)), input.value("mode", m_default_mode)};
        } catch (...) {
            return {std::string(input_json), m_default_mode};
        }
    }

    std::string get_system_prompt(const std::string& mode) {
        if (m_custom_prompts.contains(mode)) return m_custom_prompts[mode];

        if (mode == "short") {
            return "Fasse den folgenden Text in maximal 3 Sätzen zusammen. Antworte in striktem JSON gemäß Schema.";
        } else if (mode == "bullet") {
            return "Extrahiere die wichtigsten Kernpunkte des Textes als Stichpunkte. Antworte in striktem JSON gemäß Schema.";
        } else if (mode == "executive") {
            return "Erstelle ein Executive Summary für das Management. Hebe strategische Implikationen hervor. Antworte in striktem JSON gemäß Schema.";
        } else if (mode == "technical") {
            return "Fasse die technischen Details und Implementierungsaspekte zusammen. Antworte in striktem JSON gemäß Schema.";
        }
        
        return "Fasse den Text zusammen. Antworte in striktem JSON gemäß Schema.";
    }
};

extern "C" {
    Plugin* create_plugin() { return new SummarizationPlugin(); }
    void destroy_plugin(Plugin* plugin) { delete plugin; }
}

} // namespace ai_plugin
