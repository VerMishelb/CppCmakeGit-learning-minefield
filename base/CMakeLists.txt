cmake_minimum_required(VERSION 4.0)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(base
    LANGUAGES C CXX 
    VERSION 0.0.0.1
    DESCRIPTION "Description"
    HOMEPAGE_URL "https://github.com/VerMishelb/CppCmakeGit-learning-minefield"
)

if (MSVC)
    add_compile_options("/utf-8")
endif()

add_executable(base)
target_include_directories(base PUBLIC src)
target_sources(base src/main.cpp)