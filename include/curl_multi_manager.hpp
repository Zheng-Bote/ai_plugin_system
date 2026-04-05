/**
 * SPDX-FileComment: CURL Multi Interface Manager
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file curl_multi_manager.hpp
 * @brief Singleton to manage asynchronous libcurl requests
 * @version 0.1.0
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
#include <condition_variable>
#include <map>
#include <functional>
#include <atomic>
#include <vector>

namespace ai_plugin {

/**
 * @brief Manages a CURLM handle and polls it in a background thread.
 */
class CurlMultiManager {
public:
    static CurlMultiManager& instance() {
        static CurlMultiManager inst;
        return inst;
    }

    ~CurlMultiManager() {
        m_running = false;
        if (m_worker.joinable()) m_jthread.request_stop();
        if (m_worker.joinable()) m_worker.join();
        curl_multi_cleanup(m_multi_handle);
    }

    void add_request(CURL* easy_handle, std::function<void(CURLcode, long)> callback) {
        std::lock_guard lock(m_mutex);
        m_callbacks[easy_handle] = callback;
        curl_multi_add_handle(m_multi_handle, easy_handle);
    }

private:
    CurlMultiManager() {
        m_multi_handle = curl_multi_init();
        m_worker = std::thread(&CurlMultiManager::run, this);
    }

    void run() {
        while (m_running) {
            int still_running = 0;
            {
                std::lock_guard lock(m_mutex);
                curl_multi_perform(m_multi_handle, &still_running);
            }

            int msgs_in_queue = 0;
            CURLMsg* msg = nullptr;
            while ((msg = curl_multi_info_read(m_multi_handle, &msgs_in_queue))) {
                if (msg->msg == CURLMSG_DONE) {
                    CURL* easy = msg->easy_handle;
                    CURLcode res = msg->data.result;
                    long http_code = 0;
                    curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &http_code);

                    std::function<void(CURLcode, long)> cb;
                    {
                        std::lock_guard lock(m_mutex);
                        cb = m_callbacks[easy];
                        m_callbacks.erase(easy);
                        curl_multi_remove_handle(m_multi_handle, easy);
                    }
                    if (cb) cb(res, http_code);
                }
            }

            if (still_running == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            } else {
                curl_multi_wait(m_multi_handle, nullptr, 0, 100, nullptr);
            }
        }
    }

    CURLM* m_multi_handle;
    std::thread m_worker;
    std::stop_source m_jthread;
    std::atomic<bool> m_running{true};
    std::mutex m_mutex;
    std::map<CURL*, std::function<void(CURLcode, long)>> m_callbacks;
};

} // namespace ai_plugin
