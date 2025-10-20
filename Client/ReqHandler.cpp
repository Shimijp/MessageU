//
// Created by shimo on 15/10/2025.
//

#include "ReqHandler.h"

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
bool sendRequest(tcp::socket& s, const inputCode code, bool* shouldExit) {
    Header * header;
    bool flag = false;

    switch (code) {
        case regC: {
            if (checkFileExists(CLIENT_INFO_PATH)) {
                std::cerr << "client already registered!\n";
                return false;
            }
            const std::string name = get_name();
            const ClientD client_d(name);
            header = new Register(client_d);
            flag = true;
            break;
        }

        case reqLstC: {
            ClientD client_d;
            try { loadClientFromFile(CLIENT_INFO_PATH, client_d); }
            catch (const std::exception& e) {
                std::cerr << "Error loading client data: " << e.what() << '\n';
                return false;
            }
            header = new ClientList(client_d.getUUID());
            flag = true;
            break;
        }

        case getPubKey: {
            ClientD client_tt;
            try { loadClientFromFile(CLIENT_INFO_PATH, client_tt); }
            catch (const std::exception& e) {
                std::cerr << "Error loading client data: " << e.what() << '\n';
                return false;
            }
            std::string targetUuidStr;
            std::cout << "Enter target client's UUID (32 hex chars): ";
            std::cin >> targetUuidStr;
            if (!checkLegalUUID(targetUuidStr)) {
                std::cerr << "Invalid UUID format.\n";
                return false;
            }
            header = new ReqPubKey(client_tt.getUUID(),
                                                 stringToUUID16(targetUuidStr));
            flag = true;
            break;
        }
        case exitC:
            std::cout << "Exiting....\nGoodbye";
            return true;

        default:
            std::cerr << "Invalid command.\n";
            return false;
    }


    std::vector<uint8_t> data = header->toBytes();
    boost::system::error_code ec;
    boost::asio::write(s, boost::asio::buffer(data), ec);



    if (ec) {
        std::cerr << "Error sending data: " << ec.message() << '\n';
        flag = false;
        if (shouldExit) *shouldExit = true;
    }

    return flag;
}

