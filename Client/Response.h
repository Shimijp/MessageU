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
#include "Messages.h"
#include "RSAWrapper.h"
#include "utils.h"

// Response.h - Defines response message structures for MessageU protocol
// Includes header, registration response, and client list response classes

namespace Response {

    class Header final {
        uint8_t version; // Protocol version
        uint16_t code;   // Response code (LE on wire)
        uint32_t size;   // Payload size (LE on wire)
    public:
        virtual ~Header() = default;
        explicit Header(const std::vector<char>& data); // Parse header from bytes
        uint8_t getVersion() const;  // Get protocol version
        uint16_t getCode() const;    // Get response code
        uint32_t getSize() const;    // Get payload size
        bool checkSize() const; // Validate payload size
    };

    class Response {
    public:
        virtual ~Response() = default;
        Response() = default;
        virtual bool checkSuccess() const;
        virtual ResponseCode getResponseCode() const  = 0;
    };

    // Registration response: contains UUID
    class Register final : public Response {
        UUID16 uuid; // Registered client UUID
    public:
        explicit Register(const std::vector<char>& data); // Parse from bytes
        UUID16 getUUID() const;          // Get registered UUID
        bool checkSuccess() const override; // Check if registration succeeded
        ResponseCode getResponseCode() const override;

    };

    // Client list response: contains list of clients
    class ClientLst final : public Response {
        std::vector<Client> lst; // List of clients
        uint16_t numClients;     // Number of clients
    public:
        explicit ClientLst(std::vector<char>& data); // Parse from bytes
        uint16_t getNumClients() const;              // Get number of clients
        const std::vector<Client>& getList() const { return lst; }
        bool checkSuccess() const override;
        ResponseCode getResponseCode() const override;
        std::vector<Client> getClients() const { return lst; }
        void printClientList() const;
    };

    class PubKey final : public Response {
        UUID16 uuid;
        RSAPublicWrapper* rsaPub;
    public:
        explicit PubKey(const std::vector<char>& data);
        ~PubKey() override; // free rsaPub

        RSAPublicWrapper* getPubKey() const;
        UUID16 getUUID() const { return uuid; }
        bool checkSuccess() const override;
        ResponseCode getResponseCode() const override;
    };
    class GenError final : public Response {
        std::vector<char> data;
    public:
        explicit GenError(const std::vector<char>& dataIn) : data(dataIn) {}
        ResponseCode getResponseCode() const override {
            return err;
        }
    };
    class MsgSent final : public Response {
        UUID16 d_uuid;
        uint32_t msgID;
    public:
        explicit MsgSent(const std::vector<char>& data);
        uint32_t getMsgID() const { return msgID; }
        ResponseCode getResponseCode() const override;
    };
    class MsgCont {
        UUID16 s_uuid;
        uint32_t msgID;
        uint8_t msgType;
        std::string content;

    public:
        MsgCont(const UUID16& s_uuid, const uint32_t msgID, const uint8_t msgType, std::string  content)
            : s_uuid(s_uuid), msgID(msgID), msgType(msgType), content(std::move(content)) {}
        UUID16 getUUID() const {
            return s_uuid;
        }
        uint32_t getMsgID() const {
            return msgID;
        }
        uint8_t getMsgType() const {
            return msgType;
        }
        std::string getContent() const {
            return content;
        }
        void updateContent(const std::string& newContent) {
            content = newContent;
        }
    };
    class ReadMsg:public Response {
        std::vector<MsgCont> messages;
    public:
        explicit ReadMsg(const std::vector<char> &data);
        void printMsgs(std::map<UUID16, ClientSvd> & svdClients) const;
        ResponseCode getResponseCode() const override ;

    };

}

#endif // READHEADER_H
