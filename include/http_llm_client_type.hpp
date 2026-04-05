/**
 * SPDX-FileComment: HTTP LLM Client Definition
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file http_llm_client_type.hpp
 * @brief Concrete implementation of LLMClient using libcurl
 * @version 0.1.0
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "llm_client_type.hpp"
#include <string>
#include <vector>
#include <expected>

namespace ai_plugin {

/**
 * @brief Concrete implementation of the LLMClient using HTTP (libcurl mocked).
 */
class HttpLLMClient : public LLMClient {
public:
    /**
     * @brief Construct a new HttpLLMClient
     * 
     * @param api_key The API key for authorization
     * @param endpoint_url The REST endpoint url (e.g. OpenRouter or OpenAI)
     */
    HttpLLMClient(std::string_view api_key, std::string_view endpoint_url);

    /**
     * @brief Destroy the HttpLLMClient
     */
    ~HttpLLMClient() override;

    [[nodiscard]] std::expected<LLMResponse, std::string> query(const LLMQuery& query) override;

    [[nodiscard]] Generator<std::string> query_stream(const LLMQuery& query) override;

    void set_model_routing_priority(const std::vector<std::string>& models) override;

private:
    std::string m_api_key;
    std::string m_endpoint_url;
    std::vector<std::string> m_routing_models;

    [[nodiscard]] std::expected<std::string, std::string> perform_http_post(std::string_view payload, std::string_view current_model);
};

} // namespace ai_plugin
