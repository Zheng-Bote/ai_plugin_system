/**
 * SPDX-FileComment: Text Optimization Plugin Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file text_optimization_plugin.cpp
 * @brief Production-ready text optimization with LLM integration
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

using json = nlohmann::json;

namespace ai_plugin {

/**
 * @brief Implementation of the Text Optimization Plugin.
 */
class TextOptimizationPlugin : public Plugin {
public:
    [[nodiscard]] std::expected<void, std::string> init(std::string_view config_json) override {
        try {
            auto config = json::parse(config_json);
            m_default_mode = config.value("mode", "clarity");
            
            if (config.contains("custom_prompts")) {
                m_custom_prompts = config["custom_prompts"].get<std::unordered_map<std::string, std::string>>();
            }

            // Load schema for LLM guidance
            std::ifstream schema_file("data/schemas/text_optimization.schema.json");
            if (schema_file.is_open()) {
                m_schema = json::parse(schema_file);
            }

            std::println("TextOptimizationPlugin v0.1.0 initialized.");
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
        query.task_type = TaskType::ANALYSIS;
        query.temperature = 0.2;

        auto result = llm_client->query(query);
        if (!result) return std::unexpected(result.error());

        // Validate LLM response structure
        try {
            auto res_json = json::parse(result->content);
            if (!res_json.contains("optimized_text") || !res_json.contains("changes")) {
                return std::unexpected("LLM response missing required fields 'optimized_text' or 'changes'");
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
        query.task_type = TaskType::ANALYSIS;

        for (const auto& token : llm_client->query_stream(query)) {
            co_yield token;
        }
    }

    void shutdown() override {
        std::println("TextOptimizationPlugin shutting down.");
    }

    [[nodiscard]] std::string_view get_name() const override { return "text-optimization-plugin"; }
    [[nodiscard]] std::string_view get_version() const override { return "0.1.0"; }

private:
    std::string m_default_mode = "clarity";
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

        std::string base = "Optimiere den folgenden Text. Antworte in striktem JSON gemäß Schema. "
                           "Erfasse alle Änderungen im Array 'changes'. ";

        if (mode == "clarity") {
            return base + "Fokus: Maximale Klarheit und Verständlichkeit.";
        } else if (mode == "readability") {
            return base + "Fokus: Lesbarkeit verbessern (einfachere Sätze, bessere Struktur).";
        } else if (mode == "formal") {
            return base + "Fokus: Formale Sprache und professioneller Tonfall.";
        } else if (mode == "technical") {
            return base + "Fokus: Präzise, technischer Tonfall für Experten.";
        } else if (mode == "friendly") {
            return base + "Fokus: Höflicher, freundlicher und einladender Tonfall.";
        } else if (mode == "concise") {
            return base + "Fokus: Kürzen, Redundanzen entfernen, auf den Punkt kommen.";
        } else if (mode == "creative") {
            return base + "Fokus: Kreativerer Stil, abwechslungsreiches Vokabular.";
        }
        
        return base;
    }
};

extern "C" {
    Plugin* create_plugin() { return new TextOptimizationPlugin(); }
    void destroy_plugin(Plugin* plugin) { delete plugin; }
}

} // namespace ai_plugin
