//
// Created by shimo on 19/10/2025.
//

#ifndef REQUESTHABDLER_H
#define REQUESTHABDLER_H

#endif //REQUESTHABDLER_H
#include "utils.h"
#include "Response.h"
bool handleResponse(ResponseCode code, std::vector<char> &data);
bool checkSize(RequestCode code,  std::vector<char> & data);