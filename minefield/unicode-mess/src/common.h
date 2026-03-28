#pragma once

// This is guaranteed ANSI
const char* STR_INSTRUCTIONS =
R"(
Call as "app.exe <any string>". This app does the following:

#ifdef CHANGE_CP_ENABLED
    SetConsoleCP(CP_UTF8)
    SetConsoleOutputCP(CP_UTF8)
ofile.open(workdir/"1-out.txt", std::ios_base::trunc);
ifile.open(workdir/"1-in.txt");
if ifile -> std::cin.rdbuf(ifile.rdbuf());

STR_INSTRUCTIONS // Prints this message

ListSupportedCodepages();    
PrintUTF8();
PrintUTF8data();
PrintCP1251data();
PrintOEM866data();
PrintArgs(argc, argv);
InputTest();

ofile.close();
pause();
#ifdef CHANGE_CP_ENABLED
    "Returning original CP."
    SetConsoleCP(original_cp)
    SetConsoleOutputCP(original_output_cp)
)";

const char* STR_R_UTF8 = R"(UTF-8 string: tệp thử nghiệm Проверка джокера (хыхы буква ё) ¶§°ðѦѢѣѬѮ֍₽①⓭┗╉╢╳)";
const char* STR_R_UTF8_FILENAME = R"(ѦѢѣѬѮ.txt)";
#include "text_1251.h"
#include "text_oem.h"
#include "text_utf8.h"
