#include <iostream>
#include <fstream>
#include <filesystem>
#include "common.h"

namespace fs = std::filesystem;
std::ofstream ofile{};


int PrintUTF8() {
    std::cout << "PrintUTF8():" << std::endl
              << STR_R_UTF8 << std::endl;
    ofile << "PrintUTF8():" << std::endl
          << STR_R_UTF8 << std::endl;
    return 0;
}

int PrintArgs(int &argc, char **&argv) {
    if (argc < 2) { std::cout << "Not enough arguments!"; return -1; }    

    std::cout << "argv[0]:" << std::endl
              << argv[0] << std::endl
              << "argv[1]:" << std::endl
              << argv[1] << std::endl;
    ofile << "argv[0]:" << std::endl
          << argv[0] << std::endl
          << "argv[1]:" << std::endl
          << argv[1] << std::endl;
    return 0;
}

int main(int argc, char **argv) {
    /*
    argv[1] is received in 1251 when called through the terminal with chcp 65001. 
    STR_R_UTF8 is indeed UTF8, preserved as is.
    */
    std::cout << STR_INSTRUCTIONS << std::endl;

    fs::path workdir{fs::current_path()};
    std::cout << "Using file: " << (workdir/"testfile_1.txt").string() << std::endl;
    ofile.open(workdir/"testfile_1.txt", std::ios_base::trunc);
    
    PrintUTF8();
    PrintArgs(argc, argv);

    ofile.close();


    return 0;
}