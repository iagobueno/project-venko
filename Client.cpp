#include "Client.hpp"

#include <sys/socket.h>     // socket(), socklen_t
#include <netdb.h>          // gethostbyname()
#include <unistd.h>         // gethostname(), read(), write(), NULL
#include <stdlib.h>         // atoi()
#include <string.h>         // memset(), memcpy()
#include <iostream>

Client::Client(const char* port, const char* serverName) {

    // Set port that process will use to communicate
    int p = atoi(port);
    setPort(p);

    // Creates a Socket using IPv4 and TCP protocol
    this->cliSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->cliSocket < 0) {
        throw (int)2;
    }

    /* Resolve the name */
    struct hostent* server;
    if ((server = gethostbyname(serverName)) == NULL) {
        throw (int)3;
    }

    struct sockaddr_in servAddr;       // Structure to server address information
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;              // IPv4 Family
    servAddr.sin_port = htons(p);
    memcpy(&servAddr.sin_addr, server->h_addr, server->h_length);

    // Connect to server
    if (connect(this->cliSocket, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        throw (int)4;
    }
}

Client::~Client() {
    close(this->cliSocket);
}

bool Client::checkPort(const int port) {
    if (port < 1024 || port > 49151)
        return false;
    return true;
}

void Client::setPort(const int port) {
    if (!checkPort(port)) {
        throw (int)1;
    }
    this->port = port;
}

int Client::getPort() {
    return this->port;
}

void Client::commWithServer() {

    char buffer[256];

    int i;
    int nBytes;
    for (i = 0;i < 2;++i) {
        memset(buffer, 0, sizeof(buffer));  // reset buffer
        std::cin >> buffer;
        nBytes = write(cliSocket, buffer, strlen(buffer));

        if (nBytes < 0)
        {
            std::cout << "Error while writing from socket." << std::endl;
            exit(9);
        }

        memset(buffer, 0, sizeof(buffer));  // reset buffer

        nBytes = read(cliSocket, buffer, 255);
        if (nBytes < 0) {
            std::cout << "Error while reading from socket." << std::endl;
            exit(8);
        }

        std::cout << "Server: " << buffer << std::endl;
    }

}
