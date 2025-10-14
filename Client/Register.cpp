//
// Created by shimo on 04/10/2025.
//

#include "Register.h"
#include "utils.h"
Register::Register(const ClientD& client)
    : Header(client, reg, PAYLOAD_LENGTH), name(client.getName()) {
    this->rsaPublic = new RSAPublicWrapper(client.getRSAPublic());
}
std::array<uint8_t, PAYLOAD_LENGTH> Register::to_bytes() const {
    std::array<uint8_t, HEADER_SIZE> header_bytes = Header::to_bytes();
    std::array<uint8_t, PAYLOAD_LENGTH> data{};
    std::copy(header_bytes.begin(), header_bytes.end(), data.begin());
    for (size_t i = 0; i < MAX_NAME_LENGTH; ++i) {
        if (i < name.size()) {
            data[HEADER_SIZE + i] = static_cast<uint8_t>(name[i]);
        } else {
            data[HEADER_SIZE + i] = 0; // Null padding
        }
    }
    for(size_t i = 0; i < KEY_LENGTH; ++i) {
        data[HEADER_SIZE + MAX_NAME_LENGTH + i] = static_cast<uint8_t>(rsaPublic->getPublicKey()[i]);
    }
    return data;

}
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