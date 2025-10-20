//
    // Created by shimo on 14/10/2025.
    //

    #include "Response.h"

    #include <algorithm>
    #include <stdexcept>

#include "Base64Wrapper.h"

Client::Client(std::array<char, UUID_LENGTH + MAX_NAME_LENGTH> &data) {
        for (size_t i = 0; i < UUID_LENGTH; ++i) {
            uuid16.bytes[i] = static_cast<uint8_t>(data[i]);
        }
        name = std::string(&data[UUID_LENGTH]);
    }

    // Response namespace implementations
    namespace Response {
    Header::Header(const std::vector<char>& data) {
        if (data.size() < HEADER_RESPONSE_SIZE) {
            throw std::runtime_error("short header");
        }


        const auto* b = reinterpret_cast<const uint8_t*>(data.data());

        version = b[0];


        code = static_cast<uint16_t>(
            static_cast<uint16_t>(b[1]) |
            (static_cast<uint16_t>(b[2]) << 8)
        );

        size = static_cast<uint32_t>(b[3]) |
               (static_cast<uint32_t>(b[4]) << 8)  |
               (static_cast<uint32_t>(b[5]) << 16) |
               (static_cast<uint32_t>(b[6]) << 24);
    }
    bool Header::checkSize() const {
        return size < MAX_PAYLOAD_SIZE;
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

    Register::Register(const std::vector<char>  &data)  {
        for (size_t i = 0; i < UUID_LENGTH; ++i) {
            uuid.bytes[i] = static_cast<uint8_t>( data[i]);
        }
    }

    UUID16 Register::getUUID() const {
        return uuid;
    }





    ClientLst::ClientLst(std::vector<char>& data)
    {
        numClients = data.size()/ (UUID_LENGTH + MAX_NAME_LENGTH);
        for(size_t i = 0; i < data.size();  ) {
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



    uint16_t ClientLst::getNumClients() const {
        return numClients;
    }

    PubKey::PubKey(const std::vector<char> & data)
        {
            std::string decoded;

            for(int i = 0; i < KEY_LENGTH; i++ ) {
                decoded+= data[i];
            }
                decoded.reserve(); //little endian
                rsaPub = new RSAPublicWrapper(decoded);
            for(int i = KEY_LENGTH; i < KEY_LENGTH+ UUID_LENGTH; i++) {
                uuid.bytes[i] = static_cast<uint8_t>(data[i]);
            }
        }

        RSAPublicWrapper * PubKey::getPubKey() const {
            return rsaPub;
        }



    }
