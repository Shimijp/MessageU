//
// Created by shimo on 13/10/2025.
//

#ifndef UTILS_H
#define UTILS_H
#include <limits>
#include <cstdint>
#define MAX_PAYLOAD_SIZE (std::numeric_limits<std::uint32_t>::max()) // 4GB
#define HEADER_RESPONSE_SIZE 7
#define HEADER_SIZE 23
#define KEY_LENGTH  160
#define UUID_LENGTH 16
#define MAX_NAME_LENGTH 255
#define REG_SUCC_SIZE (UUID_LENGTH+HEADER_RESPONSE_SIZE)
#define MAX_CLIENT_LST_SIZE (MAX_PAYLOAD_SIZE/ (MAX_NAME_LENGTH + UUID_LENGTH))
#define REGISTER_LENGTH  (MAX_NAME_LENGTH + KEY_LENGTH + HEADER_SIZE)
enum versions {
    clientV1 = 1,
    serverV1 = 1,
    serverV2 = 2
};
typedef struct  UUID16
{
    //array of bytes for the uuid
    std::array<std::uint8_t, UUID_LENGTH> bytes{};
    bool operator==(const UUID16& other) const { return bytes == other.bytes; }
}UUID16;
struct Client {
    UUID16 uuid16;
    std::string name;
    Client(const UUID16 uuid, std::string  name) : uuid16(uuid) , name(std::move(name)){}
    explicit Client(std::array<char, UUID_LENGTH+ MAX_NAME_LENGTH> & data);
};
enum RequestCode {
    reg=  600,
    userList = 601,
    getPubKey = 602,
    sendMsg = 603,
    getMsg = 604

};
enum ResponseCode {
    regSucc = 2101,
    userListSucc = 2102,
    getPubKeySucc = 2103,
    sendMsgSucc = 2104,
    getMsgSucc = 2105,
    err = 9000,
};
enum inputCode {
    regC = 110,
    exitC = 0
};
#endif //UTILS_H
