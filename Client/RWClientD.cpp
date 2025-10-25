//
// Created by shimo on 03/10/2025.
//

#include "RWClientD.h"

#include <iomanip>
#include <iostream>

bool checkFileExists(const std::string &path) {
    std::fstream file(path);
    if(file.is_open()) {
        file.close();
        return true;
    }
    return false;
}
bool checkLegalUUID(const std::string &uuid) {
    return uuid.length() == 32; // Simple check for length, more checks can be added
}
UUID16 stringToUUID16(const std::string& uuidStr) {
    UUID16 uuid{};
    for(size_t i = 0; i < 16; ++i) {
        std::string byteStr = uuidStr.substr(i * 2, 2);
        uuid.bytes[i] = static_cast<std::uint8_t>(std::stoul(byteStr, nullptr, 16));
    }
    return uuid;
}

void loadClientFromFile(const std::string& path, ClientD * client_d) {
    if(client_d == nullptr) {

            std::cout << "client_d is null pointer\n";
            return;
        }
        std::ifstream file(path);
        if(!file.is_open()) {
            throw std::runtime_error("Could not open file: " + path);

        }
    std::string name, uuid,base64_key;
    std::getline(file, name);
    std::getline(file, uuid);
    std::ostringstream oss;
    oss << file.rdbuf();
    base64_key = oss.str();
    if(name.empty() || uuid.empty() || base64_key.empty()) {
        throw std::runtime_error("File format incorrect or missing data in file: " + path);
    }
    client_d->setClientName(name);
    if(!checkLegalUUID(uuid)) {
        throw std::runtime_error("UUID format incorrect in file: " + path);
    }
    client_d->setUUID(stringToUUID16(uuid));
    client_d->setRSAPrivateKeyFromBase64(base64_key);
}
void saveClientToFile(const std::string& path, const ClientD& client) {
    std::ofstream file(path);
    if(!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + path);

    }

    file << client.getName() << "\n";
    for (const auto& byte : client.getUUID().bytes) {
        file << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    file << "\n" << client.getPrivateKeyBase64() << "\n";
}