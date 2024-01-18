#include "Server.hpp"
#include <stdlib.h>         // atoi()

Server::Server(const char* port) {

    int p = atoi(port);
    setPort(p);

}

bool Server::checkPort(const int port) {
    if (port < 1024 || port > 49151)
        return false;
    return true;
}

void Server::setPort(const int port) {
    if (!checkPort(port)) {
        throw (int)1;
    }
    this->port = port;
}