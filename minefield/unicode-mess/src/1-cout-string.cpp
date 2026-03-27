#include <iostream>
#include <fstream>
#include <filesystem>
#include <limits>
#include <memory>
#include <string>
#include "common.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef min // ↑ This caused those undefs.
#undef max

#define MIRROR_STDOUT_TO_A_FILE
#define SETLOCALE_UTF8_ENABLED
#define CHANGE_CP_ENABLED

#ifdef MIRROR_STDOUT_TO_A_FILE
    #define allout(arg) \
        std::cout##arg; \
        ofile##arg;
#else
    #define allout(arg) std::cout##arg;
#endif // MIRROR_STDOUT_TO_A_FILE

namespace fs = std::filesystem;
std::ofstream ofile;
std::ifstream ifile{};
// std::streambuf *obuf{};
std::streambuf *ibuf{};
const auto max_stream_size{std::numeric_limits<std::streamsize>::max()};

// typedef BOOL (CALLBACK* CODEPAGE_ENUMPROCA)(LPSTR);
BOOL stupidCallback(LPSTR cp) {
    allout( << cp << ' ');
    return 1;
}

void ListSupportedCodepages() {
    allout( << "Installed codepages:" << std::endl);
    EnumSystemCodePagesA(stupidCallback, CP_INSTALLED);
    allout( << "\nSupported codepages:" << std::endl);
    EnumSystemCodePagesA(stupidCallback, CP_SUPPORTED);
    allout( << "\nIn CP=" << GetConsoleCP() << "\nOut CP=" << GetConsoleOutputCP() <<
    "\nCurrent locale=" << std::setlocale(LC_ALL, nullptr) << std::endl <<
    "ofile.getloc=" << ofile.getloc().name() << std::endl;
    );
}

int PrintUTF8() {
    allout(<< "PrintUTF8():" << std::endl
           << STR_R_UTF8 << std::endl);
    return 0;
}

int PrintArgs(int argc, char **argv) {
    if (argc < 2) { std::cout << "Not enough arguments!"; return -1; }
    allout(<< "argv[0]:" << std::endl
           << argv[0] << std::endl
           << "argv[1] (CP_ACP (e.g. 1251) on drag&drop and when you set args from terminal):" << std::endl
           << argv[1] << std::endl);
    return 0;
}

void InputTest() {
    allout(<< "Input test! Enter your string:" << std::endl);
    char str[1024]{};
    if (std::cin.peek() == '\n') {
        std::cin.ignore(max_stream_size, '\n');
    }
    std::cin >> (str);
    allout(<< "\nEntered string:" << std::endl << (str) << std::endl);
}

