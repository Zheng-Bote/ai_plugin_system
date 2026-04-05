/**
 * SPDX-FileComment: AI Plugin CLI Main Entry Point
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file main.cpp
 * @brief Main entry point for the AI Plugin CLI system
 * @version 0.4.2
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "plugin_manager.hpp"
#include "plugin_registry.hpp"
#include "http_llm_client_type.hpp"
#include "env_util.hpp"
#include <print>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void print_usage(std::string_view program_name) {
    std::println("Usage: {} <env_file> <plugin_name_or_path> <input_text_or_json>", program_name);
    std::println("Options:");
    std::println("  --list    List all discovered plugins in ./build/lib");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    // 1. Load Environment Variables
    std::string env_path = argv[1];
    ai_plugin::EnvLoader::load(env_path);
    
    // 2. Initialize Core components
    ai_plugin::PluginManager plugin_manager;
    ai_plugin::PluginRegistry registry(plugin_manager);

    // Dynamic Discovery: 
    // a) Check .env for PLUGINS_DIR
    std::string env_plugins_dir = ai_plugin::EnvLoader::get("PLUGINS_DIR");
    if (!env_plugins_dir.empty()) {
        registry.scan_directory(env_plugins_dir);
    }

    // b) Check relative to binary (e.g. if binary is in build/bin, check build/lib)
    std::filesystem::path exe_path = std::filesystem::canonical("/proc/self/exe").parent_path();
    std::vector<std::filesystem::path> search_paths = {
        exe_path / "lib",
        exe_path / "../lib",
        "./build/lib",
        "./plugins"
    };

    for (const auto& path : search_paths) {
        registry.scan_directory(path);
    }

    if (std::string(argv[1]) == "--list") {
        std::println("\nDiscovered Plugins:");
        if (registry.get_all_plugins().empty()) {
            std::println(" (No plugins found in search paths)");
        }
        for (const auto& [name, plugin] : registry.get_all_plugins()) {
            std::println(" - {:<15} | v{:<8} | {}", name, plugin->get_version(), plugin->get_description());
        }
        return 0;
    }

    std::string plugin_ref = argv[2];
    std::string input_data = argv[3];

    std::string api_key = ai_plugin::EnvLoader::get("OPENROUTER_API_KEY");
    std::string endpoint = ai_plugin::EnvLoader::get("LLM_ENDPOINT", "https://openrouter.ai/api/v1/chat/completions");
    
    ai_plugin::HttpLLMClient llm_client(api_key, endpoint);
    std::vector<std::string> models;
    for (int i = 1; i <= 10; ++i) {
        std::string model = ai_plugin::EnvLoader::get(std::format("LLM_{:02}", i));
        if (!model.empty()) models.push_back(model);
    }
    if (models.empty()) {
        // Fallback for older .env files without leading zero
        for (int i = 1; i <= 9; ++i) {
            std::string model = ai_plugin::EnvLoader::get(std::format("LLM_0{}", i));
            if (!model.empty()) models.push_back(model);
        }
    }
    llm_client.set_model_routing_priority(models);

    // 3. Load Plugin (from registry or direct path)
    ai_plugin::Plugin* plugin = registry.get_plugin(plugin_ref);
    if (!plugin) {
        // Fallback to manual path loading
        auto res = plugin_manager.load_plugin(plugin_ref);
        if (!res) {
            std::println("Error: Plugin '{}' not found in registry and path is invalid.", plugin_ref);
            return 1;
        }
        plugin = res.value();
    }

    // 4. Process Input
    json plugin_input;
    try { plugin_input = json::parse(input_data); } 
    catch (...) { plugin_input = { {"text", input_data} }; }

    // 5. Run Analysis (Async)
    std::println("\n--- Asynchronous Analysis ---");
    auto result_task = plugin->analyze(plugin_input.dump(), &llm_client);
    auto result = result_task.get(); // In main we wait for the final result

    if (result) {
        std::println("Result: \n{}", result.value());
    } else {
        std::println("Analysis failed: {}", result.error());
    }

    return 0;
}
