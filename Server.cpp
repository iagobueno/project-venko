#include "Server.hpp"
#include <stdlib.h>         // atoi()
#include <sys/socket.h>     // socket(), socklen_t
#include <unistd.h>         // gethostname(), read(), write()
#include <netdb.h>          // gethostbyname()
#include <string.h>         // memset(), memcpy()
#include <iostream>

Server::Server(const char* port) {

    // Set port that process will use to communicate
    int p = atoi(port);
    setPort(p);

    // Creates a Socket using IPv4 and TCP protocol
    masterSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (masterSocket < 0) {
        throw (int)2;
    }

    // Gets localhost on hostname
    char hostname[MAX_HOST_NAME];
    if (gethostname(hostname, MAX_HOST_NAME) != 0) {
        throw (int)3;
    }

    /* Resolve the name */
    struct hostent* host;
    if ((host = gethostbyname(hostname)) == NULL) {
        throw (int)4;
    }

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;              // IPv4 Family
    servAddr.sin_addr.s_addr = INADDR_ANY;      // Accepts connection on all network interfaces
    servAddr.sin_port = htons(p);
    memcpy(&servAddr.sin_addr, host->h_addr, host->h_length);

    // Binds master socket to address
    if (bind(masterSocket, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        throw (int)5;
    }

    this->cliLength = sizeof(cliAddr);          // The size of the client address structure
    if (listen(this->masterSocket, MAX_CONNECTIONS) < 0) {      // Server is now listening on masterSocket
        throw (int)6;
    }

}

Server::~Server() {
    close(masterSocket);
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

int Server::getPort() {
    return this->port;
}

void Server::acceptNewSession() {
    this->newSocket = accept(this->masterSocket, (struct sockaddr*)&cliAddr, &cliLength);
    if (newSocket < 0) {
        throw (int)1;
    }
}

void Server::closeMaster() {
    close(this->masterSocket);
}

void Server::closeNewSocket() {
    close(this->newSocket);
}

void Server::handleCliComm(ServerLog* log) {
    memset(this->buffer, 0, 256);

    int nBytes, i;
    for (i = 0;i < 2;++i) {
        memset(buffer, 0, 256);
        nBytes = read(this->newSocket, buffer, 255);

        if (nBytes < 0) {
            log->writeLog("Error while reading from socket.");
        }
        else {
            std::cout << "I receive the message: " << buffer << std::endl;
        }

        nBytes = write(this->newSocket, "I got your message.", 19);

        if (nBytes < 0)
        {
            log->writeLog("Error while writing on socket.");
        }
    }

    close(this->newSocket);
}