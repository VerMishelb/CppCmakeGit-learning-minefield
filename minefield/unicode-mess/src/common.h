#ifndef _common_h_
#define _common_h_

const char* STR_INSTRUCTIONS =
R"(Call as "app.exe <UTF-8 string>". This app does the following:
1. Prints this message.
2. Prints argv[0] and argv[1].
3. Tries to make a text file next to itself.
4. Puts argv[0], argv[1] inside.
5. Closes the file and tries to open it again.
)";

const char* STR_R_UTF8 = R"(This is a UTF-8 example string: здесь много сложных букв!)";

#endif