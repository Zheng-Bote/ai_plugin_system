/**
 * SPDX-FileComment: Question-Answering (QA) Plugin Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file qa_plugin.cpp
 * @brief Production-ready QA plugin with source reference support and LLM integration
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
#include <unordered_map>
#include <vector>

using json = nlohmann::json;

namespace ai_plugin {

/**
 * @brief Implementation of the Question-Answering (QA) Plugin.
 */
class QAPlugin : public Plugin {
public:
    [[nodiscard]] std::expected<void, std::string> init(std::string_view config_json) override {
        try {
            auto config = json::parse(config_json);
            m_default_mode = config.value("mode", "direct");
            
            if (config.contains("custom_prompts")) {
                m_custom_prompts = config["custom_prompts"].get<std::unordered_map<std::string, std::string>>();
            }

            // Load schema for LLM guidance
            std::ifstream schema_file("data/schemas/qa.schema.json");
            if (schema_file.is_open()) {
                m_schema = json::parse(schema_file);
            }

            std::println("QAPlugin v0.1.0 initialized.");
        } catch (const std::exception& e) {
            return std::unexpected(std::format("Init failed: {}", e.what()));
        }
        return {};
    }

    [[nodiscard]] std::expected<std::string, std::string> analyze(std::string_view input_json, LLMClient* llm_client) override {
        if (!llm_client) return std::unexpected("LLM client not provided");

        const auto qa_input = parse_input(input_json);
        const auto& text = qa_input.text;
        const auto& question = qa_input.question;
        const auto& mode = qa_input.mode;
        
        LLMQuery query;
        query.system_prompt = get_system_prompt(mode);
        query.prompt = std::format("Context: {}\n\nQuestion: {}", text, question);
        query.json_schema = m_schema.dump();
        query.task_type = TaskType::ANALYSIS;
        query.temperature = 0.1; // Low temperature for factual QA

        auto result = llm_client->query(query);
        if (!result) return std::unexpected(result.error());

        // Validate LLM response structure
        try {
            auto res_json = json::parse(result->content);
            if (!res_json.contains("answer") || !res_json.contains("supporting_facts")) {
                return std::unexpected("LLM response missing required fields 'answer' or 'supporting_facts'");
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

        const auto qa_input = parse_input(input_json);
        const auto& text = qa_input.text;
        const auto& question = qa_input.question;
        const auto& mode = qa_input.mode;
        
        LLMQuery query;
        query.system_prompt = get_system_prompt(mode);
        query.prompt = std::format("Context: {}\n\nQuestion: {}", text, question);
        query.task_type = TaskType::ANALYSIS;

        for (const auto& token : llm_client->query_stream(query)) {
            co_yield token;
        }
    }

    void shutdown() override {
        std::println("QAPlugin shutting down.");
    }

    [[nodiscard]] std::string_view get_name() const override { return "qa-plugin"; }
    [[nodiscard]] std::string_view get_version() const override { return "0.1.0"; }

private:
    std::string m_default_mode = "direct";
    std::unordered_map<std::string, std::string> m_custom_prompts;
    json m_schema;

    struct QAInput {
        std::string text;
        std::string question;
        std::string mode;
    };

    QAInput parse_input(std::string_view input_json) {
        try {
            auto input = json::parse(input_json);
            return {
                input.value("text", ""),
                input.value("question", std::string(input_json)),
                input.value("mode", m_default_mode)
            };
        } catch (...) {
            return {std::string(input_json), std::string(input_json), m_default_mode};
        }
    }

    std::string get_system_prompt(const std::string& mode) {
        if (m_custom_prompts.contains(mode)) return m_custom_prompts[mode];

        std::string base = "Beantworte die gestellte Frage basierend auf dem bereitgestellten Kontext. "
                           "Antworte in striktem JSON gemäß Schema. "
                           "Fülle 'supporting_facts' mit Fakten aus dem Text und 'source_spans' mit Referenzen zum Text aus. ";

        if (mode == "direct") {
            return base + "Antworte präzise und direkt.";
        } else if (mode == "reasoned") {
            return base + "Antworte mit einer detaillierten Begründung basierend auf dem Kontext.";
        } else if (mode == "quotes") {
            return base + "Beziehe dich stark auf Zitate aus dem Text.";
        } else if (mode == "uncertainty") {
            return base + "Führe eine Unsicherheitsanalyse durch, falls der Text die Antwort nicht eindeutig hergibt.";
        }
        
        return base;
    }
};

extern "C" {
    Plugin* create_plugin() { return new QAPlugin(); }
    void destroy_plugin(Plugin* plugin) { delete plugin; }
}

} // namespace ai_plugin
