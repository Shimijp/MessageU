//
// Created by shimo on 05/10/2025.
//

#include "header.h"
std::array<uint8_t, 23> Header::to_bytes() const {
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

