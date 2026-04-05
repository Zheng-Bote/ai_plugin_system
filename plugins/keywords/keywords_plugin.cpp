/**
 * SPDX-FileComment: Keywords Plugin Implementation (Refactored)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file keywords_plugin.cpp
 * @brief Production-ready keyword extraction inheriting from BasePlugin
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
namespace ai_plugin {
class KeywordsPlugin : public BasePlugin {
public:
    [[nodiscard]] Task<std::expected<std::string, std::string>> analyze(std::string input_json, LLMClient* llm_client) override {
        if (!llm_client) co_return std::unexpected("LLM client not provided");
        const auto input_pair = parse_input(input_json, "default");
        const auto& text = input_pair.first;
        [[maybe_unused]] const auto& mode = input_pair.second;
        LLMQuery q; q.system_prompt = std::string("Extrahiere Keywords. JSON Output.") + get_schema_instruction(); q.prompt = text; q.json_schema = m_schema.dump();
        
        auto result = co_await llm_client->query(q);
        if (!result) co_return std::unexpected(result.error());
        auto res_json = nlohmann::json::parse(result->content);
        if (auto val = validate_output(res_json); !val) co_return std::unexpected(val.error());
        co_return res_json.dump(2);
    }
    [[nodiscard]] Generator<std::string> analyze_stream(std::string input_json, LLMClient* llm_client) override {
        if (!llm_client) { co_yield "Error"; co_return; }
        const auto input_pair = parse_input(input_json, "default");
        const auto& text = input_pair.first;
        [[maybe_unused]] const auto& mode = input_pair.second;
        LLMQuery q; q.system_prompt = std::string("Keywords.") + get_schema_instruction(); q.prompt = text;
        for (const auto& t : llm_client->query_stream(q)) co_yield t;
    }
    void shutdown() override {}
    [[nodiscard]] std::string_view get_name() const override { return "keywords-plugin"; }
    [[nodiscard]] std::string_view get_version() const override { return "0.4.2"; }
    [[nodiscard]] std::string_view get_description() const override { return "Extracts key terms and topics from text."; }
protected:
    [[nodiscard]] std::string get_schema_path() const override { return "data/schemas/keywords.schema.json"; }
};
extern "C" { Plugin* create_plugin() { return new KeywordsPlugin(); } void destroy_plugin(Plugin* p) { delete p; } }
}
