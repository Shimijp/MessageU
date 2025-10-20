//
// Created by shimo on 03/10/2025.
//

#include "RWClientD.h"

#include <iomanip>

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

void loadClientFromFile(const std::string& path, ClientD& client) {
    std::ifstream file(path);
    if(!file.is_open()) {
        throw std::runtime_error("Could not open file: " + path);

    }
    std::string line1, line2,line3;
    std::getline(file, line1);
    std::getline(file, line2);
    std::getline(file, line3);
    if(line1.empty() || line2.empty() || line3.empty()) {
        throw std::runtime_error("File format incorrect or missing data in file: " + path);
    }
    client.setClientName(line1);
    if(!checkLegalUUID(line2)) {
        throw std::runtime_error("UUID format incorrect in file: " + path);
    }
    client.setUUID(stringToUUID16(line2));
    client.setRSAPrivateKeyFromBase64(line3);
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
    file << "\n";
    file << "\n" << client.getPrivateKeyBase64() << "\n";
}