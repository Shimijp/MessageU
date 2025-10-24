//
// Created by shimo on 08/09/2025.
//

#ifndef READPORT_H
#define READPORT_H
#define DEFAULT_PORT "1234"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
std::vector<std::string> get_data_from_file(std::ifstream &f );
std::string get_ip_from_str(std::string & str );
std::string get_port_from_str(std::string & str );
#endif //READPORT_H
