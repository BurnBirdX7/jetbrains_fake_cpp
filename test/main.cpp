
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

const auto FILENAME = "file.txt";

int main() {
    std::fstream fin(FILENAME);
    std::string str;

    for (size_t i = 1; std::getline(fin, str); ++i) {
        std::cout << std::setw(3) << i << "|" << str << '\n';
    }

    return 0;
}

