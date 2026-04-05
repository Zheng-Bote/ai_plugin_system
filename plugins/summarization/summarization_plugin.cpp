/**
 * SPDX-FileComment: Text Summarization Plugin Implementation (Async Task)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file summarization_plugin.cpp
 * @brief Text Summarization Plugin Implementation
 * @version 0.4.2
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

using json = nlohmann::json;

namespace ai_plugin {

class SummarizationPlugin : public BasePlugin {
public:
    [[nodiscard]] Task<std::expected<std::string, std::string>> analyze(std::string input_json, LLMClient* llm_client) override {
        if (!llm_client) co_return std::unexpected("LLM client not provided");

        const auto input_pair = parse_input(input_json, m_default_mode);
        const auto& text = input_pair.first;
        const auto& mode = input_pair.second;
        
        LLMQuery query;
        query.system_prompt = get_system_prompt(mode) + get_schema_instruction();
        query.prompt = text;
        query.json_schema = m_schema.dump();
        query.task_type = TaskType::SUMMARIZATION;
        query.temperature = 0.3;

        // True async co_await!
        auto result = co_await llm_client->query(query);
        if (!result) co_return std::unexpected(result.error());

        try {
            auto res_json = json::parse(result->content);
            if (auto val = validate_output(res_json); !val) co_return std::unexpected(val.error());
            co_return res_json.dump(2);
        } catch (...) {
            co_return std::unexpected("Invalid JSON from LLM");
        }
    }

    [[nodiscard]] Generator<std::string> analyze_stream(std::string input_json, LLMClient* llm_client) override {
        if (!llm_client) { co_yield "Error"; co_return; }
        const auto input_pair = parse_input(input_json, m_default_mode);
        LLMQuery q; q.system_prompt = get_system_prompt(input_pair.second) + get_schema_instruction(); q.prompt = input_pair.first;
        for (const auto& token : llm_client->query_stream(q)) co_yield token;
    }

    void shutdown() override {}
    [[nodiscard]] std::string_view get_name() const override { return "summarization"; }
    [[nodiscard]] std::string_view get_version() const override { return "0.4.2"; }
    [[nodiscard]] std::string_view get_description() const override { return "Summarizes long texts into various formats."; }

protected:
    [[nodiscard]] std::expected<void, std::string> init_impl() override {
        m_default_mode = m_config.value("mode", "short");
        return {};
    }
    [[nodiscard]] std::string get_schema_path() const override { return "data/schemas/summarization.schema.json"; }

private:
    std::string m_default_mode = "short";
    std::string get_system_prompt(const std::string& mode) {
        if (mode == "executive") return "Executive Summary mode.";
        return "General summary mode.";
    }
};

extern "C" { Plugin* create_plugin() { return new SummarizationPlugin(); } void destroy_plugin(Plugin* p) { delete p; } }
}
