//
// Created by shimo on 04/10/2025.
//

#ifndef REGISTER_H
#define REGISTER_H
#include <string>
#include <iostream>
#include "header.h"
#define MAX_NAME_LENGTH 255
#define KEY_LENGTH  160
#define PAYLOAD_LENGTH  (MAX_NAME_LENGTH + KEY_LENGTH + HEADER_SIZE)
class Register:Header {
private:
    std::string name;
    RSAPublicWrapper * rsaPublic;
public:
    explicit Register(const ClientD& client);
    std::array<uint8_t, PAYLOAD_LENGTH> to_bytes() const;





};
std::string get_name();
bool isValidAsciiUsername(const std::string& name);

#endif //REGISTER_H
