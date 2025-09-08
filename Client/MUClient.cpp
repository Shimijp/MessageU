//
// Created by shimo on 08/09/2025.
//

#include "ReadPort.h"



int main(int argc , char ** argv) {
    std::ifstream f("C:/dev/Client/server.info");

    const std::vector<std::string> data = get_data_from_file(f);
    if(data.empty()) {
        std::cerr << "Error getting ip and port info ... exiting\n";
        return -1;
    }
    std::cout << "IP: " << data[0] << "\nPORT: "<< data[1] << std::endl;
    f.close();
    return 0;
}