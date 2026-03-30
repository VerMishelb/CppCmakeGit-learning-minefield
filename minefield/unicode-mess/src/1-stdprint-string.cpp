#include <iostream>
#include <fstream>
#include <filesystem>
#include <limits>
#include <memory>
#include <string>
#include <print>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef min // ↑ This caused those undefs.
#undef max
#include <exception>
// #include <print>
#include <fmt/base.h>
#include <fmt/format.h>
#include <fmt/std.h>
#include <fmt/ranges.h>
#include "common.h"

//////////////////
// User options //
//////////////////

#define MIRROR_STDOUT_TO_A_FILE
// #define SET_C_LOCALE ".utf8"
// #define SET_CPP_LOCALE ".utf8"
// #define CHANGE_CP_ENABLED
// #define MANIFEST_ENABLED // THIS IS DEFINED IN CMAKELISTS!
// #define SET_STDOUT_VBUF_ENABLED
#define PRINT_UTF8
// #define PRINT_1251
// #define PRINT_866

// ~User options

using fmt::print, fmt::format;
// using std::print, std::format;

#ifdef MIRROR_STDOUT_TO_A_FILE
    #define allout(arg) \
        std::cout arg; \
        ofile arg;
#else
    #define allout(arg) std::cout arg;
#endif // MIRROR_STDOUT_TO_A_FILE
#define allprint(format_, ...) print(stdout, format_ __VA_OPT__(,) __VA_ARGS__); ofile << format(format_ __VA_OPT__(,) __VA_ARGS__) << std::flush;
#define PRINT_FUNC allprint("\n### {} ###\n", __func__);
#define CIN_STATE(msg) allout(<< (msg) << std::cin.rdstate() << " (bad, fail, eof)=" << std::cin.badbit << std::cin.failbit << std::cin.eofbit <<  '\n');

#ifdef MANIFEST_ENABLED
#define ACP2U(x) (x)
#else
#define ACP2U(x) (Ansi2utf(x))
#endif // MANIFEST_ENABLED

namespace fs = std::filesystem;
std::ofstream ofile{};
std::ifstream ifile{};
// std::streambuf *obuf{};
std::streambuf *ibuf{};
const auto max_stream_size{std::numeric_limits<std::streamsize>::max()};

const char APP_NAME[] =
"print-"
    #ifdef MANIFEST_ENABLED
    "m"
    #endif
    #ifdef SET_C_LOCALE
    "l(" SET_C_LOCALE
    #endif
    #ifdef SET_CPP_LOCALE
    "+" SET_CPP_LOCALE
    #endif
    #if defined(SET_C_LOCALE) || defined(SET_CPP_LOCALE)
    ")"
    #endif
    #ifdef CHANGE_CP_ENABLED
    "s"
    #endif
    #ifdef SET_STDOUT_VBUF_ENABLED
    "-vbuf"
    #endif
;

// typedef BOOL (CALLBACK* CODEPAGE_ENUMPROCA)(LPSTR);
BOOL stupidCallback(LPSTR cp) {
    allprint("{} ", cp);
    return 1;
}

std::string Ansi2utf(const std::string &src_ansi, UINT src_cp = CP_ACP) {
    if (src_ansi.empty()) {
        return std::string{};
    }
    
    // .size() does NOT inlcude the null byte, but std::string str[str.size()] is guaranteed to be 0 since C++11.
    // Null-terminated wchar_t* is 00 00 on Windows, 00 00 00 00 everywhere else.

    // Ansi (1251) to WTF
    print("Crap catcher Ansi2utf() No{}\n", __COUNTER__);
    int tmp_wstr_size {MultiByteToWideChar(src_cp, NULL, src_ansi.data(), src_ansi.size(), nullptr, NULL)};
    print("Crap catcher Ansi2utf() No{}\n", __COUNTER__);
    // allout(<< "Ansi2utf: tmp_wstr_size=" << tmp_wstr_size << " for src_ansi=" << src_ansi << std::endl);
    auto tmp_wstr = std::make_unique<wchar_t[]>(tmp_wstr_size);
    MultiByteToWideChar(src_cp, NULL, src_ansi.data(), src_ansi.size(), tmp_wstr.get(), tmp_wstr_size);
    print("Crap catcher Ansi2utf() No{}\n", __COUNTER__);
    
    // WTF to UTF-8
    int dst_utf_size {WideCharToMultiByte(CP_UTF8, NULL, tmp_wstr.get(), tmp_wstr_size, nullptr, 0, 0, 0)};
    print("Crap catcher Ansi2utf() No{}\n", __COUNTER__);
    std::string dst_utf{ std::string(static_cast<size_t>(dst_utf_size), '\0')};
    WideCharToMultiByte(CP_UTF8, NULL, tmp_wstr.get(), tmp_wstr_size, dst_utf.data(), dst_utf_size, 0, 0);
    print("Crap catcher Ansi2utf() No{}\n", __COUNTER__);

    return dst_utf;
}

