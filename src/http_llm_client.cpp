/**
 * SPDX-FileComment: HTTP LLM Client Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file http_llm_client.cpp
 * @brief Concrete implementation of LLMClient using libcurl
 * @version 0.1.0
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "http_llm_client_type.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <print>
#include <chrono>
#include <thread>
#include <random>

using json = nlohmann::json;

namespace ai_plugin {

// Helper for libcurl to write data into a string
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    reinterpret_cast<std::string*>(userp)->append(reinterpret_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

HttpLLMClient::HttpLLMClient(std::string_view api_key, std::string_view endpoint_url)
    : m_api_key(api_key), m_endpoint_url(endpoint_url) {
    curl_global_init(CURL_GLOBAL_ALL);
}

HttpLLMClient::~HttpLLMClient() {
    curl_global_cleanup();
}

void HttpLLMClient::set_model_routing_priority(const std::vector<std::string>& models) {
    m_routing_models = models;
}

std::expected<std::string, std::string> HttpLLMClient::perform_http_post(std::string_view payload, [[maybe_unused]] std::string_view current_model) {
    CURL* curl = curl_easy_init();
    if (!curl) return std::unexpected("Failed to initialize libcurl");

    std::string response_string;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string auth_header = "Authorization: Bearer " + m_api_key;
    headers = curl_slist_append(headers, auth_header.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, m_endpoint_url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.data());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L); // 30 seconds timeout

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        return std::unexpected(std::format("libcurl error ({}): {}", static_cast<int>(res), curl_easy_strerror(res)));
    }

    if (http_code >= 400) {
        return std::unexpected(std::format("HTTP Error {}: {}", http_code, response_string));
    }

    return response_string;
}

std::expected<LLMResponse, std::string> HttpLLMClient::query(const LLMQuery& query) {
    if (m_routing_models.empty()) {
        return std::unexpected("No routing models configured.");
    }

    int max_retries = 3;
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> jitter_dist(0, 100);

    for (const auto& model : m_routing_models) {
        int attempt = 0;
        int base_delay_ms = 1000;

        while (attempt < max_retries) {
            std::println("Attempting query with model: {} (Attempt {}/{})", model, attempt + 1, max_retries);

            // Construct JSON payload using nlohmann/json
            json payload_json = {
                {"model", model},
                {"messages", {
                    {{"role", "system"}, {"content", query.system_prompt}},
                    {{"role", "user"}, {"content", query.prompt}}
                }},
                {"temperature", query.temperature},
                {"max_tokens", query.max_tokens}
            };

            // OpenRouter / OpenAI specific JSON schema enforcement if provided
            if (!query.json_schema.empty()) {
                payload_json["response_format"] = {
                    {"type", "json_object"}
                    // Some providers also accept "schema" here
                };
            }

            auto result = perform_http_post(payload_json.dump(), model);
            
            if (result.has_value()) {
                try {
                    auto res_json = json::parse(result.value());
                    LLMResponse response;
                    // Standard OpenAI format
                    response.content = res_json["choices"][0]["message"]["content"];
                    response.model_used = model;
                    response.total_tokens = res_json["usage"]["total_tokens"];
                    response.response_hash = std::format("{:x}", std::hash<std::string>{}(response.content));
                    
                    return response;
                } catch (const std::exception& e) {
                    std::println("Failed to parse LLM response JSON: {}", e.what());
                    // Treat as failure to retry or fallback
                }
            }

            attempt++;
            if (attempt < max_retries) {
                int delay = (base_delay_ms << attempt) + jitter_dist(gen);
                std::println("Error querying model {}: {}. Retrying in {} ms", model, result.has_value() ? "JSON Parse error" : result.error(), delay);
                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            }
        }
    }

    return std::unexpected("All models failed after retries.");
}

Generator<std::string> HttpLLMClient::query_stream([[maybe_unused]] const LLMQuery& query) {
    if (m_routing_models.empty()) {
        co_yield "Error: No routing models configured.";
        co_return;
    }

    // Use the first model for streaming demo
    std::string model = m_routing_models[0];

    // Mocking streaming response for the demo
    // In a real implementation, we would use libcurl with a write callback
    // that yields tokens as they arrive via SSE (Server-Sent Events).
    
    std::string mock_response = "Dies ist eine gestreamte Antwort vom Modell " + model + ". "
                                "C++23 Coroutinen ermöglichen effizientes Token-Streaming.";
    
    std::stringstream ss(mock_response);
    std::string token;
    while (ss >> token) {
        co_yield token + " ";
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Simulate network latency
    }
}

} // namespace ai_plugin
