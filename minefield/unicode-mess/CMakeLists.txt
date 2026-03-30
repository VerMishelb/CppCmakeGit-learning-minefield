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

# option(USE_MANIFEST "Use manifest to declare UTF-8 active codepage. Only affects Win10 1903+." OFF)
#[[
4.2 added new syntax:
set(CACHE{<variable>} [TYPE <type>] [HELP <helpstring>...] [FORCE] VALUE [<value>...])
set(CACHE{USE_MANIFEST} TYPE BOOL HELP "Use manifest to declare UTF-8 active codepage." VALUE OFF)
Commented out because I realised that there's also an option() command.
]]
option(COMPILE_WIDE_TESTS "Compile wcout versions" OFF)
option(COMPILE_PRINT_TESTS "Compile std::print versions" ON)



set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

#[[
This thing must be set before the target definition for whatever reason, so
no way to put this inside target_compile_options().    :(
^^^ may not be true.
/utf-8 for indicating that source and compiled data uses utf-8.
/Zc:preprocessor forces the new preprocessor that does actually support __VA_OPT__(,)
(see https://learn.microsoft.com/en-us/cpp/preprocessor/preprocessor-experimental-overview?view=msvc-170)
]]
if (MSVC)
    add_compile_options("/utf-8" "/Zc:preprocessor")
endif()

###########
# Targets #
###########

add_library(texts STATIC)
target_sources(texts
PRIVATE
    src/text_1251.c
    src/text_oem.c
    src/text_utf8.c
PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
        src
    FILES
        src/text_1251.h
        src/text_oem.h
        src/text_utf8.h
)

# // #define SETLOCALE_UTF8_ENABLED
# // #define CHANGE_CP_ENABLED
# m - manifest
# l - setlocale
# s - SetConsole[Output]CP
set(C_LOCALE_DEF SET_C_LOCALE=".utf8")

set(TG_NAME basic)
add_executable(${TG_NAME} src/1-cout-string.cpp)
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

set(TG_NAME basic-l)
add_executable(${TG_NAME} src/1-cout-string.cpp)
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE ${C_LOCALE_DEF})

set(TG_NAME basic-s)
add_executable(${TG_NAME} src/1-cout-string.cpp)
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE CHANGE_CP_ENABLED)

set(TG_NAME basic-ls)
add_executable(${TG_NAME} src/1-cout-string.cpp)
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE ${C_LOCALE_DEF} CHANGE_CP_ENABLED)



set(TG_NAME basic-m)
add_executable(${TG_NAME} src/1-cout-string.cpp "src/Test.exe.manifest")
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE MANIFEST_ENABLED)

set(TG_NAME basic-ml)
add_executable(${TG_NAME} src/1-cout-string.cpp src/Test.exe.manifest)
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE MANIFEST_ENABLED ${C_LOCALE_DEF})

set(TG_NAME basic-ms)
add_executable(${TG_NAME} src/1-cout-string.cpp src/Test.exe.manifest)
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE MANIFEST_ENABLED CHANGE_CP_ENABLED)

set(TG_NAME basic-mls)
add_executable(${TG_NAME} src/1-cout-string.cpp src/Test.exe.manifest)
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE MANIFEST_ENABLED ${C_LOCALE_DEF} CHANGE_CP_ENABLED)

# Print

if(${COMPILE_PRINT_TESTS})

set(TG_NAME print)
set(TG_SRC src/1-stdprint-string.cpp)
add_executable(${TG_NAME} ${TG_SRC})
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

set(TG_NAME print-l)
add_executable(${TG_NAME} ${TG_SRC})
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE ${C_LOCALE_DEF})

set(TG_NAME print-s)
add_executable(${TG_NAME} ${TG_SRC})
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE CHANGE_CP_ENABLED)

set(TG_NAME print-ls)
add_executable(${TG_NAME} ${TG_SRC})
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE ${C_LOCALE_DEF} CHANGE_CP_ENABLED)



set(TG_NAME print-m)
add_executable(${TG_NAME} ${TG_SRC} "src/Test.exe.manifest")
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE MANIFEST_ENABLED)

set(TG_NAME print-ml)
add_executable(${TG_NAME} ${TG_SRC} src/Test.exe.manifest)
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE MANIFEST_ENABLED ${C_LOCALE_DEF})

set(TG_NAME print-ms)
add_executable(${TG_NAME} ${TG_SRC} src/Test.exe.manifest)
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE MANIFEST_ENABLED CHANGE_CP_ENABLED)

set(TG_NAME print-mls)
add_executable(${TG_NAME} ${TG_SRC} src/Test.exe.manifest)
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE MANIFEST_ENABLED ${C_LOCALE_DEF} CHANGE_CP_ENABLED)

endif() # COMPILE_PRINT_TESTS

# Wide versions:

if(${COMPILE_WIDE_TESTS})

set(TG_NAME wminimal)
add_executable(${TG_NAME} src/1-wcout-minimal.cpp)
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

# Never in its existence C++ have seen such crap as the thing that is compiled after this comment line.

set(TG_NAME wbasic)
add_executable(${TG_NAME} src/1-wcout-string.cpp)
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

set(TG_NAME wbasic-l)
add_executable(${TG_NAME} src/1-wcout-string.cpp)
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE ${C_LOCALE_DEF})

set(TG_NAME wbasic-s)
add_executable(${TG_NAME} src/1-wcout-string.cpp)
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE CHANGE_CP_ENABLED)

set(TG_NAME wbasic-ls)
add_executable(${TG_NAME} src/1-wcout-string.cpp)
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE ${C_LOCALE_DEF} CHANGE_CP_ENABLED)



set(TG_NAME wbasic-m)
add_executable(${TG_NAME} src/1-wcout-string.cpp "src/Test.exe.manifest")
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE MANIFEST_ENABLED)

set(TG_NAME wbasic-ml)
add_executable(${TG_NAME} src/1-wcout-string.cpp src/Test.exe.manifest)
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE MANIFEST_ENABLED ${C_LOCALE_DEF})

set(TG_NAME wbasic-ms)
add_executable(${TG_NAME} src/1-wcout-string.cpp src/Test.exe.manifest)
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE MANIFEST_ENABLED CHANGE_CP_ENABLED)

set(TG_NAME wbasic-mls)
add_executable(${TG_NAME} src/1-wcout-string.cpp src/Test.exe.manifest)
target_link_libraries(${TG_NAME} PRIVATE texts)
target_include_directories(${TG_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)
target_compile_definitions(${TG_NAME} PRIVATE MANIFEST_ENABLED ${C_LOCALE_DEF} CHANGE_CP_ENABLED)

endif() # COMPILE_WIDE_TESTS