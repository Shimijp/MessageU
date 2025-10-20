//
// Created by shimo on 08/09/2025.
//
#include <boost/asio.hpp>
#include "ReqHandler.h"
#include "Response.h"
#include "ReadPort.h"
#include "RWClientD.h"
#include "utils.h"

using boost::asio::ip::tcp;


int main(int argc , char ** argv) {
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
        sendRequest(s, static_cast<inputCode>(num_choice), &should_exit);


        //std::array<char ,  HEADER_RESPONSE_SIZE> header_data{};
        std::string buff;
        s.read_some(boost::asio::buffer(buff));
        std::cout << buff << std::endl;
        for(auto & c : buff) {
            std::cout << std::hex << (static_cast<int>(c) & 0xff) << " ";
        }

    }
    if (s.is_open()) s.close();
    return 0;
}