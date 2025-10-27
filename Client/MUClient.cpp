//
// Created by shimo on 08/09/2025.
//
#include <boost/asio.hpp>
#include "ReqHandler.h"
#include "Response.h"
#include "ReadPort.h"
#include "RespHandler.h"
#include "RWClientD.h"
#include "utils.h"

using boost::asio::ip::tcp;


int main() {
    std::ifstream f(SERVER_INFO_PATH);
    const std::vector<std::string> data = get_data_from_file(f);
    if(data.empty()) {
        std::cerr << "Error getting ip and port info ... exiting\n";
        return -1;
    }
    std::cout << "IP: " << data[0] << "\nPORT: "<< data[1] << std::endl;
    f.close();
    const char *address = data[0].c_str();
    const char * port = data[1].c_str();
    boost::asio::io_context io_context;
    tcp::socket s(io_context);
    tcp::resolver resolver(io_context);

    if (!connectToServer(s, resolver, address, port)) {
        return -1;
    }

    std::cout << "MessageU client at your service\n";
    std::string choice;
    bool should_exit = false;
    ClientD * client_d = nullptr;
    std::vector<Client> client_list;
    if(checkFileExists(CLIENT_INFO_PATH)) {
        try {
            client_d = new ClientD();
            loadClientFromFile(CLIENT_INFO_PATH, client_d);
            std::cout << "Loaded client data from file. Name: " << client_d->getName() << std::endl;
        } catch (std::runtime_error & e) {
            std::cerr << "Error loading client data: " << e.what() << "\n";
            delete client_d;
            client_d = nullptr;
            if (s.is_open()) s.close();
            return -1;
        }
    }
    while(!should_exit) {
        if (!s.is_open()) {
            std::cerr << "Disconnected from server. Exiting..." << std::endl;
            break;
        }
        std::cout << MENU;
        std::getline(std::cin,choice);
        int num_choice = 0;
        try {
            num_choice = std::stoi(choice);
        } catch (...) {
            std::cout << "Invalid input. Try again.\n";
            continue;
        }
        bool legalReq = sendRequest(s, static_cast<inputCode>(num_choice), &should_exit, client_d, client_list);
        if(legalReq) {
            Response::Response * resp = readAndHandle(s, client_list);
            if(resp == nullptr) {
                continue;
            }
            ResponseCode code = resp->getResponseCode();
            if(code == regSucc) {
                try {
                    client_d->setUUID(dynamic_cast<Response::Register *>(resp)->getUUID());
                    saveClientToFile(CLIENT_INFO_PATH, *client_d);
                    std::cout << "Client data saved to file.\n";
                } catch (std::runtime_error & e) {
                    std::cerr << "Error saving client data: " << e.what() << "\n";
                }
            }
            else if(code == userListSucc) {
                auto * clientLstResp = dynamic_cast<Response::ClientLst *>(resp);
                client_list = clientLstResp->getClients();

            }
            else if(code == err) {
                std::cerr << "Server returned an error response.\n";
            }

            delete resp;
        }







    }
    if (s.is_open()) s.close();
    if(client_d != nullptr) delete client_d;
    return 0;
}