//
// Created by shimo on 25/10/2025.
//

#ifndef MESSAGES_H
#define MESSAGES_H

#include "Request.h"

class ReqSymKey final :Message {
public:
    explicit ReqSymKey(UUID16 uuid, UUID16 d_uuid);
    std::vector<uint8_t> toBytes() const override;
};




#endif //MESSAGES_H
