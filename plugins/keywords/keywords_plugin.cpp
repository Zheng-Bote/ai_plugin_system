/**
 * SPDX-FileComment: Keywords Plugin Implementation (Refactored)
 */
#include "base_plugin.hpp"
#include "llm_client_type.hpp"
namespace ai_plugin {
class KeywordsPlugin : public BasePlugin {
public:
    [[nodiscard]] std::expected<std::string, std::string> analyze(std::string_view input_json, LLMClient* llm_client) override {
        if (!llm_client) return std::unexpected("LLM client not provided");
        const auto input_pair = parse_input(input_json, "default");
        const auto& text = input_pair.first;
        [[maybe_unused]] const auto& mode = input_pair.second;
        LLMQuery q; q.system_prompt = "Extrahiere Keywords. JSON Output."; q.prompt = text; q.json_schema = m_schema.dump();
        auto result = llm_client->query(q);
        if (!result) return std::unexpected(result.error());
        auto res_json = nlohmann::json::parse(result->content);
        if (auto val = validate_output(res_json); !val) return std::unexpected(val.error());
        return res_json.dump(2);
    }
    [[nodiscard]] Generator<std::string> analyze_stream(std::string_view input_json, LLMClient* llm_client) override {
        if (!llm_client) { co_yield "Error"; co_return; }
        const auto input_pair = parse_input(input_json, "default");
        const auto& text = input_pair.first;
        [[maybe_unused]] const auto& mode = input_pair.second;
        LLMQuery q; q.system_prompt = "Keywords."; q.prompt = text;
        for (const auto& t : llm_client->query_stream(q)) co_yield t;
    }
    void shutdown() override {}
    [[nodiscard]] std::string_view get_name() const override { return "keywords-plugin"; }
    [[nodiscard]] std::string_view get_version() const override { return "0.3.0"; }
protected:
    [[nodiscard]] std::string get_schema_path() const override { return "data/schemas/keywords.schema.json"; }
};
extern "C" { Plugin* create_plugin() { return new KeywordsPlugin(); } void destroy_plugin(Plugin* p) { delete p; } }
}
