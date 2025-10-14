//
// Created by shimo on 05/10/2025.
//

#ifndef HEADER_H
#define HEADER_H

#include "ClientD.h"
#define HEADER_SIZE 23
struct Header {
    UUID16 client_id;
    uint8_t version;
    uint16_t msg_type;
    uint32_t payload_length;

    Header(const ClientD& client, const uint16_t msg_type, const uint32_t payload_length)
        : client_id(client.getUUID()), version(1),
          msg_type(msg_type), payload_length(payload_length) {}

    std::array<uint8_t, HEADER_SIZE> to_bytes() const;
    static Header from_bytes(const std::array<uint8_t, HEADER_SIZE>& bytes);
};
#endif //HEADER_H
