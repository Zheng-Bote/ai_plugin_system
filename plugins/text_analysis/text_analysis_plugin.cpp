/**
 * SPDX-FileComment: Text Analysis Plugin Definition
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file text_analysis_plugin.cpp
 * @brief Text Analysis Plugin Example implementation
 * @version 0.1.0
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "plugin_type.hpp"
#include <nlohmann/json.hpp>
#include <print>
#include <thread>
#include <chrono>

using json = nlohmann::json;

namespace ai_plugin {

class TextAnalysisPlugin : public Plugin {
public:
    [[nodiscard]] std::expected<void, std::string> init(std::string_view config_json) override {
        try {
            auto config = json::parse(config_json);
            std::println("TextAnalysisPlugin initialized with config (mode: {})", config.value("mode", "default"));
        } catch (...) {
            std::println("TextAnalysisPlugin initialized with raw config: {}", config_json);
        }
        return {};
    }

    [[nodiscard]] std::expected<std::string, std::string> analyze(std::string_view input_json, [[maybe_unused]] LLMClient* llm_client) override {
        std::println("TextAnalysisPlugin analyzing: {}", input_json);
        
        std::string extracted_text;
        try {
            auto input = json::parse(input_json);
            extracted_text = input.value("text", std::string(input_json));
        } catch (...) {
            extracted_text = input_json;
        }

        // Mocking structured output
        json result = {
            {"summary", "Analyzed text: " + extracted_text},
            {"sentiment", {
                {"label", "positive"},
                {"score", 0.95}
            }},
            {"categories", {"demo", "cpp"}},
            {"confidence", 0.99}
        };
        
        return result.dump(2);
    }

    [[nodiscard]] Generator<std::string> analyze_stream(std::string_view input_json, [[maybe_unused]] LLMClient* llm_client) override {
        std::println("TextAnalysisPlugin streaming analysis...");
        
        std::string text = "Analyzing input as stream... ";
        try {
            auto input = json::parse(input_json);
            text += input.value("text", std::string(input_json));
        } catch (...) {
            text += input_json;
        }

        std::string tokens[] = {"{", "\"summary\":", "\"Starting", "streamed", "analysis", "of", "your", "input.\"", "}"};
        for (const auto& t : tokens) {
            co_yield t + " ";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void shutdown() override {
        std::println("TextAnalysisPlugin shutting down.");
    }

    [[nodiscard]] std::string_view get_name() const override {
        return "text-analysis-plugin";
    }

    [[nodiscard]] std::string_view get_version() const override {
        return "0.1.0";
    }
};

extern "C" {
    Plugin* create_plugin() {
        return new TextAnalysisPlugin();
    }

    void destroy_plugin(Plugin* plugin) {
        delete plugin;
    }
}

} // namespace ai_plugin
