#include <iostream>
#include <fstream>
#include <filesystem>
#include <limits>
#include <memory>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef min // ↑ This caused those undefs.
#undef max
#include <print>
#include "common.h"

//////////////////
// User options //
//////////////////

#define MIRROR_STDOUT_TO_A_FILE
// #define SETLOCALE_UTF8_ENABLED
// #define CHANGE_CP_ENABLED
#define SET_STDOUT_VBUF_ENABLED
#define PRINT_UTF8
#define PRINT_1251
#define PRINT_866

// ~User options

#ifdef MIRROR_STDOUT_TO_A_FILE
    #define allout(arg) \
        std::wcout##arg; \
        ofile##arg;
#else
    #define allout(arg) std::wcout##arg;
#endif // MIRROR_STDOUT_TO_A_FILE
#define PRINT_FUNC allout(<< "\n### " << __func__ << " ###\n");
#define CIN_STATE(msg) allout(<< (msg) << std::cin.rdstate() << " (bad, fail, eof)=" << std::cin.badbit << std::cin.failbit << std::cin.eofbit <<  '\n');

#ifdef MANIFEST_ENABLED
#define ACP2U(x) (x)
#else
#define ACP2U(x) (Ansi2utf(x))
#endif // MANIFEST_ENABLED

namespace fs = std::filesystem;
std::wofstream ofile{};
std::wifstream ifile{};
// std::streambuf *obuf{};
std::wstreambuf *ibuf{};
const auto max_stream_size{std::numeric_limits<std::streamsize>::max()};

const char APP_NAME[] =
"basic-"
    #ifdef MANIFEST_ENABLED
    "m"
    #endif
    #ifdef SETLOCALE_UTF8_ENABLED
    "l"
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
    allout( << cp << ' ');
    return 1;
}

BOOL stupidCallbackwwwwwwiiiiidddeee(LPWSTR cp) {
    allout( << cp << ' ');
    return 1;
}

// CODEPAGE_ENUMPROCW stupid;
// BOOL stupid(LPWSTR cp) {
//     allout( << cp << ' ');
//     return 1;
// }

std::string Ansi2utf(const std::string &src_ansi) {
    if (src_ansi.empty()) {
        return std::string{};
    }
    
    // .size() does NOT inlcude the null byte, but std::string str[str.size()] is guaranteed to be 0 since C++11.
    // Null-terminated wchar_t* is 00 00 on Windows, 00 00 00 00 everywhere else.

    // Ansi (1251) to WTF
    int tmp_wstr_size {MultiByteToWideChar(CP_ACP, NULL, src_ansi.data(), src_ansi.size(), nullptr, NULL)};
    // allout(<< "Ansi2utf: tmp_wstr_size=" << tmp_wstr_size << " for src_ansi=" << src_ansi << std::endl);
    auto tmp_wstr = std::make_unique<wchar_t[]>(tmp_wstr_size);
    MultiByteToWideChar(CP_ACP, NULL, src_ansi.data(), src_ansi.size(), tmp_wstr.get(), tmp_wstr_size);

    // WTF to UTF-8
    int dst_utf_size {WideCharToMultiByte(CP_UTF8, NULL, tmp_wstr.get(), tmp_wstr_size, nullptr, 0, 0, 0)};
    std::string dst_utf{ std::string(static_cast<size_t>(dst_utf_size), '\0')};
    WideCharToMultiByte(CP_UTF8, NULL, tmp_wstr.get(), tmp_wstr_size, dst_utf.data(), dst_utf_size, 0, 0);

    return dst_utf;
}

// void bootlegGetline() {

// }

///////////
// Tests //
///////////

