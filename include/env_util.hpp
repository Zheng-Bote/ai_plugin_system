/**
 * SPDX-FileComment: Simple Env Loader Definition
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file env_util.hpp
 * @brief Simple .env file parser
 * @version 0.4.2
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include <string>
#include <string_view>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <cstdlib>

namespace ai_plugin {

/**
 * @brief Simple utility to load environment variables from a .env file.
 */
class EnvLoader {
public:
    static void load(std::string_view file_path) {
        std::ifstream file(file_path.data());
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line)) {
            // Remove comments
            auto comment_pos = line.find('#');
            if (comment_pos != std::string::npos) {
                line.erase(comment_pos);
            }

            // Skip empty lines
            if (line.empty()) continue;

            auto eq_pos = line.find('=');
            if (eq_pos == std::string::npos) continue;

            std::string key = line.substr(0, eq_pos);
            std::string value = line.substr(eq_pos + 1);

            // Simple trim
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            if (!key.empty()) {
                setenv(key.c_str(), value.c_str(), 1);
            }
        }
    }

    static std::string get(std::string_view key, std::string_view default_val = "") {
        const char* val = std::getenv(key.data());
        return val ? std::string(val) : std::string(default_val);
    }
};

} // namespace ai_plugin
