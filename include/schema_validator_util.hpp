/**
 * SPDX-FileComment: JSON Schema Validation Utility using valijson
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file schema_validator_util.hpp
 * @brief Helper to validate nlohmann::json against a schema file
 * @version 0.1.0
 * @date 2026-04-05
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include <nlohmann/json.hpp>
#include <valijson/adapters/nlohmann_json_adapter.hpp>
#include <valijson/utils/nlohmann_json_utils.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>
#include <expected>
#include <string>

namespace ai_plugin {

/**
 * @brief Validates a JSON object against a JSON schema using valijson.
 */
class SchemaValidator {
public:
    /**
     * @brief Validate JSON data against a JSON schema.
     * 
     * @param data The JSON data to validate.
     * @param schema_json The JSON schema to validate against.
     * @return std::expected<void, std::string> Success or validation error description.
     */
    static std::expected<void, std::string> validate(const nlohmann::json& data, const nlohmann::json& schema_json) {
        valijson::Schema schema;
        valijson::SchemaParser parser;
        valijson::adapters::NlohmannJsonAdapter schema_adapter(schema_json);

        try {
            parser.populateSchema(schema_adapter, schema);
        } catch (const std::exception& e) {
            return std::unexpected(std::format("Schema parsing failed: {}", e.what()));
        }

        valijson::Validator validator;
        valijson::adapters::NlohmannJsonAdapter data_adapter(data);
        valijson::ValidationResults results;

        if (!validator.validate(schema, data_adapter, &results)) {
            std::string errors = "JSON validation failed:";
            valijson::ValidationResults::Error error;
            while (results.popError(error)) {
                errors += "\n - " + error.description;
            }
            return std::unexpected(errors);
        }

        return {};
    }
};

} // namespace ai_plugin
