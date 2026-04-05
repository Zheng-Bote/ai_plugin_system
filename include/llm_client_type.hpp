/**
 * SPDX-FileComment: LLM Client Interface Definition (Coroutine Optimized)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file llm_client_type.hpp
 * @brief Definition of the generic LLM Client Interface and Data Types
 * @version 0.4.2
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

enum class TaskType {
    ANALYSIS,
    CODE,
    SUMMARIZATION,
    UNKNOWN
};

struct LLMQuery {
    std::string prompt;
    std::string system_prompt;
    std::string json_schema;
    TaskType task_type = TaskType::UNKNOWN;
    size_t max_tokens = 1024;
    double temperature = 0.0;
};

struct LLMResponse {
    std::string content;
    std::string model_used;
    size_t total_tokens;
    std::string response_hash;
};

/**
 * @brief The Generic LLMClient Interface.
 */
class LLMClient {
public:
    virtual ~LLMClient() = default;

    /**
     * @brief Sends a structured prompt to the LLM. Now a Task for true async awaiting.
     */
    [[nodiscard]] virtual Task<std::expected<LLMResponse, std::string>> query(LLMQuery query) = 0;

    /**
     * @brief Sends a streaming request to the LLM.
     */
    [[nodiscard]] virtual Generator<std::string> query_stream(LLMQuery query) = 0;

    virtual void set_model_routing_priority(const std::vector<std::string>& models) = 0;
};

} // namespace ai_plugin
