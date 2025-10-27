//
// Created by shimo on 05/10/2025.
//

#ifndef HEADER_H
#define HEADER_H

#include "ClientD.h"
#include <vector>

// Request.h - Defines request message structures for MessageU protocol
// Includes header, registration, client list, and public key request classes

struct Header {
    UUID16 client_id; // Sender's UUID
    uint8_t version;  // Protocol version
    uint16_t msg_type; // Type of message/request
    uint32_t payload_length; // Length of payload
    Header(const UUID16 uuid, const uint16_t msg_type, const uint32_t payload_length)
        : client_id(uuid), version(1),
          msg_type(msg_type), payload_length(payload_length) {}
    Header(const ClientD& client, const uint16_t msg_type, const uint32_t payload_length)
        : client_id(client.getUUID()), version(1),
          msg_type(msg_type), payload_length(payload_length) {}

    virtual ~Header() = default; // Virtual destructor for proper inheritance
    virtual std::vector<uint8_t> toBytes() const; // Virtual serialize function returning vector

};
// Registration request: contains client info and public key
class Register final : public Header {
private:
    std::string name; // Client name
    RSAPublicWrapper * rsaPublic; // RSA public key wrapper
public:
    explicit Register(const ClientD& client); // Construct from client
    std::vector<uint8_t> toBytes() const override; // Serialize to bytes - returns REGISTER_LENGTH size vector
};
// Request for client list
class ClientList final :public Header {
public:
    explicit ClientList(UUID16 uuid); // Construct with UUID
    std::vector<uint8_t> toBytes() const override; // Serialize to bytes - returns HEADER_SIZE size vector
};
// Request for another client's public key
class ReqPubKey final : public Header {
    UUID16 targetUuid; // Target client UUID
public:
    ReqPubKey( UUID16 uuid, UUID16 targetUuid); // Construct with source and target UUIDs
    UUID16 getTargetUuid() const; // Get target UUID
    std::vector<uint8_t> toBytes() const override; // Serialize to bytes - returns HEADER_SIZE + UUID_LENGTH size vector
};

class Message : public Header {
    UUID16 d_uuid;
    uint8_t type;
    uint32_t size;
public:
    Message(UUID16 uuid, UUID16 d_uuid, uint8_t type, uint32_t size);
    UUID16 getUUID() const;
    uint8_t getType() const;
    uint32_t getSize() const;
    void setUUID(UUID16 uuid) ;
    void setSize(uint32_t size);
    std::vector<uint8_t> toBytes() const override;
};

class ReqMsg final : public Header {
public:
    explicit ReqMsg(UUID16 uuid);
    std::vector<uint8_t> toBytes() const override;
};
std::string get_name(); // Prompt and validate client name
bool isValidAsciiUsername(const std::string& name); // Validate ASCII username

#endif //HEADER_H
