//
// Created by shimo on 14/10/2025.
//

#ifndef READHEADER_H
#define READHEADER_H
#include <array>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "utils.h"

class Header {
    uint8_t version;
    uint16_t code;
    uint32_t size;
public:
    virtual ~Header() = default;

    explicit Header(const std::array<char, HEADER_RESPONSE_SIZE>  &data);
    uint8_t getVersion() const;
    uint16_t getCode() const;
    uint32_t getSize() const;
    virtual bool checkSize() const;

};
class Register final : Header {
    UUID16 uuid;
public:
    explicit Register(const std::array<char, REG_SUCC_SIZE>  &data);
    UUID16 getUUID() const;
    bool checkSize() const override;
    bool checkSuccess() const;
};
class ClientLst final : Header {
    std::vector<Client> lst;
    uint16_t numClients;
public:
    explicit ClientLst(std::vector <char> & data);
    //explicit ClientLst(std::array<char, MAX_CLIENT_LST_SIZE> & data);
    uint16_t getNumClients() const;
    bool checkSize() const override;
};



#endif //READHEADER_H