void unfuckCout() {
    if (!std::cout.good()) {
        auto f_bad = (std::cout.rdstate() & std::cout.badbit) != 0;
        auto f_fail = (std::cout.rdstate() & std::cout.failbit) != 0;
        auto f_eof = (std::cout.rdstate() & std::cout.eofbit) != 0;
        std::cout.clear();
        std::cout << "\nstd::cout died :) (bad, fail, eof): " << f_bad << f_fail << f_eof << std::endl;
    }
}

// void bootlegGetline() {

// }

///////////
// Tests //
///////////

void PrintCPInfo(unsigned int cp) {
    print("Crap catcher No{}\n", __COUNTER__);
    CPINFOEXA cp_info{};
    if (!GetCPInfoExA(cp, 0, &cp_info)) {
        allprint("GetCPInfoExA() failed: {}\n", GetLastError());
        return;
    };
    
    char unicode_default_char[4]={};
    mbstate_t mbstate{};
    size_t retval{};
    print("Crap catcher No{}\n", __COUNTER__);
    wcrtomb_s(&retval, unicode_default_char, cp_info.UnicodeDefaultChar, &mbstate);
    print("Crap catcher No{}\n", __COUNTER__);
    
    // WE CRASH HERE BEFORE THE ACP CALL (AND AFTER THE ACP CALL)
    // allprint(
    //     "cp_info.CodePage={}\n"
    //     "cp_info.CodePageName={}\n"
    //     "cp_info.DefaultChar={}\n"
    //     "cp_info.LeadByte={}\n"
    //     "cp_info.MaxCharSize={}\n"
    //     "unicode_default_char={}\n",
    //     cp_info.CodePage,
    //     ACP2U(cp_info.CodePageName) ,
    //     cp_info.DefaultChar, // this prints an array and this is likely wide char but I can't prove it.
    //     cp_info.LeadByte,
    //     cp_info.MaxCharSize,
    //     unicode_default_char
    // );

    allprint("cp_info.CodePage={}\n",cp_info.CodePage);
    allprint("cp_info.CodePageName={}\n",ACP2U(cp_info.CodePageName)); // Dies here specifically
    allprint("cp_info.DefaultChar={}\n",cp_info.DefaultChar); // this prints an array and this is likely wide char but I can't prove it.)
    allprint("cp_info.LeadByte={}\n",cp_info.LeadByte);
    allprint("cp_info.MaxCharSize={}\n",cp_info.MaxCharSize);
    allprint("unicode_default_char={}\n",unicode_default_char);

    print("Crap catcher No{}\n", __COUNTER__);
    
    unfuckCout();
}

void ListSupportedCodepages() {
    PRINT_FUNC;
    allprint("Installed codepages:\n");
    EnumSystemCodePagesA(stupidCallback, CP_INSTALLED);
    allprint("\nSupported codepages:\n");
    EnumSystemCodePagesA(stupidCallback, CP_SUPPORTED);

    allprint("\n\nCurrent stdin CP:\n");
    PrintCPInfo(GetConsoleCP());
    allprint("\nCurrent stdout CP:\n");
    PrintCPInfo(GetConsoleOutputCP());
    allprint("\nThread ACP:\n");
    PrintCPInfo(CP_THREAD_ACP); // CP_THREAD_ACP
    allprint("\nCP_ACP:\n");
    PrintCPInfo(CP_ACP);
    allprint("\nCurrent locale={}\n" "ofile.getloc={}\n",std::setlocale(LC_ALL, nullptr),
        ofile.getloc().name());
    unfuckCout();
}

int PrintUTF8() {
    PRINT_FUNC;
    printf("Printf: %s\n", STR_R_UTF8);
    unfuckCout();
    printf_s("Printf_s: %s\n", STR_R_UTF8);
    unfuckCout();
    allprint("std::print: {}\n", STR_R_UTF8);
    unfuckCout();
    allout(<< "std::cout: " << STR_R_UTF8 << std::endl);
    unfuckCout();
    return 0;
}

int PrintUTF8data() {
    PRINT_FUNC;
    print("{}\n", data_text_utf8);
    allprint("{}\n", data_text_utf8);
    unfuckCout();
    return 0;
}
int PrintCP1251data() {
    PRINT_FUNC;
    allprint("{}\n", data_text_1251);
    unfuckCout();
    return 0;
}
int PrintOEM866data() {
    PRINT_FUNC;
    allprint("{}\n", data_text_oem);
    unfuckCout();
    return 0;
}

int PrintArgs(int argc, char **argv) {
    PRINT_FUNC;
    for (auto i = 0; i < argc; i++) {
    #ifdef MANIFEST_ENABLED
        const std::string arg{argv[i]};
    #else
        const std::string arg{Ansi2utf(argv[i])};
    #endif
        allprint("argv[{}]:\n{}\n", i, arg);
        unfuckCout();
    }
    return 0;
}

