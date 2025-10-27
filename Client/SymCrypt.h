//
// Created by shimo on 26/10/2025.
//

#ifndef SYMCRYPT_H
#define SYMCRYPT_H


#include <vector>
#include "AESWrapper.h"
#include "RSAWrapper.h"
#include "utils.h"

std::string genKey();
std::vector<uint8_t> toByes(std::string & key);
std::string encryptKey(std::vector<uint8_t> & data , RSAPublicWrapper * wrapper);
std::vector<uint8_t> encryptKey(std::vector<char> & data, RSAPublicWrapper * wrapper);

#endif //SYMCRYPT_H
