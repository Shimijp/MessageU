//
// Created by shimo on 05/10/2025.
//

#ifndef HEADER_H
#define HEADER_H

#include "ClientD.h"

struct Header {
    UUID16 client_id;
    uint8_t version;
    uint16_t msg_type;
    uint32_t payload_length;
    Header(const UUID16 uuid, const uint16_t msg_type, const uint32_t payload_length)
        : client_id(uuid), version(1),
          msg_type(msg_type), payload_length(payload_length) {}
    Header(const ClientD& client, const uint16_t msg_type, const uint32_t payload_length)
        : client_id(client.getUUID()), version(1),
          msg_type(msg_type), payload_length(payload_length) {}

    std::array<uint8_t, HEADER_SIZE> toBytes() const;

};
class Register:Header {
private:
    std::string name;
    RSAPublicWrapper * rsaPublic;
public:
    explicit Register(const ClientD& client);
    std::array<uint8_t, REGISTER_LENGTH> toBytes() const;

};
class ClientList:Header
{
public:
    explicit ClientList(UUID16 uuid);
    std::array<uint8_t , HEADER_SIZE> toBytes() const;
};
class ReqPubKey:Header {
    UUID16 targetUuid;
public:
    ReqPubKey(const UUID16 uuid, UUID16 targetUuid);
    UUID16 getTargetUuid() const;
    std::array<uint8_t, HEADER_SIZE + UUID_LENGTH> toBytes() const;
};

std::string get_name();
bool isValidAsciiUsername(const std::string& name);

#endif //HEADER_H
