/**
 * SPDX-FileComment: Text Summarization Plugin Implementation (Refactored)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file summarization_plugin.cpp
 * @brief Production-ready text summarization inheriting from BasePlugin
 * @version 0.3.0
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "base_plugin.hpp"
#include "llm_client_type.hpp"
#include <nlohmann/json.hpp>
#include <print>
#include <string>
#include <unordered_map>

using json = nlohmann::json;

namespace ai_plugin {

class SummarizationPlugin : public BasePlugin {
public:
    [[nodiscard]] std::expected<std::string, std::string> analyze(std::string_view input_json, LLMClient* llm_client) override {
        if (!llm_client) return std::unexpected("LLM client not provided");

        const auto input_pair = parse_input(input_json, m_default_mode);
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

        try {
            auto res_json = json::parse(result->content);
            if (auto val = validate_output(res_json); !val) return std::unexpected(val.error());
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

        const auto input_pair = parse_input(input_json, m_default_mode);
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
    [[nodiscard]] std::string_view get_version() const override { return "0.3.0"; }

protected:
    [[nodiscard]] std::expected<void, std::string> init_impl() override {
        m_default_mode = m_config.value("mode", "short");
        if (m_config.contains("custom_prompts")) {
            m_custom_prompts = m_config["custom_prompts"].get<std::unordered_map<std::string, std::string>>();
        }
        return {};
    }

    [[nodiscard]] std::string get_schema_path() const override {
        return "data/schemas/summarization.schema.json";
    }

private:
    std::string m_default_mode = "short";
    std::unordered_map<std::string, std::string> m_custom_prompts;

    std::string get_system_prompt(const std::string& mode) {
        if (m_custom_prompts.contains(mode)) return m_custom_prompts[mode];

        if (mode == "short") return "Fasse in maximal 3 Sätzen zusammen. JSON Output.";
        if (mode == "bullet") return "Erstelle Stichpunkte. JSON Output.";
        if (mode == "executive") return "Erstelle ein Executive Summary. JSON Output.";
        if (mode == "technical") return "Fasse technische Details zusammen. JSON Output.";
        
        return "Fasse den Text zusammen. JSON Output.";
    }
};

extern "C" {
    Plugin* create_plugin() { return new SummarizationPlugin(); }
    void destroy_plugin(Plugin* plugin) { delete plugin; }
}

} // namespace ai_plugin
