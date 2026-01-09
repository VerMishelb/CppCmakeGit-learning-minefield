cmake_minimum_required(VERSION 4.0)

# Static linking of vcruntime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Put libs together with executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(first-project LANGUAGES CXX
    VERSION 0.0.0.1
    DESCRIPTION "Our first project."
    HOMEPAGE_URL "https://github.com/VerMishelb/CppCmakeGit-learning-minefield/tree/master/tutorial-from-neocities/first-project"
)
add_compile_options(
    "$<$<C_COMPILER_ID:MSVC>:/utf-8>"
    "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
)

# TARGETS
# first-program
add_executable(first-program src/main.cpp)
target_include_directories(first-program PUBLIC src)