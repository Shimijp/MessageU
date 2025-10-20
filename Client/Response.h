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

#include "RSAWrapper.h"
#include "utils.h"

// Response.h - Defines response message structures for MessageU protocol
// Includes header, registration response, and client list response classes

namespace Response {
    class Header final {
        uint8_t version; // Protocol version
        uint16_t code;   // Response code
        uint32_t size;   // Payload size
    public:
        virtual ~Header() = default;
        explicit Header(const std::vector<char> & data); // Parse header from bytes
        uint8_t getVersion() const; // Get protocol version
        uint16_t getCode() const;   // Get response code
        uint32_t getSize() const;   // Get payload size
        virtual bool checkSize() const; // Validate payload size
    };
    // Registration response: contains UUID
    class Register final  {
        UUID16 uuid; // Registered client UUID
    public:
        explicit Register(const  std::vector<char> & data); // Parse from bytes
        UUID16 getUUID() const; // Get registered UUID
        bool checkSuccess() const; // Check if registration succeeded
    };
    // Client list response: contains list of clients
    class ClientLst final {
        std::vector<Client> lst; // List of clients
        uint16_t numClients; // Number of clients
    public:
        explicit ClientLst(std::vector <char> & data); // Parse from bytes
        uint16_t getNumClients() const; // Get number of clients
    };
    class PubKey final {
        UUID16 uuid;
        RSAPublicWrapper * rsaPub;
    public:
        explicit PubKey(const std::vector<char> & data);
        RSAPublicWrapper * getPubKey() const;
        bool checkSize() const ;

    };
}

#endif //READHEADER_H
