/**
 * SPDX-FileComment: CURL Multi Interface Manager (Coroutine Enhanced)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file curl_multi_manager.hpp
 * @brief Singleton to manage asynchronous libcurl requests with coroutine support
 * @version 0.4.2
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include <curl/curl.h>
#include <thread>
#include <mutex>
#include <map>
#include <functional>
#include <atomic>
#include <coroutine>

#include <memory>

namespace ai_plugin {

/**
 * @brief Manages a CURLM handle and polls it in a background thread.
 */
class CurlMultiManager {
public:
    /**
     * @brief Result structure for async HTTP operations.
     */
    struct HttpResult {
        CURLcode res{CURLE_OK};
        long http_code{0};
        std::string response_data;
    };

    static CurlMultiManager& instance() {
        static CurlMultiManager inst;
        return inst;
    }

    ~CurlMultiManager() {
        m_running = false;
        curl_multi_wakeup(m_multi_handle); // Wake up the thread
        if (m_worker.joinable()) m_worker.join();
        curl_multi_cleanup(m_multi_handle);
        curl_global_cleanup();
    }

    /**
     * @brief Adds a request and associates it with a suspended coroutine.
     */
    void add_request(CURL* easy_handle, std::coroutine_handle<> handle, std::shared_ptr<HttpResult> result_ptr) {
        {
            std::lock_guard lock(m_mutex);
            m_continuations[easy_handle] = {handle, result_ptr};
            curl_multi_add_handle(m_multi_handle, easy_handle);
        }
        curl_multi_wakeup(m_multi_handle); // Wake up the polling thread
    }

private:
    CurlMultiManager() {
        curl_global_init(CURL_GLOBAL_ALL);
        m_multi_handle = curl_multi_init();
        m_worker = std::thread(&CurlMultiManager::run, this);
    }

    void run() {
        while (m_running) {
            int still_running = 0;
            CURLMcode mc;
            {
                std::lock_guard lock(m_mutex);
                mc = curl_multi_perform(m_multi_handle, &still_running);
            }

            if (mc != CURLM_OK) {
                // Error handling could be added here
            }

            int msgs_in_queue = 0;
            CURLMsg* msg = nullptr;
            while ((msg = curl_multi_info_read(m_multi_handle, &msgs_in_queue))) {
                if (msg->msg == CURLMSG_DONE) {
                    CURL* easy = msg->easy_handle;
                    CURLcode res = msg->data.result;
                    long http_code = 0;
                    curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &http_code);

                    Continuation cont;
                    {
                        std::lock_guard lock(m_mutex);
                        auto it = m_continuations.find(easy);
                        if (it != m_continuations.end()) {
                            cont = it->second;
                            m_continuations.erase(it);
                        }
                        curl_multi_remove_handle(m_multi_handle, easy);
                    }
                    
                    if (cont.handle) {
                        cont.result->res = res;
                        cont.result->http_code = http_code;
                        cont.handle.resume(); // Wake up the coroutine!
                    }
                }
            }

            if (still_running == 0 && m_continuations.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            } else {
                curl_multi_wait(m_multi_handle, nullptr, 0, 50, nullptr);
            }
        }
    }

    struct Continuation {
        std::coroutine_handle<> handle;
        std::shared_ptr<HttpResult> result;
    };

    CURLM* m_multi_handle;
    std::thread m_worker;
    std::atomic<bool> m_running{true};
    std::mutex m_mutex;
    std::map<CURL*, Continuation> m_continuations;
};

} // namespace ai_plugin
