#include <iostream>
// #include <fstream>
// #include <filesystem>
#include <limits>
// #include <memory>
#include <string>
// #define WIN32_LEAN_AND_MEAN
// #include <Windows.h>
// #undef min // ↑ This caused those undefs.
// #undef max
// #include <print>

#include <fcntl.h>
#include <cstdio>
#include <io.h>

#include "common.h"

#define PRINT_FUNC std::wcout << "\n### " << __func__ << " ###\n";


const wchar_t APP_NAME[] = L"wmiminal";

void pause() {
    PRINT_FUNC;
    std::wcout << "Press Enter to exit...\n";
    std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main(int argc, char **argv) {
    // _setmode(_fileno(stdout), _O_U16TEXT);
    std::wcout << APP_NAME << L'\n';
    std::wcout <<
    L"This is a UTF-16 test:\n"
    L"U+168 U~ = <Ũ> as symbol and <\u0168> as \\u0168\n"
    "U+1F5FF moyai = <\U0001f5ff>\n"
    "Cyrillic texts = <падпараджа>\n"
    "End."
    << std::endl;
    if (!std::wcout.good()) {
        auto f_bad = std::wcout.rdstate() & std::wcout.badbit;
        auto f_fail = std::wcout.rdstate() & std::wcout.failbit;
        auto f_eof = std::wcout.rdstate() & std::wcout.eofbit;
        std::wcout.clear();
        std::wcout << L"\n\n\nStream flags (bad, fail, eof): " << f_bad << f_fail << f_eof << std::endl;
    }

    pause();

    return 0;
}