//
// Created by shimo on 08/09/2025.
//

#include "ReadPort.h"




std::vector<std::string> get_data_from_file(std::ifstream &f ){
       if(!f.is_open()) {
           std::cerr << "File not open";
           return {};
       }
    std::string line;
    std::getline(f, line);
    if(line.empty()) {
        std::cerr << "File is empty...\n";
        return {};
    }
    std::string ip = get_ip_from_str(line);
    std::string port = get_port_from_str(line);

    return {ip, port};
    }
std::string get_ip_from_str(std::string & str ){
    size_t pos = str.find(':');
    if(pos == std::string::npos) {
        std::cerr << "No ip found in the string";
        return "";
    }
    return str.substr(0, pos);

}
std::string get_port_from_str(std::string & str ){
    size_t pos = str.find(':');
    if(pos == std::string::npos) {
        std::cerr << "No port found in the string";
        return "";
    }
    return str.substr(pos + 1);

}







