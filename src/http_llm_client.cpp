/**
 * SPDX-FileComment: HTTP LLM Client Implementation (Async)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file http_llm_client.cpp
 * @brief Concrete implementation of LLMClient using curl_multi
 * @version 0.2.0
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "http_llm_client_type.hpp"
#include "curl_multi_manager.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <print>
#include <chrono>
#include <thread>
#include <random>
#include <future>

using json = nlohmann::json;

namespace ai_plugin {

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
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    auto promise = std::make_shared<std::promise<std::pair<CURLcode, long>>>();
    auto future = promise->get_future();

    CurlMultiManager::instance().add_request(curl, [promise](CURLcode res, long code) {
        promise->set_value({res, code});
    });

    auto [res, http_code] = future.get();
    
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
    if (m_routing_models.empty()) return std::unexpected("No routing models configured.");

    for (const auto& model : m_routing_models) {
        json payload_json = {
            {"model", model},
            {"messages", {
                {{"role", "system"}, {"content", query.system_prompt}},
                {{"role", "user"}, {"content", query.prompt}}
            }},
            {"temperature", query.temperature},
            {"max_tokens", query.max_tokens}
        };

        if (!query.json_schema.empty()) {
            payload_json["response_format"] = {{"type", "json_object"}};
        }

        auto result = perform_http_post(payload_json.dump(), model);
        
        if (result.has_value()) {
            try {
                auto res_json = json::parse(result.value());
                
                // Check for API-level errors
                if (res_json.contains("error")) {
                    std::println("API Error from model {}: {}", model, res_json["error"].dump());
                    continue; // Try next model
                }

                LLMResponse response;
                response.content = res_json["choices"][0]["message"]["content"];
                response.model_used = model;
                response.total_tokens = res_json.value("usage", json::object()).value("total_tokens", 0);
                response.response_hash = std::format("{:x}", std::hash<std::string>{}(response.content));
                return response;
            } catch (const std::exception& e) {
                std::println("Error parsing response from model {}: {}", model, e.what());
            }
        }
    }

    return std::unexpected("All models failed.");
}

Generator<std::string> HttpLLMClient::query_stream([[maybe_unused]] const LLMQuery& query) {
    if (m_routing_models.empty()) {
        co_yield "Error: No routing models configured.";
        co_return;
    }

    // Mocking streaming for now, as real SSE streaming with curl_multi requires a more complex write_callback 
    // that pushes to a queue. For the sake of the v3 architectural demo, we mock it here.
    std::string mock_response = "Dies ist eine asynchrone Antwort (v3).";
    std::stringstream ss(mock_response);
    std::string token;
    while (ss >> token) {
        co_yield token + " ";
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

} // namespace ai_plugin
