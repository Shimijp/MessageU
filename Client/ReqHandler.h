//
// Created by shimo on 15/10/2025.
//

#ifndef REQHANDLER_H
#define REQHANDLER_H
#include <iostream>

#include "Request.h"
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
bool connectToServer(tcp::socket &s, tcp::resolver &resolver, const char *address, const char *port);
bool sendRequest(tcp::socket & s,  inputCode code,  bool * shouldExit, ClientD *&targetClient);
#endif //REQHANDLER_H
