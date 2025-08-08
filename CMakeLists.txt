cmake_minimum_required(VERSION 3.16)
project(OceroEngine LANGUAGES C CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# Options
option(OceroEngine_ENABLE_LTO "Enable link-time optimization where supported" ON)
option(OceroEngine_ENABLE_LLM "Enable LLM features (requires libcurl)" ON)

# Suppress CMake deprecation/dev warnings globally
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "Disable CMake deprecated warnings" FORCE)
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON CACHE BOOL "Suppress developer warnings" FORCE)

# FetchContent for auto-fetching dependencies when not available system-wide
include(FetchContent)

# Collect sources (reconfigures when files change)
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.c
)

add_executable(${PROJECT_NAME} ${ENGINE_SOURCES})

# Include directories for project headers and vendored headers
# - source: for includes like "opengl/..."
# - Include/GLFW/include: glad, glfw3, glm, stb_image.h, shader_m.h
# - Include/nlohmann: for "#include <json.hpp>"
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/GLFW/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/nlohmann
)

# Platform libraries
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

# Link OpenGL and dl (dl needed by glad on Unix-like)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL ${CMAKE_DL_LIBS} Threads::Threads)

## GLFW: prefer system, fallback to FetchContent
set(GLFW_LINKED FALSE)
find_package(glfw3 CONFIG QUIET)
if(glfw3_FOUND)
    if(TARGET glfw)
        target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
        set(GLFW_LINKED TRUE)
    elseif(TARGET glfw3)
        target_link_libraries(${PROJECT_NAME} PRIVATE glfw3)
        set(GLFW_LINKED TRUE)
    endif()
endif()

if(NOT GLFW_LINKED)
    message(STATUS "glfw3 not found via find_package; fetching GLFW...")
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.9
    )
    FetchContent_MakeAvailable(glfw)
    if(TARGET glfw)
        target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
    elseif(TARGET glfw3)
        target_link_libraries(${PROJECT_NAME} PRIVATE glfw3)
    else()
        message(FATAL_ERROR "GLFW fetch did not produce a known target")
    endif()
endif()

# GLM (header-only). Prefer system, fallback to FetchContent or vendored headers already in include path
find_package(glm CONFIG QUIET)
if(glm_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
else()
    message(STATUS "glm not found via find_package; fetching GLM...")
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
    )
    FetchContent_MakeAvailable(glm)
    if(TARGET glm::glm)
        target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
    else()
        # As a last resort, rely on vendored headers in Include/GLFW/include
        message(STATUS "Using vendored GLM headers from Include/GLFW/include")
    endif()
endif()

# SFML for audio and system (sleep)
set(SFML_LINKED FALSE)
find_package(SFML 2.5 COMPONENTS audio system QUIET)
if(SFML_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Audio SFML::System)
    set(SFML_LINKED TRUE)
endif()

if(NOT SFML_LINKED)
    # pkg-config fallback for SFML (optional)
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(SFML2 QUIET sfml-audio sfml-system)
        if(SFML2_FOUND)
            target_include_directories(${PROJECT_NAME} PRIVATE ${SFML2_INCLUDE_DIRS})
            target_link_libraries(${PROJECT_NAME} PRIVATE ${SFML2_LIBRARIES})
            target_compile_options(${PROJECT_NAME} PRIVATE ${SFML2_CFLAGS_OTHER})
            set(SFML_LINKED TRUE)
        endif()
    endif()
endif()

if(NOT SFML_LINKED)
    message(STATUS "SFML not found via find_package/pkg-config; fetching SFML...")
    set(SFML_BUILD_AUDIO ON CACHE BOOL "" FORCE)
    set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_GRAPHICS OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_WINDOW OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SFML_USE_STATIC_STD_LIBS OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
    )
    FetchContent_MakeAvailable(SFML)
    # Link appropriate targets depending on how SFML is provided
    if(TARGET SFML::Audio AND TARGET SFML::System)
        target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Audio SFML::System)
        set(SFML_LINKED TRUE)
    elseif(TARGET sfml-audio AND TARGET sfml-system)
        target_link_libraries(${PROJECT_NAME} PRIVATE sfml-audio sfml-system)
        set(SFML_LINKED TRUE)
    else()
        message(FATAL_ERROR "SFML fetch did not provide required targets (Audio/System)")
    endif()
endif()

# LLM toggle and CURL dependency
if(OceroEngine_ENABLE_LLM)
    target_compile_definitions(${PROJECT_NAME} PRIVATE OCERO_ENABLE_LLM=1)
    find_package(CURL REQUIRED)
    if(TARGET CURL::libcurl)
        target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)
    else()
        target_include_directories(${PROJECT_NAME} PRIVATE ${CURL_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} PRIVATE ${CURL_LIBRARIES})
    endif()
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE OCERO_ENABLE_LLM=0)
endif()

# Definitions and warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()

# Link-time optimization where supported
if(OceroEngine_ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT lto_supported OUTPUT lto_msg)
    if(lto_supported)
        set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()

# Output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy runtime assets (shaders, resources) next to the executable
set(RUNTIME_ASSET_DIR ${CMAKE_BINARY_DIR}/bin)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/source/shaders)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/source/shaders
                ${RUNTIME_ASSET_DIR}/shaders)
endif()

set(ASSET_SRC_DIR "")
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
    set(ASSET_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)
elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/source/resources)
    set(ASSET_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source/resources)
endif()

if(ASSET_SRC_DIR)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${ASSET_SRC_DIR}
                ${RUNTIME_ASSET_DIR}/resources)
endif()

# Copy window icon to expected relative path ./game/icon.png
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/source/game/icon.png)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${RUNTIME_ASSET_DIR}/game
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/source/game/icon.png
                ${RUNTIME_ASSET_DIR}/game/icon.png)
endif()

message(STATUS "Building ${PROJECT_NAME} with sources: ${ENGINE_SOURCES}")
