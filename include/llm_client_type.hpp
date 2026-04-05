/**
 * SPDX-FileComment: LLM Client Interface Definition
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file llm_client_type.hpp
 * @brief Definition of the generic LLM Client Interface and Data Types
 * @version 0.1.0
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "coro_type.hpp"
#include <string>
#include <string_view>
#include <expected>
#include <vector>

namespace ai_plugin {

/**
 * @brief Enum for different routing requirements.
 */
enum class TaskType {
    ANALYSIS,
    CODE,
    SUMMARIZATION,
    UNKNOWN
};

/**
 * @brief Struct defining parameters for LLM queries.
 */
struct LLMQuery {
    std::string prompt;
    std::string system_prompt;
    std::string json_schema;
    TaskType task_type = TaskType::UNKNOWN;
    size_t max_tokens = 1024;
    double temperature = 0.0;
};

/**
 * @brief Struct holding LLM response.
 */
struct LLMResponse {
    std::string content;
    std::string model_used;
    size_t total_tokens;
    std::string response_hash;
};

/**
 * @brief The Generic LLMClient Interface for exchanging messages with various providers.
 */
class LLMClient {
public:
    virtual ~LLMClient() = default;

    /**
     * @brief Sends a structured prompt to the LLM and enforces JSON schema output.
     * 
     * @param query The parameters and payload for the LLM request.
     * @return std::expected<LLMResponse, std::string> The generated content or error message.
     */
    [[nodiscard]] virtual std::expected<LLMResponse, std::string> query(const LLMQuery& query) = 0;

    /**
     * @brief Sends a streaming request to the LLM.
     * 
     * @param query The parameters and payload for the LLM request.
     * @return Generator<std::string> A generator yielding tokens.
     */
    [[nodiscard]] virtual Generator<std::string> query_stream(const LLMQuery& query) = 0;

    /**
     * @brief Set the priority list of models for routing.
     * 
     * @param models Vector of model strings (e.g. from .env file).
     */
    virtual void set_model_routing_priority(const std::vector<std::string>& models) = 0;
};

} // namespace ai_plugin
