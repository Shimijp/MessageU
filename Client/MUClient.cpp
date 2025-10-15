//
// Created by shimo on 08/09/2025.
//
#include <boost/asio.hpp>
#include "Request.h"
#include "ReadPort.h"
#include "RWClientD.h"
#include "utils.h"
#define MENU "110) register\n0) exit client\n"
#define SERVER_INFO_PATH "C:/dev/MessageU/Client/server.info"
#define CLIENT_INFO_PATH "C:/dev/MessageU/Client/client.info"
using boost::asio::ip::tcp;

// Helper function to connect to server
bool connect_to_server(tcp::socket &s, tcp::resolver &resolver, const char *address, const char *port) {
    boost::system::error_code ec;
    auto endpoints = resolver.resolve(address, port, ec);
    if (ec) {
        std::cerr << "Failed to resolve address: " << ec.message() << std::endl;
        return false;
    }
    auto res=  boost::asio::connect(s, endpoints, ec);
    if (ec) {
        std::cerr << "Failed to connect: " << ec.message() << std::endl;
        return false;
    }
    return true;
}

// Helper function to read and print server response
bool read_and_print_response(tcp::socket &s) {
    boost::system::error_code ec;
    std::array<char, MAX_PAYLOAD_SIZE> buf{};
    const size_t len = s.read_some(boost::asio::buffer(buf), ec);
    if (!ec && len > 0) {
        std::cout << "Server response: " << std::string(buf.data(), len) << std::endl;
        return false;
    } if (ec == boost::asio::error::eof || ec == boost::asio::error::connection_reset) {
        std::cerr << "Connection closed by server." << std::endl;

    } if (ec) {
        std::cerr << "Error reading response: " << ec.message() << std::endl;
    }
    return true;
}

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

    if (!connect_to_server(s, resolver, address, port)) {
        return -1;
    }

    std::cout << "MessageU client at your service\n";
    std::string choise;
    bool should_exit = false;
    while(!should_exit) {
        if (!s.is_open()) {
            std::cerr << "Disconnected from server. Exiting..." << std::endl;
            break;
        }
        std::cout << MENU;
        std::getline(std::cin,choise);
        int num_choise = 0;
        try {
            num_choise = std::stoi(choise);
        } catch (...) {
            std::cout << "Invalid input. Try again.\n";
            continue;
        }
        switch(num_choise) {
            case regC: {
                if(checkFileExists(CLIENT_INFO_PATH)) {
                    std::cerr  << "client already registered!\n";
                    break;
                }
                std::string name = get_name();
                ClientD client_d(name);
                Register reg(client_d);
                std::array<uint8_t, REGISTER_LENGTH> msg = reg.toBytes();
                boost::system::error_code ec;
                boost::asio::write(s, boost::asio::buffer(msg, REGISTER_LENGTH), ec);
                if (ec) {
                    std::cerr << "Error sending data: " << ec.message() << std::endl;
                    break;
                }
                should_exit = read_and_print_response(s);
                break;

            }
            case exitC: {
                std::cout << "Exiting client..." << std::endl;
                s.close();
                return 0;
            }
            default:
                std::cout << "Unknown command\n";
        }
    }
    if (s.is_open()) s.close();
    return 0;
}