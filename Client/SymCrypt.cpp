//
// Created by shimo on 26/10/2025.
//
#include "SymCrypt.h"

#include <stdexcept>

std::string genKey() {
    unsigned char key[AESWrapper::DEFAULT_KEYLENGTH];
    AESWrapper::GenerateKey(key, AESWrapper::DEFAULT_KEYLENGTH);
    return {reinterpret_cast<char*>(key), AESWrapper::DEFAULT_KEYLENGTH};
}
std::string encryptKey(const std::string & key, RSAPublicWrapper *wrapper) {
    try {
        std::string enc  = wrapper->encrypt(key);
        return enc;
    }
    catch (...) {
        throw std::runtime_error("Failed to encrypt symmetric key with RSA public key.");
    }

}
std::string decryptKey(const std::string & key, RSAPrivateWrapper * wrapper) {
    std:: string dec = wrapper->decrypt(key);
    return dec;
}
std::vector<uint8_t> toBytes(std::string & key) {
    std::vector<uint8_t> bytes(key.begin(), key.end());
    return bytes;
}
std::string encryptMsg(const std::string& plain, const std::string& key) {
    AESWrapper aes(reinterpret_cast<const unsigned char*>(key.data()), AESWrapper::DEFAULT_KEYLENGTH);
    std::string cipher = aes.encrypt(plain.c_str(), plain.size());
    return cipher;
}

std::string decryptMsg(const std::string& cipher, const std::string& key) {
    AESWrapper aes(reinterpret_cast<const unsigned char*>(key.data()), AESWrapper::DEFAULT_KEYLENGTH);
    std::string dec = aes.decrypt(cipher.c_str(), cipher.size());
    return dec;
}