void InputTest() {
    PRINT_FUNC;
    allprint("Input test! Enter your string:\n");
    std::string str{};
    std::getline(std::cin, str);
    // std::cin >> str;
    // std::cin.ignore(1, '\n');
    // CIN_STATE("cin state after cin: ");
    allprint("\nEntered string:\n{}\nIn UTF-8:\n{}\n", (str), Ansi2utf(str, GetConsoleCP()));
    unfuckCout();
}

void pause() {
    PRINT_FUNC;
    print("Press Enter to exit...\n");
    std::cin.ignore(max_stream_size, '\n');
}



int main(int argc, char **argv) {
    try {
        HANDLE hcsb = CreateFileA("CONOUT$", GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        CONSOLE_FONT_INFOEX cfi = {sizeof(cfi)};
        cfi.nFont = 0;
        SetCurrentConsoleFontEx(hcsb, FALSE, &cfi);
    print("{}\n", APP_NAME);
    fs::path workdir{fs::current_path()};
    print("Using files: {}\n", (workdir/"1-out(-in).txt").string());
    
    // #if FILES_FOR_CIN_COUT == 1
    // ibuf = std::cin.rdbuf(ifile.rdbuf());
    // obuf = std::cout.rdbuf(ofile.rdbuf());
    // #endif
    
    ofile.open(workdir/"1-print-out.txt", std::ios_base::trunc | std::ios_base::binary);
    if (!ofile) { print("Could not open/create {}\n", (workdir/"1-print-out.txt").string()); }
    ifile.open(workdir/"1-print-in.txt");
    if (!ifile) { print("Could not open {}\n", (workdir/"1-print-in.txt").string()); }
    else { ibuf = std::cin.rdbuf(ifile.rdbuf()); }

#ifdef SET_STDOUT_VBUF_ENABLED
    setvbuf(stdout, nullptr, _IOFBF, 1000); // Is there a "correct" C++ way of doing this?
    std::cout << "SET_STDOUT_VBUF_ENABLED. Don't forget to std::flush." << std::endl;
#endif // SET_STDOUT_VBUF_ENABLED
#ifdef SET_C_LOCALE
    allprint("SET_C_LOCALE defined as {}.\n", SET_C_LOCALE);
    std::setlocale(LC_ALL, SET_C_LOCALE);
#endif // SET_C_LOCALE
#ifdef SET_CPP_LOCALE
    std::locale::global(std::locale(SET_CPP_LOCALE)); // also does std::setlocale(LC_ALL, ".utf8");
    allprint("New global C++ locale: {}\n", std::locale().name());
    allprint("The 1234567890 test before imbue:\n>{}<\n", 1234567890);
    allprint("Locale: {}\n", std::locale().name());
    allout(.imbue(std::locale()));
    allprint("The 1234567890 test after imbue:\n>{}<\n", 1234567890);

    ifile.imbue(std::locale());
    std::cin.imbue(std::locale());
    allprint("New C++ cout locale: std::cout.getloc().name()={}\n", std::cout.getloc().name());
#endif // SETLOCALE_UTF8_ENABLED
    allprint("Current C locale std::setlocale(LC_ALL, nullptr)={}\n", std::setlocale(LC_ALL, nullptr));

#ifdef CHANGE_CP_ENABLED
    allprint("CHANGE_CP_ENABLED defined. Changing CP to 65001.\n");
    UINT original_cp{GetConsoleCP()};
    UINT original_output_cp{GetConsoleOutputCP()};
    
    allprint("original_cp={}\n" "\noriginal_output_cp={}\n", original_cp, original_output_cp);
    if (!SetConsoleCP(GetACP())) { allprint("SetConsoleCP(GetACP()) failed: {}\n", GetLastError()); }
    if (!SetConsoleOutputCP(CP_UTF8)) { allprint("SetConsoleOutputCP(CP_UTF8) failed: {}\n", GetLastError()); }
#endif // CHANGE_CP_ENABLED

#ifdef MANIFEST_ENABLED
    allprint("MANIFEST_ENABLED. Win 10+ will treat argv[] and WinAPI \"A\" functions as UTF-8.\n");
#endif

    ListSupportedCodepages();
    PrintArgs(argc, argv);
    #ifdef PRINT_UTF8
    PrintUTF8();
    PrintUTF8data();
    #endif
    #ifdef PRINT_1251
    PrintCP1251data();
    #endif
    #ifdef PRINT_866
    PrintOEM866data();
    #endif
    InputTest();

    ofile.close();
    pause();

#ifdef CHANGE_CP_ENABLED
    print("Returning original CP.\n");
    if (!SetConsoleCP(original_cp)) { print("SetConsoleCP({}) failed: {}\n", original_cp, GetLastError()); }
    if (!SetConsoleOutputCP(original_output_cp)) { print("SetConsoleOutputCP({}) failed: {}\n", original_output_cp, GetLastError()); }
#endif // CHANGE_CP_ENABLED
            CloseHandle(hcsb);
    } catch (const std::exception &e) {
        std::printf("%s\n", e.what());
        pause();
    }

    return 0;
}