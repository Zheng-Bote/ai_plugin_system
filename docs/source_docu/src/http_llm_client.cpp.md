# http_llm_client.cpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | HTTP LLM Client Implementation (True Async Await) |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | Apache-2.0 |
| **File** | `http_llm_client.cpp` |
| **Description** | Concrete implementation of LLMClient using curl_multi and co_await |
| **Version** | 0.4.2 |
| **Date** | 2026-04-05 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | Apache-2.0 |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `static Task<std::expected<std::string, std::string>> perform_async_post(std::string api_key, std::string url, std::string payload) `

> Perform HTTP POST using co_await for true async suspension.

---