void PrintCPInfo(unsigned int cp) {
    CPINFOEXW cp_info{};
    if (!GetCPInfoExW(cp, 0, &cp_info)) {
        allout(<< "GetCPInfoExW() failed: " << GetLastError() << std::endl);
        return;
    };

    allout(
        <<
        "cp_info.CodePage=" <<
        cp_info.CodePage <<
        "\ncp_info.CodePageName=" <<
        cp_info.CodePageName << 
        "\ncp_info.DefaultChar=" <<
        cp_info.DefaultChar <<
        "\ncp_info.LeadByte=" <<
        cp_info.LeadByte <<
        "\ncp_info.MaxCharSize=" <<
        cp_info.MaxCharSize <<
        "\nunicode_default_char=" <<
        cp_info.UnicodeDefaultChar << std::endl;
    );
}

void ListSupportedCodepages() {
    PRINT_FUNC;
    allout( << "Installed codepages:" << std::endl);
    EnumSystemCodePagesW(stupidCallbackwwwwwwiiiiidddeee, CP_INSTALLED);
    allout( << "\nSupported codepages:" << std::endl);
    EnumSystemCodePagesW(stupidCallbackwwwwwwiiiiidddeee, CP_SUPPORTED);

    allout(<< std::endl << "\nCurrent stdin CP:\n");
    PrintCPInfo(GetConsoleCP());
    allout( << "\nCurrent stdout CP:\n");
    PrintCPInfo(GetConsoleOutputCP());
    allout( << "\nThread ACP:\n");
    PrintCPInfo(CP_THREAD_ACP); // CP_THREAD_ACP
    allout( << "\nCP_ACP:\n");
    PrintCPInfo(CP_ACP);
    allout(<< "\nCurrent locale=" << std::setlocale(LC_ALL, nullptr) << std::endl <<
    "ofile.getloc=" << ofile.getloc().name().c_str() << std::endl);
}

int PrintUTF8() {
    PRINT_FUNC;
    allout(<< STR_R_UTF8 << std::endl);
    return 0;
}

int PrintUTF8data() {
    PRINT_FUNC;
    allout(<< data_text_utf8 << std::endl);
    return 0;
}
int PrintCP1251data() {
    PRINT_FUNC;
    allout(<< data_text_1251 << std::endl);
    return 0;
}
int PrintOEM866data() {
    PRINT_FUNC;
    allout(<< data_text_oem << std::endl);
    return 0;
}

int PrintArgs(int argc, char **argv) {
    PRINT_FUNC;
    for (auto i = 0; i < argc; i++) {
        allout(<< "argv[" << i << "]:" << std::endl << argv[i] << std::endl);
    }
    return 0;
}

void InputTest() {
    PRINT_FUNC;
    allout(<< "Input test! Enter your string:" << std::endl);
    std::wstring str{};
    std::getline(std::wcin, str);
    // std::cin >> str;
    // std::cin.ignore(1, '\n');
    // CIN_STATE("cin state after cin: ");
    allout(<< L"\nEntered string:" << std::endl << (str) << std::endl);
}

void pause() {
    PRINT_FUNC;
    std::wcout << "Press Enter to exit...\n";
    std::wcin.ignore(max_stream_size, '\n');
}

