/**
 * SPDX-FileComment: LLM Context and Session Manager
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file session_manager.hpp
 * @brief Manages conversation history and state
 * @version 0.1.0
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace ai_plugin {

struct Message {
    std::string role;
    std::string content;
};

/**
 * @brief Manages conversation state for stateful LLM interactions.
 */
class SessionManager {
public:
    void add_message(const std::string& role, const std::string& content) {
        m_history.push_back({role, content});
    }

    nlohmann::json get_history_as_json() const {
        nlohmann::json j = nlohmann::json::array();
        for (const auto& msg : m_history) {
            j.push_back({{"role", msg.role}, {"content", msg.content}});
        }
        return j;
    }

    void clear() { m_history.clear(); }

private:
    std::vector<Message> m_history;
};

} // namespace ai_plugin
