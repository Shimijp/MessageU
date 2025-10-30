//
// Created by shimo on 05/10/2025.
//

#include "Request.h"

#include <iostream>


std::vector<uint8_t> Header::toBytes() const {
    std::vector<uint8_t> data(HEADER_SIZE);
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
    : Header(client, reg, MAX_NAME_LENGTH + KEY_LENGTH), name(client.getName()) {
    this->rsaPublic = new RSAPublicWrapper(client.getRSAPublic());
}
std::vector<uint8_t> Register::toBytes() const {
    std::vector<uint8_t> header_bytes = Header::toBytes();
    std::vector<uint8_t> data(REGISTER_LENGTH);

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
        std::cout << "Enter your name (max 255 chars): ";
        std::getline(std::cin >> std::ws, name);

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
std::vector<uint8_t> ClientList::toBytes() const {
    return Header::toBytes();
}
ReqPubKey::ReqPubKey(const UUID16 uuid, const UUID16 targetUuid)
    : Header(uuid, getPubKey, UUID_LENGTH), targetUuid(targetUuid) {}
UUID16 ReqPubKey::getTargetUuid() const {
    return targetUuid;
}
std::vector<uint8_t> ReqPubKey::toBytes() const {
    std::vector<uint8_t> header_bytes = Header::toBytes();
    std::vector<uint8_t> data(HEADER_SIZE + UUID_LENGTH);

    std::copy(header_bytes.begin(), header_bytes.end(), data.begin());
    for (size_t i = 0; i < UUID_LENGTH; ++i) {
        data[HEADER_SIZE + i] = targetUuid.bytes[i];
    }
    return data;
}
Message::Message(const UUID16 uuid, const UUID16 d_uuid, const uint8_t type, const uint32_t size) :
Header(uuid, sendMsg, MSG_HEADER_SIZE+size), d_uuid(d_uuid) , type(type),size(size){}
uint32_t Message::getSize() const {
    return size;
}
uint8_t Message::getType() const {
    return type;
}
UUID16 Message::getUUID() const {
    return d_uuid;
}
void Message::setUUID(const UUID16 uuid)  {
    this->d_uuid = uuid;
}
std::vector<uint8_t> Message::toBytes() const {
    std::vector<uint8_t> headerBytes = Header::toBytes();
    std::vector<uint8_t> data(HEADER_SIZE+MSG_HEADER_SIZE);
    std::copy(headerBytes.begin(), headerBytes.end(), data.begin());

    size_t i = HEADER_SIZE;

    // 1) Dest UUID (16B)
    for (const auto& byte : d_uuid.bytes) {
        data[i++] = byte;
    }

    // 2) MessageType (1B)
    data[i++] = type;

    // 3) ContentSize (4B, LE)
    data[i++] = static_cast<uint8_t>(size & 0xFF);
    data[i++] = static_cast<uint8_t>((size >> 8) & 0xFF);
    data[i++] = static_cast<uint8_t>((size >> 16) & 0xFF);
    data[i++] = static_cast<uint8_t>((size >> 24) & 0xFF);

    return data;
}
void Message::setSize(const uint32_t size) {
    this->size = size;
}


ReqMsg::ReqMsg(const UUID16 uuid) :Header(uuid, getMsg, 0){};
std::vector<uint8_t> ReqMsg::toBytes() const {
    return Header::toBytes();
}



