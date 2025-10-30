//
// Created by shimo on 19/10/2025.
//

#include "RespHandler.h"

#include <iostream>

#include "Request.h"
std::vector<char> readExact(tcp::socket& s, std::size_t n) {
    std::vector<char> buf(n);
    boost::system::error_code ec;

    std::size_t bytes_read =
        boost::asio::read(s, boost::asio::buffer(buf.data(), buf.size()), ec);

    if (ec) {
        std::cerr << "Socket read error: " << ec.message() << std::endl;
        buf.clear();
    } else if (bytes_read != n) {
        std::cerr << "Incomplete read: got " << bytes_read << " of " << n << " bytes\n";
        buf.resize(bytes_read);
    }

    return buf;
}
Response::Response* readAndHandle(tcp::socket& s, std::map<UUID16, ClientSvd> & svdClientss) {
    const auto headerBuf = readExact(s, HEADER_RESPONSE_SIZE);
    const Response::Header header(headerBuf);
    const auto code = static_cast<ResponseCode>(header.getCode());
    const uint32_t size = header.getSize();

    auto payload = readExact(s, size);


    return handleResponse(code, payload, svdClientss);
}
Response::Response *  handleResponse(const ResponseCode code, std::vector<char> &data, std::map<UUID16, ClientSvd> & svdClients) {
    Response::Response * resp = nullptr;
    std::cout << "Handling response code: " << static_cast<uint16_t>(code) <<  " size: " << data.size() << std::endl;
    if(!checkSize(code, data)) {
        std::cerr << "Invalid response size\n";
        return resp;
    }
    switch(code) {
        case regSucc: {
            resp = new Response::Register(data);
            std::cout << "Registration successful. UUID: ";
            break;
        }
        case userListSucc: {
            try {
               resp = new Response::ClientLst(data);
                std::cout << "Number of clients: " << dynamic_cast<Response::ClientLst *>(resp)->getNumClients() << std::endl;
                dynamic_cast<Response::ClientLst*>(resp)->printClientList();
                break;
            }catch (std::runtime_error & e) {
                std::cerr << "Error parsing client list: " << e.what() << "\n";
                break;
            }


        }
        case getPubKeySucc: {
            resp = new Response::PubKey(data);
            std::cout << "Public key received successfully.\n" << Base64Wrapper::encode(dynamic_cast<Response::PubKey *>(resp)->getPubKey()->getPublicKey()) << std::endl;

            const bool updated = updateClientPubKey(dynamic_cast<Response::PubKey *>(resp)->getUUID(),
                                                    dynamic_cast<Response::PubKey *>(resp)->getPubKey(),
                                                    svdClients);
            if(!updated) {
                std::cerr << "Failed to update stored public key for client.\n";

            }



            break;
        }
        case sendMsgSucc:
            {
            resp = new Response::MsgSent(data);
            std::cout << "Message sent successfully. Message ID: " << dynamic_cast<Response::MsgSent *>(resp)->getMsgID() << std::endl;
            break;
        }
        case getMsgSucc:
            {
            try {
                resp = new Response::ReadMsg(data);
                dynamic_cast<Response::ReadMsg *>(resp)->printMsgs(svdClients);
            }
            catch (std::runtime_error & e) {
                std::cerr << "Error parsing received message.\n" << e.what();
            }


            break;
        }
        case err: {
            std::cerr << "Error response received from server.\n";
            resp = new Response::GenError(data);
            break;
        }
        default:
            std::cerr << "Unknown response code.\n";
            return resp;

    }
    return resp;
}
bool checkSize(const ResponseCode code, const std::vector<char> & data) {
    switch(code) {
        case regSucc:
            return data.size() == REG_SUCC_SIZE;
            break;
        case userListSucc:
            return data.size() <= MAX_CLIENT_LST_SIZE;
            break;
        case getPubKeySucc:
            return data.size() == KEY_LENGTH + UUID_LENGTH;
            break;
        case sendMsgSucc:
            return data.size() ==MSG_ID_SIZE +UUID_LENGTH;
        case getMsgSucc:
            return data.size() <= MAX_PAYLOAD_SIZE;
        case err:
            return data.empty();
            break;
        default:
            return false;
    }
}