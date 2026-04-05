/**
 * SPDX-FileComment: HTTP Awaitable for Coroutines
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file http_awaitable.hpp
 * @brief C++20 Awaitable for non-blocking libcurl requests
 * @version 0.4.2
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "curl_multi_manager.hpp"
#include <coroutine>
#include <expected>
#include <string>
#include <memory>

namespace ai_plugin {

/**
 * @brief An awaitable object that suspends the coroutine until a libcurl request finishes.
 */
struct HttpAwaitable {
    CURL* easy_handle;
    std::shared_ptr<CurlMultiManager::HttpResult> result;

    explicit HttpAwaitable(CURL* handle) 
        : easy_handle(handle), result(std::make_shared<CurlMultiManager::HttpResult>()) {}

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> handle) {
        CurlMultiManager::instance().add_request(easy_handle, handle, result);
    }

    std::expected<std::string, std::string> await_resume() {
        if (result->res != CURLE_OK) {
            return std::unexpected(std::format("libcurl error: {}", curl_easy_strerror(result->res)));
        }
        if (result->http_code >= 400) {
            return std::unexpected(std::format("HTTP Error {}: {}", result->http_code, result->response_data));
        }
        return result->response_data;
    }
};

} // namespace ai_plugin
