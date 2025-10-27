//
// Created by shimo on 25/10/2025.
//

#ifndef MESSAGES_H
#define MESSAGES_H

#include "Request.h"
#include <algorithm>
class ReqSymKey final :Message {
public:
    explicit ReqSymKey(UUID16 uuid, UUID16 d_uuid);
    std::vector<uint8_t> toBytes() const override;
};
class SymKeyMsg final : public Message {
    std::vector<uint8_t> symKeyEnc;
public:
    SymKeyMsg(UUID16 uuid,  UUID16 d_uuid, const std::vector<uint8_t>& symKeyEnc);
    std::vector<uint8_t> toBytes() const override;
};
class TextMsg final : public Message {
    std::vector<uint8_t> encryptedText;;
public:
    TextMsg(UUID16 uuid, UUID16 d_uuid, const std::vector<uint8_t>& encryptedText);
    std::vector<uint8_t> toBytes() const override;
    std::string getEncryptedTextAsString() const;
    void getTextFromClient();
    void UpdateSize();
};

UUID16 getUUUIDFromName(std::vector<Client> & clients, const std::string & targetName);
std::string getNameFromUUID(std::vector<Client> & clients, const UUID16 & uuid);
#endif //MESSAGES_H
