# curl_multi_manager.hpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | CURL Multi Interface Manager (Coroutine Enhanced) |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | Apache-2.0 |
| **File** | `curl_multi_manager.hpp` |
| **Description** | Singleton to manage asynchronous libcurl requests with coroutine support |
| **Version** | 0.4.2 |
| **Date** | 2026-04-05 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | Apache-2.0 |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `class CurlMultiManager `

> Manages a CURLM handle and polls it in a background thread.

---

### `struct HttpResult `

> Result structure for async HTTP operations.

---

### `void add_request(CURL* easy_handle, std::coroutine_handle<> handle, std::shared_ptr<HttpResult> result_ptr) `

> Adds a request and associates it with a suspended coroutine.

---

