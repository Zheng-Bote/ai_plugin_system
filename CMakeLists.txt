cmake_minimum_required(VERSION 3.28)

project(ai_plugin_system 
    VERSION 0.3.1 
    DESCRIPTION "AI Plugin System - DRAFT"
    HOMEPAGE_URL "https://github.com/Zheng-Bote/ai_plugin_system"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror)
endif()

# Dependencies
find_package(CURL REQUIRED)
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    find_path(NLOHMANN_JSON_INCLUDE_DIR "nlohmann/json.hpp")
    if(NLOHMANN_JSON_INCLUDE_DIR)
        include_directories(${NLOHMANN_JSON_INCLUDE_DIR})
    endif()
endif()

# Valijson (Header-only)
find_path(VALIJSON_INCLUDE_DIR "valijson/validator.hpp")
if(VALIJSON_INCLUDE_DIR)
    include_directories(${VALIJSON_INCLUDE_DIR})
else()
    # Fallback to common location
    include_directories(/usr/include)
endif()

# Directories
include_directories(include)

# Core Library / Executable
add_executable(ai_cli
    src/main.cpp
    src/http_llm_client.cpp
    src/plugin_manager.cpp
)
target_link_libraries(ai_cli PRIVATE CURL::libcurl dl)
if(nlohmann_json_FOUND)
    target_link_libraries(ai_cli PRIVATE nlohmann_json::nlohmann_json)
endif()

# Plugins as shared libraries
add_library(text_analysis_plugin SHARED plugins/text_analysis/text_analysis_plugin.cpp)
target_include_directories(text_analysis_plugin PRIVATE include)
if(nlohmann_json_FOUND)
    target_link_libraries(text_analysis_plugin PRIVATE nlohmann_json::nlohmann_json)
endif()

add_library(sbom_heuristic_plugin SHARED plugins/sbom_heuristic/sbom_heuristic_plugin.cpp)
target_include_directories(sbom_heuristic_plugin PRIVATE include)
if(nlohmann_json_FOUND)
    target_link_libraries(sbom_heuristic_plugin PRIVATE nlohmann_json::nlohmann_json)
endif()

add_library(summarization_plugin SHARED plugins/summarization/summarization_plugin.cpp)
target_include_directories(summarization_plugin PRIVATE include)
if(nlohmann_json_FOUND)
    target_link_libraries(summarization_plugin PRIVATE nlohmann_json::nlohmann_json)
endif()

add_library(text_optimization_plugin SHARED plugins/text_optimization/text_optimization_plugin.cpp)
target_include_directories(text_optimization_plugin PRIVATE include)
if(nlohmann_json_FOUND)
    target_link_libraries(text_optimization_plugin PRIVATE nlohmann_json::nlohmann_json)
endif()

add_library(qa_plugin SHARED plugins/qa/qa_plugin.cpp)
target_include_directories(qa_plugin PRIVATE include)
if(nlohmann_json_FOUND)
    target_link_libraries(qa_plugin PRIVATE nlohmann_json::nlohmann_json)
endif()

# New Plugins v2
add_library(normalization_plugin SHARED plugins/normalization/normalization_plugin.cpp)
target_include_directories(normalization_plugin PRIVATE include)
if(nlohmann_json_FOUND)
    target_link_libraries(normalization_plugin PRIVATE nlohmann_json::nlohmann_json)
endif()

add_library(keywords_plugin SHARED plugins/keywords/keywords_plugin.cpp)
target_include_directories(keywords_plugin PRIVATE include)
if(nlohmann_json_FOUND)
    target_link_libraries(keywords_plugin PRIVATE nlohmann_json::nlohmann_json)
endif()

add_library(prompt_opt_plugin SHARED plugins/prompt_opt/prompt_opt_plugin.cpp)
target_include_directories(prompt_opt_plugin PRIVATE include)
if(nlohmann_json_FOUND)
    target_link_libraries(prompt_opt_plugin PRIVATE nlohmann_json::nlohmann_json)
endif()

add_library(embeddings_plugin SHARED plugins/embeddings/embeddings_plugin.cpp)
target_include_directories(embeddings_plugin PRIVATE include)
if(nlohmann_json_FOUND)
    target_link_libraries(embeddings_plugin PRIVATE nlohmann_json::nlohmann_json)
endif()

# Ensure output goes to build dir
set_target_properties(ai_cli text_analysis_plugin sbom_heuristic_plugin summarization_plugin text_optimization_plugin qa_plugin 
    normalization_plugin keywords_plugin prompt_opt_plugin embeddings_plugin
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/libs.txt" 
     CONTENT "$<TARGET_PROPERTY:ai_cli,LINK_LIBRARIES>")
