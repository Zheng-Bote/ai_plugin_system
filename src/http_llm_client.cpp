/**
 * SPDX-FileComment: HTTP LLM Client Implementation (True Async Await)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file http_llm_client.cpp
 * @brief Concrete implementation of LLMClient using curl_multi and co_await
 * @version 0.4.2
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "http_llm_client_type.hpp"
#include "curl_multi_manager.hpp"
#include "http_awaitable.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <print>
#include <chrono>
#include <thread>

using json = nlohmann::json;

namespace ai_plugin {

static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total = size * nmemb;
    reinterpret_cast<std::string*>(userp)->append(reinterpret_cast<char*>(contents), total);
    return total;
}

HttpLLMClient::HttpLLMClient(std::string_view api_key, std::string_view endpoint_url)
    : m_api_key(api_key), m_endpoint_url(endpoint_url) {
}

HttpLLMClient::~HttpLLMClient() {
}

void HttpLLMClient::set_model_routing_priority(const std::vector<std::string>& models) {
    m_routing_models = models;
}
/**
 * @brief Perform HTTP POST using co_await for true async suspension.
 * Parameters are passed by value to ensure they stay alive in the coroutine frame.
 */
static Task<std::expected<std::string, std::string>> perform_async_post(std::string api_key, std::string url, std::string payload) {
    CURL* curl = curl_easy_init();
    if (!curl) co_return std::unexpected("Failed to initialize libcurl");

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string auth_header = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, auth_header.c_str());
    
    // OpenRouter recommended headers
    headers = curl_slist_append(headers, "HTTP-Referer: https://github.com/google/gemini-cli");
    headers = curl_slist_append(headers, "X-Title: Gemini AI Plugin System");

    HttpAwaitable awaitable(curl);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &awaitable.result->response_data);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    // co_await suspends this coroutine until CurlMultiManager signals completion.
    auto result = co_await awaitable;

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    co_return result;
}

Task<std::expected<LLMResponse, std::string>> HttpLLMClient::query(LLMQuery query) {
    if (m_routing_models.empty()) co_return std::unexpected("No routing models configured.");

    for (const auto& model : m_routing_models) {
        json payload_json = {
            {"model", model},
            {"messages", {
                {{"role", "system"}, {"content", query.system_prompt}},
                {{"role", "user"}, {"content", query.prompt}}
            }},
            {"temperature", query.temperature}
            };

            // We rely on the strong system prompt for JSON output to maximize compatibility 
            // with models that don't support the 'structured-outputs' feature flag.

            auto result = co_await perform_async_post(m_api_key, m_endpoint_url, payload_json.dump());

        if (result.has_value()) {
            try {
                auto res_json = json::parse(result.value());
                if (res_json.contains("error")) {
                    std::println("API Error from {}: {}", model, res_json["error"].dump());
                    continue;
                }

                LLMResponse response;
                response.content = res_json["choices"][0]["message"]["content"];
                response.model_used = model;
                response.total_tokens = res_json.value("usage", json::object()).value("total_tokens", 0);
                response.response_hash = std::format("{:x}", std::hash<std::string>{}(response.content));
                co_return response;
            } catch (const std::exception& e) {
                std::println("JSON Parse Error from {}: {}", model, e.what());
            }
        } else {
            std::println("Request failed for {}: {}", model, result.error());
        }
    }

    co_return std::unexpected("All models failed.");
}

Generator<std::string> HttpLLMClient::query_stream([[maybe_unused]] LLMQuery query) {
    std::string mock_response = "Streaming is simulated in v4 for now.";
    std::stringstream ss(mock_response);
    std::string token;
    while (ss >> token) {
        co_yield token + " ";
    }
}

} // namespace ai_plugin
