/**
 * SPDX-FileComment: HTTP LLM Client Definition (True Async Support)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file http_llm_client_type.hpp
 * @brief Concrete implementation of LLMClient using libcurl
 * @version 0.4.2
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
 * @brief Concrete implementation of the LLMClient using HTTP.
 */
class HttpLLMClient : public LLMClient {
public:
    HttpLLMClient(std::string_view api_key, std::string_view endpoint_url);
    ~HttpLLMClient() override;

    [[nodiscard]] Task<std::expected<LLMResponse, std::string>> query(LLMQuery query) override;

    [[nodiscard]] Generator<std::string> query_stream(LLMQuery query) override;

    void set_model_routing_priority(const std::vector<std::string>& models) override;

private:
    std::string m_api_key;
    std::string m_endpoint_url;
    std::vector<std::string> m_routing_models;
};

} // namespace ai_plugin