void pause() {
    std::cout << "Press Enter to exit...\n";
    if (std::cin.peek() == '\n') {
        std::cin.ignore(max_stream_size, '\n');
    }
    char c{0};
    std::cin.get(c);
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
- The manifest trick is only for 1903+.
- Stupid Windows uses 1251 for any OS interactions on Win 10 22H2 or any other system really, at least with default manifest.
- If manifest sets UTF-8 — ????? (have to check).
- Pre-1903 (hiiii windows 7 👋😊) can only cope with CP_ACP (1251) and W functions, even if manifest declares UTF-8.
    To turn wchar_t crap into UTF-8 use another crap from stringapiset.h, or rather Windows.h
    (Don't forget to #define WIN32_LEAN_AND_MEAN and #undef min max after inclusion):
    
    WideCharToMultiByte(target_codepage,
        DWORD flags (must be NULL),
        wchar_t *from, int from_length,
        char* to, int to_size,
        char default_char (must be NULL), bool use_default_char (must be NULL))
    WideCharToMultiByte(65001, NULL, wchar_t *from, int from_length, char *to, int to_size, 0, 0) to convert.
    WideCharToMultiByte(65001, NULL, wchar_t *from, int from_length, nullptr, 0, 0, 0) to get to_size.

  Technically from_length can be -1 so it's null-terminated, but I'd rather not do this.
  A similar thing exists to turn char (UTF-8) into wchar_t:
    MultiByteToWideChar(65001, 0, str.data(), (int)str.size(), wstrTo.data(), size_needed);
  So we end up with something like this:


- Console uses OEM 866 (CP_OEMCP) for RUNTIME input and output by default, not 1251 (CP_ACP) like the rest of the OS or the exe arguments passed IN THE CONSOLE (WHY?!).
  This can be changed with SetConsoleCP(65001) for stdin and SetConsoleOutputCP(65001) for stdout.

  It is worth noting that these are kept after the app is executed, so I'd save them and return the original values before exiting.
  The supported codepages can be listed using an EnumSystemCodePagesA(stupidCallback, CP_INSTALLED); call, which is in ListSupportedCodepages();
  It also prints the current value.

- std::setlocale(LC_ALL, ".utf-8"); has no effect on encoding pre-Win10 1803 (NOT 1903), but probably does the same thing that the manifest does.
  Logically, this makes the app expect all C-strings to be UTF-8, which is good because we still have no sane way of working with u8string and char8_t...
  For this to have the slightest chance to work the app MUST be linked statically, or the call will fail on anything before Win 10 1803.

*/

std::string Ansi2utf(const std::string &src_ansi) {
    if (src_ansi.empty()) {
        return std::string{};
    }
    
    // .size() does NOT inlcude the null byte, but std::string str[str.size()] is guaranteed to be 0 since C++11.
    // Null-terminated wchar_t* is 00 00 on Windows, 00 00 00 00 everywhere else.

    // Ansi (1251) to WTF
    int tmp_wstr_size {MultiByteToWideChar(CP_ACP, NULL, src_ansi.data(), src_ansi.size()+1, nullptr, NULL)};
    // allout(<< "Ansi2utf: tmp_wstr_size=" << tmp_wstr_size << " for src_ansi=" << src_ansi << std::endl);
    auto tmp_wstr = std::make_unique<wchar_t[]>(tmp_wstr_size);
    MultiByteToWideChar(CP_ACP, NULL, src_ansi.data(), src_ansi.size(), tmp_wstr.get(), tmp_wstr_size);

    // WTF to UTF-8
    int dst_utf_size {WideCharToMultiByte(CP_UTF8, NULL, tmp_wstr.get(), tmp_wstr_size, nullptr, 0, 0, 0)};
    std::string dst_utf{ std::string(static_cast<size_t>(dst_utf_size), '\0')};
    WideCharToMultiByte(CP_UTF8, NULL, tmp_wstr.get(), tmp_wstr_size, dst_utf.data(), dst_utf_size, 0, 0);

    return dst_utf;
}

int main(int argc, char **argv) {
#ifdef SETLOCALE_UTF8_ENABLED
    std::locale::global(std::locale(".utf-8")); // also does std::setlocale(LC_ALL, ".utf-8");
    std::cout << "SETLOCALE_UTF8_ENABLED. Current locale=" << std::setlocale(LC_ALL, nullptr) << std::endl;
    ofile = {};
    // ofile.imbue(std::locale());
    std::cout << "New locale: cout=" << std::cout.getloc().name() << " global=" << std::locale().name() << std::endl;
#endif // SETLOCALE_UTF8_ENABLED
#ifdef CHANGE_CP_ENABLED
    UINT original_cp{GetConsoleCP()};
    UINT original_output_cp{GetConsoleOutputCP()};
    if (!SetConsoleCP(CP_UTF8)) { std::cout << "SetConsoleCP(CP_UTF8) failed: " << GetLastError() << std::endl; }
    if (!SetConsoleOutputCP(CP_UTF8)) { std::cout << "SetConsoleOutputCP(CP_UTF8) failed:" << GetLastError() << std::endl; }
#endif // CHANGE_CP_ENABLED

    fs::path workdir{fs::current_path()};
    std::cout << "Using files: " << (workdir/"1-out(-in).txt") << std::endl;

    // #if FILES_FOR_CIN_COUT == 1
    // ibuf = std::cin.rdbuf(ifile.rdbuf());
    // obuf = std::cout.rdbuf(ofile.rdbuf());
    // #endif

    ofile.open(workdir/"1-out.txt", std::ios_base::trunc);
    if (!ofile) { std::cout << "Could not open/create " << (workdir/"1-out.txt") << std::endl; }
    ifile.open(workdir/"1-in.txt");
    if (!ifile) { std::cout << "Could not open " << (workdir/"1-in.txt") << std::endl; }
    else { ibuf = std::cin.rdbuf(ifile.rdbuf()); }

    
    std::cout << STR_INSTRUCTIONS << std::endl;

    ListSupportedCodepages();    
    PrintUTF8();
    PrintArgs(argc, argv);
    InputTest();
    

    ofile.close();
    pause();

#ifdef CHANGE_CP_ENABLED
    std::cout << "Returning original CP." << std::endl;
    if (!SetConsoleCP(original_cp)) { std::cout << "SetConsoleCP(" << original_cp <<") failed: " << GetLastError() << std::endl; }
    if (!SetConsoleOutputCP(original_output_cp)) { std::cout << "SetConsoleOutputCP(" << original_output_cp << ") failed:" << GetLastError() << std::endl; }
#endif // CHANGE_CP_ENABLED

    return 0;
}