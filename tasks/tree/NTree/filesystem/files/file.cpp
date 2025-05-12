#include "file.hpp"

#include <iostream>

namespace filesystem {

void File::Read(size_t bytes) const {
    std::cout << content_.substr(0, bytes) << std::endl;
}

void File::Write() {
    content_.clear();
    std::string line;
    while (std::getline(std::cin, line)) {
        content_ += line + '\n';
    }
}

void File::Append() {
    std::string line;
    while (std::getline(std::cin, line)) {
        content_ += line + '\n';
    }
}

}  // end namespace filesystem
