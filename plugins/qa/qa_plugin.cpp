/**
 * SPDX-FileComment: Question-Answering Plugin Implementation (Refactored)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file qa_plugin.cpp
 * @brief Production-ready QA inheriting from BasePlugin
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

namespace ai_plugin {

class QAPlugin : public BasePlugin {
public:
    [[nodiscard]] Task<std::expected<std::string, std::string>> analyze(std::string input_json, LLMClient* llm_client) override {
        if (!llm_client) co_return std::unexpected("LLM client not provided");
        auto input = nlohmann::json::parse(input_json);
        LLMQuery q; q.system_prompt = "Beantworte die Frage basierend auf dem Kontext. JSON Output." + get_schema_instruction();
        q.prompt = std::format("Context: {}\nQuestion: {}", input.value("text", ""), input.value("question", ""));
        q.json_schema = m_schema.dump();
        
        auto result = co_await llm_client->query(q);
        if (!result) co_return std::unexpected(result.error());
        try {
            auto res_json = nlohmann::json::parse(result->content);
            if (auto val = validate_output(res_json); !val) co_return std::unexpected(val.error());
            co_return res_json.dump(2);
        } catch (...) { co_return std::unexpected("Invalid LLM JSON"); }
    }

    [[nodiscard]] Generator<std::string> analyze_stream(std::string input_json, LLMClient* llm_client) override {
        if (!llm_client) { co_yield "Error: LLM client not provided"; co_return; }
        auto input = nlohmann::json::parse(input_json);
        LLMQuery q; q.system_prompt = "QA Stream." + get_schema_instruction();
        q.prompt = std::format("Context: {}\nQuestion: {}", input.value("text", ""), input.value("question", ""));
        for (const auto& token : llm_client->query_stream(q)) co_yield token;
    }

    void shutdown() override {}
    [[nodiscard]] std::string_view get_name() const override { return "qa-plugin"; }
    [[nodiscard]] std::string_view get_version() const override { return "0.4.2"; }
    [[nodiscard]] std::string_view get_description() const override { return "Answers questions based on provided context."; }

protected:
    [[nodiscard]] std::string get_schema_path() const override { return "data/schemas/qa.schema.json"; }
};

extern "C" {
    Plugin* create_plugin() { return new QAPlugin(); }
    void destroy_plugin(Plugin* plugin) { delete plugin; }
}

} // namespace ai_plugin
