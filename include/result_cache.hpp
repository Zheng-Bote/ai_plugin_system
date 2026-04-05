/**
 * SPDX-FileComment: In-Memory LRU Cache
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file result_cache.hpp
 * @brief Thread-safe LRU cache for LLM results
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
#include <unordered_map>
#include <list>
#include <mutex>
#include <optional>

namespace ai_plugin {

/**
 * @brief Simple LRU Cache for storing analysis results.
 */
class ResultCache {
public:
    explicit ResultCache(size_t max_size) : m_max_size(max_size) {}

    std::optional<std::string> get(const std::string& key) {
        std::lock_guard lock(m_mutex);
        auto it = m_cache.find(key);
        if (it == m_cache.end()) return std::nullopt;

        // Move to front (MRU)
        m_lru_list.splice(m_lru_list.begin(), m_lru_list, it->second);
        return it->second->second;
    }

    void put(const std::string& key, const std::string& value) {
        std::lock_guard lock(m_mutex);
        auto it = m_cache.find(key);
        if (it != m_cache.end()) {
            m_lru_list.splice(m_lru_list.begin(), m_lru_list, it->second);
            it->second->second = value;
            return;
        }

        if (m_cache.size() >= m_max_size) {
            auto last = m_lru_list.back();
            m_cache.erase(last.first);
            m_lru_list.pop_back();
        }

        m_lru_list.push_front({key, value});
        m_cache[key] = m_lru_list.begin();
    }

private:
    size_t m_max_size;
    std::mutex m_mutex;
    std::list<std::pair<std::string, std::string>> m_lru_list;
    std::unordered_map<std::string, std::list<std::pair<std::string, std::string>>::iterator> m_cache;
};

} // namespace ai_plugin
