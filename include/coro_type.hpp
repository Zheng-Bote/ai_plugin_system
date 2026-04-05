/**
 * SPDX-FileComment: Coroutine Support Types
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file coro_type.hpp
 * @brief Minimal Task and Generator implementations for C++20 Coroutines
 * @version 0.1.0
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include <coroutine>
#include <optional>
#include <utility>
#include <exception>

namespace ai_plugin {

/**
 * @brief A simple Task type for asynchronous operations.
 */
template <typename T>
struct Task {
    struct promise_type {
        T result;
        auto get_return_object() { return Task{std::coroutine_handle<promise_type>::from_promise(*this)}; }
        auto initial_suspend() { return std::suspend_always{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }
        void return_value(T value) { result = std::move(value); }
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> handle;

    explicit Task(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~Task() { if (handle) handle.destroy(); }

    T get() {
        if (!handle.done()) handle.resume();
        return std::move(handle.promise().result);
    }
};

/**
 * @brief A simple Generator type for streaming data.
 */
template <typename T>
struct Generator {
    struct promise_type {
        T current_value;
        auto get_return_object() { return Generator{std::coroutine_handle<promise_type>::from_promise(*this)}; }
        auto initial_suspend() { return std::suspend_always{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }
        auto yield_value(T value) {
            current_value = std::move(value);
            return std::suspend_always{};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> handle;

    explicit Generator(std::coroutine_handle<promise_type> h) : handle(h) {}
    Generator(Generator&& other) noexcept : handle(other.handle) { other.handle = nullptr; }
    ~Generator() { if (handle) handle.destroy(); }

    struct Iterator {
        std::coroutine_handle<promise_type> h;
        bool done;

        void operator++() {
            h.resume();
            done = h.done();
        }
        const T& operator*() const { return h.promise().current_value; }
        bool operator!=(const Iterator& other) const { return done != other.done; }
    };

    Iterator begin() {
        if (handle) {
            handle.resume();
            return {handle, handle.done()};
        }
        return {nullptr, true};
    }
    Iterator end() { return {nullptr, true}; }
};

} // namespace ai_plugin
