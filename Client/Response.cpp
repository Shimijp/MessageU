    //
    // Created by shimo on 14/10/2025.
    //

    #include "Response.h"

#include <algorithm>
#include <stdexcept>

Client::Client(std::array<char, UUID_LENGTH + MAX_NAME_LENGTH> &data) {
        for (size_t i = 0; i < UUID_LENGTH; ++i) {
            uuid16.bytes[i] = static_cast<uint8_t>(data[i]);
        }
        name = std::string(&data[UUID_LENGTH]);
    }
    Header::Header(const std::array<char, HEADER_RESPONSE_SIZE> &data) {
        version = static_cast<uint8_t>(data[0]);
        code = (static_cast<uint8_t>(data[1]) << 8) | static_cast<uint8_t>(data[2]);
        size = (static_cast<uint8_t>(data[3] | (static_cast<uint8_t>(data[4]) << 8) |
               (static_cast<uint8_t>(data[5]) << 16) | static_cast<uint8_t>(data[6]) << 24));
    }
    bool Header::checkSize() const {
        return size < MAX_PAYLOAD_SIZE ;
    }

    uint8_t Header::getVersion() const {
        return version;
    }
    uint16_t Header::getCode() const {
        return code;
    }
    uint32_t Header::getSize() const {
        return size;
    }
    Register::Register(const std::array<char, REG_SUCC_SIZE> &data) : Header(std::array<char, HEADER_RESPONSE_SIZE>{
            data[0], data[1], data[2], data[3], data[4], data[5], data[6]}) {
        for (size_t i = 0; i < 16; ++i) {
            uuid.bytes[i] = static_cast<uint8_t>(data[7 + i]);
        }
    }
    UUID16 Register::getUUID() const {
        return uuid;
    }
    bool Register::checkSize() const {
        return getSize() == UUID_LENGTH;
    }
    bool Register::checkSuccess() const {
        return getCode() == regSucc;
    }
ClientLst::ClientLst(std::vector<char>& data)
    :Header([&data]{
    if (data.size() < HEADER_RESPONSE_SIZE)
        throw std::runtime_error("short buffer for header");
    std::array<char, HEADER_RESPONSE_SIZE> a{};
    std::copy_n(data.begin(), HEADER_RESPONSE_SIZE, a.begin());
    return a;
    }())
    {
        numClients = getSize() / (UUID_LENGTH + MAX_NAME_LENGTH);
        if(!checkSize()) {
            throw std::runtime_error("invalid size for client list");
        }
        for(size_t i = HEADER_RESPONSE_SIZE; i < data.size();  ) {
                if (i + UUID_LENGTH + MAX_NAME_LENGTH > data.size()) {
                    throw std::runtime_error("short buffer for client entry");
                }
                std::array<char, UUID_LENGTH + MAX_NAME_LENGTH> entry{};
                std::copy_n(data.begin() + i, UUID_LENGTH + MAX_NAME_LENGTH, entry.begin());
                Client entryClient(entry);
                lst.emplace_back(entry);
                i += UUID_LENGTH + MAX_NAME_LENGTH;
            }

    }
    bool  ClientLst::checkSize() const {
        return getSize() % (UUID_LENGTH + MAX_NAME_LENGTH) == 0 && getSize()/(UUID_LENGTH + MAX_NAME_LENGTH) <= MAX_CLIENT_LST_SIZE;
    }
    uint16_t ClientLst::getNumClients() const {
        return numClients;
    }





