cmake_minimum_required(VERSION 3.24)
include(FetchContent)

# list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Set the output directory for the executable to bin/Debug or bin/Release depending on the build type
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

# Get the name of the current folder and use it as the project name
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME FOLDER_NAME)   
string(REPLACE " " "_" FOLDER_NAME ${FOLDER_NAME})
project(${FOLDER_NAME})

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")

set(EXECUTABLE_NAME ${PROJECT_NAME})
add_executable(${EXECUTABLE_NAME} ${SRC_FILES})

# Pass the CMAKE_SOURCE_DIR to the code, so that we can make our paths relative to it
target_compile_definitions(${EXECUTABLE_NAME} PRIVATE CMAKE_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\")

target_include_directories(${EXECUTABLE_NAME} PRIVATE "src")
target_compile_features(${EXECUTABLE_NAME} PRIVATE cxx_std_20)

# Set good compile options for the executable
if (MSVC)
    target_compile_options(${EXECUTABLE_NAME} PRIVATE /W3)
else()
    target_compile_options(${EXECUTABLE_NAME} PRIVATE -Wall -Wextra -Wpedantic -pedantic-errors)
endif()

# Minimal third-party download strategy
# Course setup: disable TLS verification for environments where CMake cannot validate cert chain.
set(CMAKE_TLS_VERIFY 0)
set(CMAKE_DEPS_DIR "${CMAKE_BINARY_DIR}/_deps")
file(MAKE_DIRECTORY "${CMAKE_DEPS_DIR}")

function(download_if_missing OUT_PATH URL)
    set(need_download OFF)
    if(NOT EXISTS "${OUT_PATH}")
        set(need_download ON)
    else()
        file(SIZE "${OUT_PATH}" existing_size)
        if(existing_size EQUAL 0)
            set(need_download ON)
        endif()
    endif()

    if(need_download)
        file(REMOVE "${OUT_PATH}")
        message(STATUS "Downloading ${URL}")
        file(DOWNLOAD "${URL}" "${OUT_PATH}" SHOW_PROGRESS STATUS download_status)
        list(GET download_status 0 status_code)
        if(NOT status_code EQUAL 0)
            list(GET download_status 1 status_message)
            message(FATAL_ERROR "Failed to download ${URL}: ${status_message}")
        endif()
        file(SIZE "${OUT_PATH}" downloaded_size)
        if(downloaded_size EQUAL 0)
            message(FATAL_ERROR "Downloaded empty file from ${URL}")
        endif()
    endif()
endfunction()

## glm (source-only via archive URL)
set(GLM_VERSION "1.0.3")
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glm URL https://github.com/g-truc/glm/archive/refs/tags/${GLM_VERSION}.tar.gz)
FetchContent_MakeAvailable(glm)

## raylib (source-only)
set(RAYLIB_VERSION "6.0")
set(BUILD_EXAMPLES OFF CACHE INTERNAL "")
FetchContent_Declare(raylib URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz)

FetchContent_MakeAvailable(raylib)

## imgui (only required source/header files)
set(IMGUI_VERSION "v1.92.1")
set(IMGUI_DIR "${CMAKE_DEPS_DIR}/imgui_src")
file(MAKE_DIRECTORY "${IMGUI_DIR}")

set(IMGUI_REQUIRED_FILES
    imgui.h
    imconfig.h
    imgui_internal.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
)

foreach(IMGUI_FILE IN LISTS IMGUI_REQUIRED_FILES)
    download_if_missing(
        "${IMGUI_DIR}/${IMGUI_FILE}"
        "https://raw.githubusercontent.com/ocornut/imgui/${IMGUI_VERSION}/${IMGUI_FILE}"
    )
endforeach()

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
)
target_include_directories(imgui PUBLIC "${IMGUI_DIR}")
target_compile_features(imgui PUBLIC cxx_std_20)

## rlimgui (only required files)
set(RLIMGUI_TAG "Raylib_6_0")
set(RLIMGUI_DIR "${CMAKE_DEPS_DIR}/rlimgui_src")
file(MAKE_DIRECTORY "${RLIMGUI_DIR}")

set(RLIMGUI_REQUIRED_FILES
    rlImGui.h
    rlImGui.cpp
    imgui_impl_raylib.h
)

foreach(RLIMGUI_FILE IN LISTS RLIMGUI_REQUIRED_FILES)
    download_if_missing(
        "${RLIMGUI_DIR}/${RLIMGUI_FILE}"
        "https://raw.githubusercontent.com/raylib-extras/rlImGui/${RLIMGUI_TAG}/${RLIMGUI_FILE}"
    )
endforeach()

add_library(rlimgui STATIC ${RLIMGUI_DIR}/rlImGui.cpp)
target_include_directories(rlimgui PUBLIC "${RLIMGUI_DIR}")
target_compile_definitions(rlimgui PUBLIC NO_FONT_AWESOME)
target_compile_features(rlimgui PUBLIC cxx_std_20)
target_link_libraries(rlimgui PUBLIC raylib imgui)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE glm::glm raylib imgui rlimgui)

# Treat raylib headers as system headers for this target to reduce warning noise.
get_target_property(RAYLIB_INCLUDE_DIRS raylib INTERFACE_INCLUDE_DIRECTORIES)
if(RAYLIB_INCLUDE_DIRS)
    target_include_directories(${EXECUTABLE_NAME} SYSTEM PRIVATE ${RAYLIB_INCLUDE_DIRS})
endif()

# Additional settings for macOS for raylib
if (APPLE)
    # Link necessary frameworks for macOS
    target_link_libraries(${EXECUTABLE_NAME} PUBLIC "-framework IOKit")
    target_link_libraries(${EXECUTABLE_NAME} PUBLIC "-framework Cocoa")
    target_link_libraries(${EXECUTABLE_NAME} PUBLIC "-framework OpenGL")
endif()