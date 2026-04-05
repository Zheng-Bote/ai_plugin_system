/**
 * SPDX-FileComment: Circuit Breaker Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file circuit_breaker.hpp
 * @brief Prevents cascading failures when LLM providers are down
 * @version 0.4.2
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include <chrono>
#include <atomic>
#include <string>

namespace ai_plugin {

/**
 * @brief Simple Circuit Breaker state machine.
 */
class CircuitBreaker {
public:
    enum class State { CLOSED, OPEN, HALF_OPEN };

    CircuitBreaker(int failure_threshold, std::chrono::seconds reset_timeout)
        : m_failure_threshold(failure_threshold), m_reset_timeout(reset_timeout) {}

    bool allow_request() {
        if (m_state == State::OPEN) {
            auto now = std::chrono::steady_clock::now();
            if (now - m_last_failure_time > m_reset_timeout) {
                m_state = State::HALF_OPEN;
                return true;
            }
            return false;
        }
        return true;
    }

    void record_success() {
        m_failure_count = 0;
        m_state = State::CLOSED;
    }

    void record_failure() {
        m_failure_count++;
        m_last_failure_time = std::chrono::steady_clock::now();
        if (m_failure_count >= m_failure_threshold) {
            m_state = State::OPEN;
        }
    }

    State get_state() const { return m_state; }

private:
    State m_state{State::CLOSED};
    int m_failure_threshold;
    std::chrono::seconds m_reset_timeout;
    std::atomic<int> m_failure_count{0};
    std::chrono::steady_clock::time_point m_last_failure_time;
};

} // namespace ai_plugin