/*
Windows 10 (RUS)
argv[1] is received in CP_ACP (1251) when called through the terminal. 
STR_R_UTF8 is indeed UTF8, preserved as is.
chcp has no effect on arguments, apart from display.
866 is default terminal encoding.
1251 is default Windows encoding 

console cp  text            terminal
866         UTF-8           This is a UTF-8 example string: ╨╖╨┤╨╡╤Б╤М ╨╝╨╜╨╛╨│╨╛ ╤Б╨╗╨╛╨╢╨╜╤Л╤Е ╨▒╤Г╨║╨▓!
866         argv (1251)     ЄхёЄю (same for the drag&drop files)
65001       UTF-8           This is a UTF-8 example string: здесь много сложных букв!
65001       argv (1251)     �����



Moral of the story:
- Win 10 1903+ the process code page will always be UTF-8. This is a direct quote from the docs. No idea what it means because it doesn't really help.

- MSVC will fight you unles you pass a /utf-8 flag, that changes "execution charset" and "source charset". I'd say that nowadays this flag is mandatory.
  I suspect that execution charset specifically may be different for Windows XP, but I'm not the one to test.
  The execution character set is the encoding used for the text of your program that is input to the compilation phase after all preprocessing steps.
  This character set is used for the internal representation of any string or character literals in the compiled code.
  
  The source character set is the encoding used to interpret the source text of your program.
  It's converted into the internal representation used as input to the preprocessing phases before compilation.
  The internal representation is then converted to the execution character set to store string and character values in the executable.

- The manifest trick is only for 1903+. And it's annoying to write XMLs.

- Stupid Windows uses 1251 for any OS interactions on Win 10 22H2 or any other system really, at least with default manifest.

- If manifest sets UTF-8 — ????? (have to check). Doesn't do anything pre Win 10 1903.

- Pre-1903 (hiiii windows 7 👋😊) can only cope with CP_ACP (1251) with A and whcar with W functions, even if manifest declares UTF-8.
  All arguments passed to the app are in CP_ACP.
  To turn wchar_t crap into UTF-8 use another crap from stringapiset.h, or rather Windows.h
  (Don't forget to #define WIN32_LEAN_AND_MEAN and #undef min max after inclusion):
    WideCharToMultiByte(target_codepage,
        DWORD flags (must be NULL),
        wchar_t *from, int from_length,
        char* to, int to_size,
        char default_char (must be NULL), bool use_default_char (must be NULL))
    WideCharToMultiByte(65001, NULL, wchar_t *from, int from_length, nullptr, 0, 0, 0) to get to_size.
    WideCharToMultiByte(65001, NULL, wchar_t *from, int from_length, char *to, int to_size, 0, 0) to convert.
  Technically from_length can be -1 so it's null-terminated, but I'd rather not do this.
  A similar thing exists to turn char (UTF-8) into wchar_t:
    MultiByteToWideChar(65001, 0, str.data(), (int)str.size(), wstrTo.data(), size_needed);
  So we end up with something like Ansi2utf() from this file.

- Console uses OEM 866 (CP_OEMCP) for RUNTIME input and output by default, not 1251 (CP_ACP) like the rest of the OS or the exe arguments passed IN THE CONSOLE (WHY?!).
  This can be changed with SetConsoleCP(65001) for stdin and SetConsoleOutputCP(65001) for stdout.

  It is worth noting that these are kept after the app is executed, so I'd save them and return the original values before exiting.
  The supported codepages can be listed using an EnumSystemCodePagesA(stupidCallback, CP_INSTALLED); call, which is in ListSupportedCodepages();
  It also prints the current value.

- std::setlocale(LC_ALL, ".utf-8"); has no effect on encoding pre-Win10 1803 (NOT 1903), but probably does the same thing that the manifest does.
  Logically, this makes the app expect all C-strings to be UTF-8, which is good because we still have no sane way of working with u8string and char8_t...
  For this to have the slightest chance to work the app MUST be linked statically, or the call will fail on anything before Win 10 1803.



About this application:
- For some reason, if both SETLOCALE_UTF8_ENABLED and CHANGE_CP_ENABLED are defined, the output of
  ListSupportedCodepages(), or rather GetConsole[Output]CP() specifically, is broken in the most hilarious way:
  it turns into "In CP=65�001", where the broken symbol is 0xC2.
- Update on the previous one! C2 is a part of C2 A0 sequence, where A0 is... gone.

Windows 10 22H2 19045.6466, Russian: DOS 866, 1251
^utf — output in utf8;
^aсp — output in ACP;
App         out_cp      in_cp       thread_cp (argv)
basic       866         866         1251
basic-s     utf8        1251        1251
basic-s     utf8        utf8*       1251    * — unreliable, only works in Windows Terminal if the sequence is 2+ bytes. Replaces chars with 0 otherwise.
basic-l     866^acp     866         1251
basic-m     866         866         utf8
basic-ml    866^utf     866         utf8    Terminal output dies from .imbue due to U+00A0 as thousands separator.
basic-ms    utf8        1251        utf8
basic-mls   utf8        1251        utf8    Terminal output dies from .imbue due to U+00A0 as thousands separator.

Strats:
Pre-Win 10: out_cp = utf8, in_cp = GetACP()
*/

