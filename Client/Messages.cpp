//
// Created by shimo on 25/10/2025.
//

#include "Messages.h"

#include <cassert>
#include <iostream>
#include <stdexcept>
ReqSymKey::ReqSymKey(const UUID16 uuid, const UUID16 d_uuid) :Message(uuid, d_uuid, static_cast<uint8_t> (reqSymKey),0 ) {}
std::vector<uint8_t> ReqSymKey::toBytes() const {
    return Message::toBytes();
}
SymKeyMsg::SymKeyMsg(const UUID16 uuid, const UUID16 d_uuid, const std::vector<uint8_t>& enc)
    : Message(uuid, d_uuid, static_cast<uint8_t>(sendSymKey), static_cast<uint32_t>(enc.size())),
      symKeyEnc(enc) {}

std::vector<uint8_t> SymKeyMsg::toBytes() const {
    if (symKeyEnc.size() != SYM_KEY_ENC_LENGTH) {
        throw std::runtime_error("SymKeyMsg: encrypted key must be 128 bytes");
    }
    auto base = Message::toBytes();           // HEADER + DEST(16) + TYPE(1) + SIZE(4)
    base.insert(base.end(), symKeyEnc.begin(), symKeyEnc.end()); // BODY
    return base;
}
TextMsg::TextMsg(const UUID16 uuid, const UUID16 d_uuid, const std::vector<uint8_t>& encryptedText)
    : Message(uuid, d_uuid, sendTextMsg, encryptedText.size()),
      encryptedText(encryptedText) {}
std::vector<uint8_t> TextMsg::toBytes() const {
    if(encryptedText.empty()) {
        throw std::runtime_error("TextMsg: encrypted text cannot be empty");
    }
    if(encryptedText.size() > MAX_PAYLOAD_SIZE) {
        throw std::runtime_error("TextMsg: encrypted text exceeds maximum payload size");
    }
    auto base = Message::toBytes(); // HEADER + DEST(16) + TYPE(1) + SIZE(4)
    base.insert(base.end(), encryptedText.begin(), encryptedText.end()); // BODY
    return base;
}
void TextMsg::getTextFromClient() {
    std::string input;
    std::cout << "Enter the text: ";
    std::getline(std::cin, input);
    encryptedText = std::vector<uint8_t>(input.begin(), input.end());
}
void TextMsg::UpdateSize() {
    this->payload_length += encryptedText.size();
    setSize(encryptedText.size());
}

UUID16 getUUUIDFromName(std::vector<Client> & clients, const std::string & targetName) {
    for(const auto& client : clients) {
        if(client.name == targetName) {
            return client.uuid16;
        }
    }
    throw std::runtime_error("Client with name " + targetName + " not found in client list.");
}
std::string getNameFromUUID(std::vector<Client> &clients, const UUID16 &uuid) {
    for(const auto& client : clients) {
        if(client.uuid16 == uuid) {
            return client.name;
        }
    }
    return "";
}
