/**
 * SPDX-FileComment: Main entry point for AI Plugin CLI v1
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file main.cpp
 * @brief CLI application with dynamic plugin loading and real LLM integration
 * @version 0.1.0
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "plugin_manager.hpp"
#include "http_llm_client_type.hpp"
#include "env_util.hpp"
#include <print>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void print_usage(std::string_view program_name) {
    std::println("Usage: {} <env_file> <plugin_so_path> <input_text_or_json>", program_name);
}

int main(int argc, char** argv) {
    if (argc < 4) {
        print_usage(argv[0]);
        return 1;
    }

    std::string env_path = argv[1];
    std::string plugin_path = argv[2];
    std::string input_data = argv[3];

    // 1. Load Environment Variables
    ai_plugin::EnvLoader::load(env_path);
    std::string api_key = ai_plugin::EnvLoader::get("OPENROUTER_API_KEY");
    std::string endpoint = ai_plugin::EnvLoader::get("LLM_ENDPOINT", "https://openrouter.ai/api/v1/chat/completions");
    
    if (api_key.empty()) {
        std::println("Error: OPENROUTER_API_KEY not found in {}", env_path);
        return 1;
    }

    // 2. Initialize LLM Client
    ai_plugin::HttpLLMClient llm_client(api_key, endpoint);
    std::vector<std::string> models;
    for (int i = 1; i <= 5; ++i) {
        std::string model = ai_plugin::EnvLoader::get(std::format("LLM_0{}", i));
        if (!model.empty()) models.push_back(model);
    }
    if (models.empty()) {
        models.push_back("meta-llama/llama-3-70b-instruct"); // Default fallback
    }
    llm_client.set_model_routing_priority(models);

    // 3. Load Plugin Dynamically
    ai_plugin::PluginManager plugin_manager;
    auto plugin_res = plugin_manager.load_plugin(plugin_path);
    if (!plugin_res) {
        std::println("Error loading plugin: {}", plugin_res.error());
        return 1;
    }
    ai_plugin::Plugin* plugin = plugin_res.value();

    // 4. Initialize Plugin
    json config = { {"mode", "production"} };
    if (auto res = plugin->init(config.dump()); !res) {
        std::println("Error initializing plugin: {}", res.error());
        return 1;
    }

    // 5. Process Input (Text or JSON)
    json plugin_input;
    try {
        // Try to parse as JSON
        plugin_input = json::parse(input_data);
        std::println("Processing JSON input...");
    } catch (...) {
        // Otherwise treat as plain text
        std::println("Processing plain text input...");
        plugin_input = { {"text", input_data} };
    }

    // 6. Run Analysis
    std::println("\n--- Synchronous Analysis ---");
    auto result = plugin->analyze(plugin_input.dump(), &llm_client);
    if (result) {
        std::println("Result: \n{}", result.value());
    } else {
        std::println("Analysis failed: {}", result.error());
    }

    // 7. Run Streaming Analysis
    std::println("\n--- Streaming Analysis ---");
    for (const auto& chunk : plugin->analyze_stream(plugin_input.dump(), &llm_client)) {
        std::print("{}", chunk);
        std::cout.flush(); // Ensure output is visible during streaming
    }
    std::println("\n--- Stream Complete ---\n");

    // 8. Test LLM Streaming directly
    std::println("--- LLM Direct Streaming Test ---");
    ai_plugin::LLMQuery stream_query{"Erzähle mir kurz etwas über C++23.", "Du bist ein Experte.", "", ai_plugin::TaskType::SUMMARIZATION, 512, 0.7};
    for (const auto& token : llm_client.query_stream(stream_query)) {
        std::print("{}", token);
        std::cout.flush();
    }
    std::println("\n--- LLM Stream Complete ---");

    return 0;
}