int main(int argc, char **argv) {
    // std::setlocale(LC_ALL, ".utf8");
    std::wcout << APP_NAME << '\n';
    fs::path workdir{fs::current_path()};
    std::wcout << L"Using files: " << (workdir/L"1w-out(-in).txt") << std::endl;
    
    // #if FILES_FOR_CIN_COUT == 1
    // ibuf = std::cin.rdbuf(ifile.rdbuf());
    // obuf = std::cout.rdbuf(ofile.rdbuf());
    // #endif
    
    ofile.open(workdir/L"1w-out.txt", std::ios_base::trunc | std::ios_base::binary);
    if (!ofile) { std::wcout << L"Could not open/create " << (workdir/L"1w-out.txt") << std::endl; }
    ifile.open(workdir/L"1w-in.txt");
    if (!ifile) { std::wcout << L"Could not open " << (workdir/L"1w-in.txt") << std::endl; }
    else { ibuf = std::wcin.rdbuf(ifile.rdbuf()); }

#ifdef SETLOCALE_UTF8_ENABLED
    allout(<< "SETLOCALE_UTF8_ENABLED defined.\n");
#ifdef SET_STDOUT_VBUF_ENABLED
    setvbuf(stdout, nullptr, _IOFBF, 1000); // Is there a "correct" C++ way of doing this?
    std::wcout << "SET_STDOUT_VBUF_ENABLED. Don't forget to std::flush." << std::endl;
#endif // SET_STDOUT_VBUF_ENABLED

    std::locale::global(std::locale("")); // also does std::setlocale(LC_ALL, ".utf8");
    allout(<< "New global C++ locale: " << std::locale().name().c_str() << std::endl);
    allout(<< "The 1234567890 test before imbue:\n>" << 1234567890 << "<" << std::endl);
    allout(<< "Locale: " << std::locale().name().c_str() << std::endl;);
    allout(.imbue(std::locale()));
    allout(<< "The 1234567890 test after imbue:\n>" << 1234567890 << "<" << std::endl);

    // allout(<< STR_INSTRUCTIONS << std::endl);
    ifile.imbue(std::locale());
    std::wcin.imbue(std::locale());
    allout(<< "Current C locale std::setlocale(LC_ALL, nullptr)=" << std::setlocale(LC_ALL, nullptr) << std::endl);
    allout(<< "New C++ cout locale: std::cout.getloc().name()=" << std::wcout.getloc().name().c_str() << std::endl);
#endif // SETLOCALE_UTF8_ENABLED

#ifdef CHANGE_CP_ENABLED
    allout(<< "CHANGE_CP_ENABLED defined. Changing CP to 65001." << std::endl);
    UINT original_cp{GetConsoleCP()};
    UINT original_output_cp{GetConsoleOutputCP()};
    
    allout(<< "original_cp=" << original_cp << "\noriginal_output_cp=" << original_output_cp << std::endl);
    if (!SetConsoleCP(GetACP())) { allout(<< "SetConsoleCP(GetACP()) failed: " << GetLastError() << std::endl); }
    if (!SetConsoleOutputCP(CP_UTF8)) { allout(<< "SetConsoleOutputCP(CP_UTF8) failed:" << GetLastError() << std::endl); }
#endif // CHANGE_CP_ENABLED

#ifdef MANIFEST_ENABLED
    allout(<< "MANIFEST_ENABLED. Win 10+ will treat argv[] as UTF-8." << std::endl);
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
    std::wcout << "Returning original CP." << std::endl;
    if (!SetConsoleCP(original_cp)) { std::wcout << "SetConsoleCP(" << original_cp <<") failed: " << GetLastError() << std::endl; }
    if (!SetConsoleOutputCP(original_output_cp)) { std::wcout << "SetConsoleOutputCP(" << original_output_cp << ") failed:" << GetLastError() << std::endl; }
#endif // CHANGE_CP_ENABLED

    return 0;
}