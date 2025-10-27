//
// Created by shimo on 15/10/2025.
//

#include "ReqHandler.h"

#include "Messages.h"
#include "RWClientD.h"

bool connectToServer(tcp::socket &s, tcp::resolver &resolver, const char *address, const char *port) {
    boost::system::error_code ec;
    auto endpoints = resolver.resolve(address, port, ec);
    if (ec) {
        std::cerr << "Failed to resolve address: " << ec.message() << std::endl;
        return false;
    }
    const auto res=  boost::asio::connect(s, endpoints, ec);

    if (ec) {
        std::cerr << "Failed to connect: " << ec.message() << std::endl;
        return false;
    }
    std::cout << "connected to: "  << res << std::endl;
    return true;
}
bool sendRequest(tcp::socket& s, const inputCode code, bool * shouldExit, ClientD*& targetClient, std::vector<Client> & client_list) {
    Header * header = nullptr;
    bool flag = false;
    switch (code) {
        case regC: {
            if (checkFileExists(CLIENT_INFO_PATH) || targetClient != nullptr) {
                std::cerr << "client already registered!\n";
                return false;

            }
            const std::string name = get_name();
            std::cout << "Registering client with name: " << name << std::endl;
            targetClient = new ClientD(name);
            header = new Register(*targetClient);
            flag = true;
            break;
        }

        case reqLstC: {
            if(targetClient == nullptr) {
                std::cerr << "Client not registered. Please register first.\n";
                return false;
            }
            header = new ClientList(targetClient->getUUID());
            flag = true;
            break;
        }

        case reqPubC:{
            if(targetClient == nullptr) {
                std::cerr << "Client not registered. Please register first.\n";
                return false;
            }
            std::string targetUuidStr;
            std::cout << "Enter target client's UUID (32 hex chars): ";
            std::cin >> targetUuidStr;
            if (!checkLegalUUID(targetUuidStr)) {
                std::cerr << "Invalid UUID format.\n";
                return false;
            }
            header = new ReqPubKey(targetClient->getUUID(),
                                                 stringToUUID16(targetUuidStr));
            flag = true;
            break;
        }
        case reqSend:
            try {
                if(targetClient == nullptr) {
                    std::cerr << "Client not registered. Please register first.\n";
                    return false;
                }
                std:: cout << "Enter target client's name:  max 254 chars\n";
                std::string targetName;
                std::getline(std::cin, targetName);
                if(targetName.size() >= MAX_NAME_LENGTH) {
                    std::cerr << "Name too long. Max 254 characters.\n";
                    return false;
                }

                const UUID16 targetUuid = getUUUIDFromName( client_list, targetName );
                header = new TextMsg(targetClient->getUUID(), targetUuid, {});
                dynamic_cast<TextMsg *>(header)->getTextFromClient();
                dynamic_cast<TextMsg *>(header)->UpdateSize();
                flag = true;
            }
        catch (...){
            std::cerr << "Error: Target client not found in client list. Please request client list first.\n";
            return false;
        }
        break;
        case reqMsgs:
            header = new ReqMsg(targetClient->getUUID());
            std::cout << "Requesting messages...\n";
            flag = true;
            break;
        case exitC:
            std::cout << "Exiting....\nGoodbye\n";
            if(shouldExit) *shouldExit = true;
            return false;
        default:
            std::cerr << "Invalid command.\n";
            return false;
    }


    std::vector<uint8_t> data = header->toBytes();
    boost::system::error_code ec;
    boost::asio::write(s, boost::asio::buffer(data), ec);
    delete header;



    if (ec ) {
        std::cerr << "Error sending data: " << ec.message() << '\n';
        flag = false;
        if (shouldExit) *shouldExit = true;
    }
    else{
        std::cout << "Request sent successfully.\n";
    }

    return flag;
}
