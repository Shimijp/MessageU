//
// Created by shimo on 19/10/2025.
//

#ifndef REQUESTHABDLER_H
#define REQUESTHABDLER_H
#include <boost/asio.hpp>
#include "utils.h"
#include "Response.h"
using boost::asio::ip::tcp;
std::vector<char> readExact(tcp::socket& s, std::size_t n);
Response::Response* readAndHandle(tcp::socket& s, std::vector<Client> & clients);
Response::Response * handleResponse(ResponseCode code, std::vector<char> &data, std::vector<Client> & clients);
bool checkSize(ResponseCode code, const std::vector<char> & data);

#endif //REQUESTHABDLER_H

