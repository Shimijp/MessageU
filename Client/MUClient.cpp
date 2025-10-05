//
// Created by shimo on 08/09/2025.
//
#include <boost/asio.hpp>
#include "ReadPort.h"
#include "Register.h"
#define MENU "110) register\n0) exit client\n"
using boost::asio::ip::tcp;

int main(int argc , char ** argv) {
    std::ifstream f("C:/dev/MessageU/Client/server.info");

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

    tcp::endpoint res = boost::asio::connect(s, resolver.resolve(address, port));


    int max_length = 1024;
    std::cout << "MessageU client at your service\n"<< std::endl;
    std::string msg;
    std::string choise;
    while(choise != "0") {
        std::cout << MENU;
        std::getline(std::cin,choise);
        switch(choise) {
            case "110": {
                msg = "110";
                boost::asio::write(s, boost::asio::buffer(msg, msg.length()));
                std::string name = get_name();
                msg = name;
                break;
            }
            default:
                std::cout << "Unknown command\n";

        }

    }


    s.close();
    return 0;
}