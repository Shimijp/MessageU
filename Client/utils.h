//
// Created by shimo on 13/10/2025.
// utils.h - Utility definitions and structures for MessageU client
// Contains constants, enums, and basic data structures used throughout the client
//

#ifndef UTILS_H
#define UTILS_H
#include <array>
#include <limits>
#include <cstdint>
#include <string>
#define MAX_PAYLOAD_SIZE (std::numeric_limits<std::uint32_t>::max()) // Maximum payload size (4GB)
#define HEADER_RESPONSE_SIZE 7 // Size of response header in bytes
#define HEADER_SIZE 23 // Size of request header in bytes
#define KEY_LENGTH  160 // Length of RSA public key in bytes
#define UUID_LENGTH 16 // Length of UUID in bytes
#define MAX_NAME_LENGTH 255 // Maximum length for client name
#define REG_SUCC_SIZE (UUID_LENGTH) // Size of successful registration response
#define MAX_CLIENT_LST_SIZE (MAX_PAYLOAD_SIZE/ (MAX_NAME_LENGTH + UUID_LENGTH)) // Maximum number of clients in list
#define REGISTER_LENGTH  (MAX_NAME_LENGTH + KEY_LENGTH + HEADER_SIZE) // Length of registration request
#define MENU "110) Register\n120)Request for client list\n130_Request for public Key\n0) exit client\n"
#define SERVER_INFO_PATH "C:/dev/MessageU/Client/server.info"
#define CLIENT_INFO_PATH "C:/dev/MessageU/Client/client.info"

// Version enums for client and server
enum versions {
    clientV1 = 1,
    serverV1 = 1,
    serverV2 = 2
};

// UUID16: 16-byte unique identifier for clients
typedef struct  UUID16
{
    //array of bytes for the uuid
    std::array<std::uint8_t, UUID_LENGTH> bytes{};
    bool operator==(const UUID16& other) const { return bytes == other.bytes; }
}UUID16;

// Client: Structure representing a client with UUID and name
struct Client {
    UUID16 uuid16;
    std::string name;
    Client(const UUID16 uuid, std::string  name) : uuid16(uuid) , name(std::move(name)){}
    explicit Client(std::array<char, UUID_LENGTH+ MAX_NAME_LENGTH> & data);
};

// Request and response codes for messaging protocol
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

// Input codes for user commands
enum inputCode {
    regC = 110,
    reqLstC = 120,
    reqPubC = 130,
    reqMsgs = 140,
    reqSend = 150,
    reqSKey = 151,
    reqSendSKey = 152,
    exitC = 0
};
#endif //UTILS_H
