//
// Created by shimo on 04/10/2025.
//

#include "Register.h"
std::string get_name() {
    std::string name;
    while (true) {
        std::cout << "Enter your name (max 32 chars): ";
        std::getline(std::cin, name);

        if (isValidAsciiUsername(name)) {
            return name;
        }
        std::cerr << "Invalid name. Must be ASCII printable characters and less than 255 characters.\n";
    }
}
bool isValidAsciiUsername(const std::string& name) {
    if (name.size() >= 255) return false;
    for(unsigned char ch : name) {
        if (ch < 32 || ch > 126) return false; //check is ascii printable
    }
    return true;
}