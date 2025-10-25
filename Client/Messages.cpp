//
// Created by shimo on 25/10/2025.
//

#include "Messages.h"
ReqSymKey::ReqSymKey(const UUID16 uuid, const UUID16 d_uuid) :Message(uuid, d_uuid, static_cast<uint8_t> (reqSymKey),0 ) {}
std::vector<uint8_t> ReqSymKey::toBytes() const {
    return Message::toBytes();
}