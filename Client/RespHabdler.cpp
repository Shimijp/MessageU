//
// Created by shimo on 19/10/2025.
//

#include "RespHabdler.h"

bool handleResponse(ResponseCode code, std::vector<char> &data);
bool checkSize(const ResponseCode code,  std::vector<char> & data) {
    switch(code) {
        case regSucc:
            return data.size() == REG_SUCC_SIZE;
            break;
        case
    }
}