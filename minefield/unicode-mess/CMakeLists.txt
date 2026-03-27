cmake_minimum_required(VERSION 4.2)

project(unicode-mess
    LANGUAGES C CXX 
    VERSION 0.0.0.1
    DESCRIPTION "Does some basic operations related to arg/file/console IO with UTF-8."
    HOMEPAGE_URL "https://github.com/VerMishelb/CppCmakeGit-learning-minefield"
)

########################
# Configurable options #
########################

option(USE_MANIFEST "Use manifest to declare UTF-8 active codepage. Only affects Win10 1903+." OFF)
#[[
4.2 added new syntax:
set(CACHE{<variable>} [TYPE <type>] [HELP <helpstring>...] [FORCE] VALUE [<value>...])
set(CACHE{USE_MANIFEST} TYPE BOOL HELP "Use manifest to declare UTF-8 active codepage." VALUE OFF)
Commented out because I realised that there's also an option() command.
]]



set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

#[[
This thing must be set before the target definition for whatever reason, so
no way to put this inside target_compile_options().    :(
]]
if (MSVC)
    add_compile_options("/utf-8")
endif()

###########
# Targets #
###########

add_executable(basic
    src/1-cout-string.cpp
    $<$<BOOL:${USE_MANIFEST}>:"src/Test.exe.manifest">
)
target_include_directories(basic PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)