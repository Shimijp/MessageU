//
// Created by shimo on 03/10/2025.
//

#ifndef RWCLIENTD_H
#define RWCLIENTD_H
#include <fstream>
#include "clientD.h"


bool checkFileExists(const std::string& path);
bool checkLegalUUID(const std::string &uuid);
UUID16 stringToUUID16(const std::string& uuidStr);

void loadClientFromFile(const std::string& path, ClientD& client);
void saveClientToFile(const std::string& path, const ClientD& client);

#endif //RWCLIENTD_H
