cmake_minimum_required(VERSION 4.1)

project(unicode-mess
    LANGUAGES C CXX 
    VERSION 0.0.0.1
    DESCRIPTION "Does some basic operations related to arg/file/console IO with UTF-8."
    HOMEPAGE_URL "https://github.com/VerMishelb/CppCmakeGit-learning-minefield"
)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if (MSVC)
    add_compile_options("/utf-8")
endif()


###########
# Targets #
###########

add_executable(basic src/1-cout-string.cpp)
target_include_directories(basic PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)