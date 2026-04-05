/**
 * SPDX-FileComment: Text Optimization Plugin Implementation (Refactored)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file text_optimization_plugin.cpp
 * @brief Production-ready text optimization inheriting from BasePlugin
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

namespace ai_plugin {

class TextOptimizationPlugin : public BasePlugin {
public:
    [[nodiscard]] std::expected<std::string, std::string> analyze(std::string_view input_json, LLMClient* llm_client) override {
        if (!llm_client) return std::unexpected("LLM client not provided");
        const auto input_pair = parse_input(input_json, "clarity");
        const auto& text = input_pair.first;
        [[maybe_unused]] const auto& mode = input_pair.second;
        LLMQuery q;
        q.system_prompt = "Optimiere den Text. JSON Output.";
        q.prompt = text; q.json_schema = m_schema.dump();
        auto result = llm_client->query(q);
        if (!result) return std::unexpected(result.error());
        try {
            auto res_json = nlohmann::json::parse(result->content);
            if (auto val = validate_output(res_json); !val) return std::unexpected(val.error());
            return res_json.dump(2);
        } catch (...) { return std::unexpected("Invalid LLM JSON"); }
    }

    [[nodiscard]] Generator<std::string> analyze_stream(std::string_view input_json, LLMClient* llm_client) override {
        if (!llm_client) { co_yield "Error: LLM client not provided"; co_return; }
        const auto input_pair = parse_input(input_json, "clarity");
        const auto& text = input_pair.first;
        [[maybe_unused]] const auto& mode = input_pair.second;
        LLMQuery q; q.system_prompt = "Optimiere Text. JSON Stream."; q.prompt = text;
        for (const auto& token : llm_client->query_stream(q)) co_yield token;
    }

    void shutdown() override {}
    [[nodiscard]] std::string_view get_name() const override { return "text-optimization-plugin"; }
    [[nodiscard]] std::string_view get_version() const override { return "0.3.0"; }

protected:
    [[nodiscard]] std::string get_schema_path() const override { return "data/schemas/text_optimization.schema.json"; }
};

extern "C" {
    Plugin* create_plugin() { return new TextOptimizationPlugin(); }
    void destroy_plugin(Plugin* plugin) { delete plugin; }
}

} // namespace ai_plugin
