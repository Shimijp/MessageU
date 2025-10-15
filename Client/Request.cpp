//
// Created by shimo on 05/10/2025.
//

#include "Request.h"

#include <iostream>


std::array<uint8_t, HEADER_SIZE> Header::toBytes() const {
    std::array<uint8_t, HEADER_SIZE> data{};
    size_t i = 0;
    for (const auto& byte : client_id.bytes) {
        data[i++] = byte;
    }
    data[i++] = version;
    data[i++] = (msg_type ) & 0xFF;
    data[i++] = (msg_type >> 8) & 0xFF;
    data[i++] = payload_length & 0xFF;
    data[i++] = (payload_length >> 8) & 0xFF;
    data[i++] = (payload_length >> 16) & 0xFF;
    data[i++] = (payload_length >> 24) & 0xFF;

    return data;
}
Register::Register(const ClientD& client)
    : Header(client, reg, REGISTER_LENGTH), name(client.getName()) {
    this->rsaPublic = new RSAPublicWrapper(client.getRSAPublic());
}
std::array<uint8_t, REGISTER_LENGTH> Register::toBytes() const {
    std::array<uint8_t, HEADER_SIZE> header_bytes = Header::toBytes();
    std::array<uint8_t, REGISTER_LENGTH> data{};
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
ClientList::ClientList(const UUID16 uuid) : Header( uuid, userList, 0) {}
std::array<uint8_t, HEADER_SIZE> ClientList::toBytes() const {
    return Header::toBytes();
}
ReqPubKey::ReqPubKey(const UUID16 uuid, const UUID16 targetUuid)
    : Header(uuid, getPubKey, UUID_LENGTH), targetUuid(targetUuid) {}
UUID16 ReqPubKey::getTargetUuid() const {
    return targetUuid;
}
std::array<uint8_t, HEADER_SIZE + UUID_LENGTH> ReqPubKey::toBytes() const {
    std::array<uint8_t, HEADER_SIZE> header_bytes = Header::toBytes();
    std::array<uint8_t, HEADER_SIZE + UUID_LENGTH> data{};
    std::copy(header_bytes.begin(), header_bytes.end(), data.begin());
    for (size_t i = 0; i < UUID_LENGTH; ++i) {
        data[HEADER_SIZE + i] = targetUuid.bytes[i];
    }
    return data;
}
