//
// Created by shimo on 14/10/2025.
//
#include "Response.h"

#include <algorithm>
#include <stdexcept>
#include <cstring> // for memchr
#include <ios>
#include <iostream>

#include "Base64Wrapper.h"

// ---------------- Client ctor (from utils.h declaration) ----------------
Client::Client(std::array<char, UUID_LENGTH + MAX_NAME_LENGTH>& data) {
    // Copy UUID (16 bytes)
    for (size_t i = 0; i < UUID_LENGTH; ++i) {
        uuid16.bytes[i] = static_cast<uint8_t>(data[i]);
    }
    // Safe name extraction: read up to NUL within MAX_NAME_LENGTH window
    const char* nameStart = &data[UUID_LENGTH];
    const void* nulPos = std::memchr(nameStart, '\0', MAX_NAME_LENGTH);
    size_t nameLen = nulPos ? (static_cast<const char*>(nulPos) - nameStart)
                            : (MAX_NAME_LENGTH ? MAX_NAME_LENGTH - 1 : 0);
    name.assign(nameStart, nameLen);
}

// ---------------- Response namespace implementations ----------------
namespace Response {

    // ---- Header ----
    Header::Header(const std::vector<char>& data) {
        if (data.size() < HEADER_RESPONSE_SIZE) {
            throw std::runtime_error("short header");
        }
        const auto* b = reinterpret_cast<const uint8_t*>(data.data());

        version = b[0];

        // little-endian 16-bit
        code = static_cast<uint16_t>(static_cast<uint16_t>(b[1]) |
                                     (static_cast<uint16_t>(b[2]) << 8));

        // little-endian 32-bit
        size = static_cast<uint32_t>(b[3]) |
               (static_cast<uint32_t>(b[4]) << 8) |
               (static_cast<uint32_t>(b[5]) << 16) |
               (static_cast<uint32_t>(b[6]) << 24);


        if (version != 2) {
            throw std::runtime_error("unsupported response version");
        }
    }

    bool Header::checkSize() const {
        return size < MAX_PAYLOAD_SIZE;
    }

    uint8_t Header::getVersion() const { return version; }
    uint16_t Header::getCode() const { return code; }
    uint32_t Header::getSize() const { return size; }

    // ---- Base Response ----
    bool Response::checkSuccess() const {
        return true;
    }

    // ---- Register ----
    Register::Register(const std::vector<char>& data) {
        if (data.size() < UUID_LENGTH) {
            throw std::runtime_error("short register payload");
        }
        for (size_t i = 0; i < UUID_LENGTH; ++i) {
            uuid.bytes[i] = static_cast<uint8_t>(data[i]);
        }
    }

    UUID16 Register::getUUID() const { return uuid; }

    bool Register::checkSuccess() const { return true; }
    ResponseCode Register::getResponseCode() const {
        return regSucc;
    }

    // ---- ClientLst ----
    ClientLst::ClientLst(std::vector<char>& data) {
        const size_t entrySize = UUID_LENGTH + MAX_NAME_LENGTH;
        if (entrySize == 0) {
            numClients = 0;
            return;
        }
        if (data.size() % entrySize != 0) {
            throw std::runtime_error("invalid clients list payload size");
        }
        numClients = static_cast<uint16_t>(data.size() / entrySize);
        lst.reserve(numClients);

        for (size_t i = 0; i < data.size();) {
            if (i + entrySize > data.size()) {
                throw std::runtime_error("short buffer for client entry");
            }
            std::array<char, UUID_LENGTH + MAX_NAME_LENGTH> entry{};
            std::copy_n(data.begin() + static_cast<long>(i),
                        entrySize,
                        entry.begin());

            Client entryClient(entry);
            lst.emplace_back(entryClient);  // <-- דחיפה של Client בפועל

            i += entrySize;
        }
    }

    uint16_t ClientLst::getNumClients() const { return numClients; }

    bool ClientLst::checkSuccess() const { return true; }
    ResponseCode ClientLst::getResponseCode() const {
        return userListSucc;
    }
    // ---- PubKey ----
    PubKey::PubKey(const std::vector<char>& data)
        : rsaPub(nullptr)
    {
        const size_t need = KEY_LENGTH + UUID_LENGTH;
        if (data.size() < need) {
            throw std::runtime_error("short public key payload");
        }

        // First 160 bytes = raw public key blob (exactly KEY_LENGTH)
        std::string decoded;
        decoded.assign(data.begin(), data.begin() + KEY_LENGTH);

        rsaPub = new RSAPublicWrapper(decoded);

        // Next 16 bytes = UUID (offset KEY_LENGTH)
        for (size_t i = 0; i < UUID_LENGTH; ++i) {
            uuid.bytes[i] = static_cast<uint8_t>(data[KEY_LENGTH + i]);
        }
    }
    ResponseCode PubKey::getResponseCode() const {
        return getPubKeySucc;
    }

    PubKey::~PubKey() {
        delete rsaPub;
        rsaPub = nullptr;
    }

    RSAPublicWrapper* PubKey::getPubKey() const {
        return rsaPub;
    }

    bool PubKey::checkSuccess() const {
        return rsaPub != nullptr;
    }

} // namespace Response
