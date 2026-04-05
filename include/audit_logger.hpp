/**
 * SPDX-FileComment: Audit and JSON Logging Utility
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file audit_logger.hpp
 * @brief Persistent JSON logging for auditing LLM requests
 * @version 0.4.2
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <chrono>
#include <format>

namespace ai_plugin {

/**
 * @brief Logs LLM transactions to a JSON file for auditing.
 */
class AuditLogger {
public:
    explicit AuditLogger(const std::string& log_file) : m_log_file(log_file) {}

    void log_transaction(const std::string& model, const std::string& prompt, const std::string& response, long status_code) {
        nlohmann::json entry;
        auto now = std::chrono::system_clock::now();
        
        entry["timestamp"] = std::format("{:%FT%TZ}", now);
        entry["model"] = model;
        entry["prompt_hash"] = std::hash<std::string>{}(prompt);
        entry["response_hash"] = std::hash<std::string>{}(response);
        entry["status"] = status_code;

        std::ofstream f(m_log_file, std::ios::app);
        if (f.is_open()) {
            f << entry.dump() << std::endl;
        }
    }

private:
    std::string m_log_file;
};

} // namespace ai_plugin
