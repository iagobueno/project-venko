#include "Client.hpp"
#include "UserInterface.hpp"
#include "message.h"
#include <sstream>

#include <sys/socket.h>     // socket(), socklen_t
#include <netdb.h>          // gethostbyname()
#include <unistd.h>         // gethostname(), read(), write(), NULL
#include <stdlib.h>         // atoi()
#include <string.h>         // memset(), memcpy()
#include <iostream>

Client::Client(const char* port, const char* serverName) {

    this->quit = false;

    // Set port that process will use to communicate
    int p = atoi(port);
    setPort(p);

    // Set serverName so UI can print it
    std::string str(serverName);
    this->serverName = str;

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

    this->userName = ui.getUserName();
    ui.greetings();

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

std::string Client::getUserName() {
    return this->userName;
}

std::string Client::getServerName() {
    return this->serverName;
}

void Client::listFiles() {

    std::string msg{ "0 " };        // protocol version
    msg.append("list");           // command

    sendData(msg);
    std::cout << recData() << std::endl << std::endl;
}

void Client::parseInput(std::string inputBuffer) {

    // extract first(command) and possible second word ( fileName)
    std::istringstream iss(inputBuffer);
    std::string command, fileName;
    iss >> command;
    iss >> fileName;

    if (command == "help" || command == "h") {
        if (fileName != "") {
            throw(int)2;
        }
        ui.help();
    }
    if (command == "list" || command == "ls") {
        if (fileName != "") {
            throw(int)2;
        }
        listFiles();
    }
    else if (command == "quit" || command == "exit") {
        if (fileName != "") {
            throw(int)2;
        }
        this->quit = true;
    }
    // else if (command == "remove" || command == "rm") {
    // }
    // else if (command == "get") {
    // }
    // else if (command == "upload") {
    // }
    // return 0;

}

void Client::sendData(std::string data) {
    int nBytes;
    char buffer[BUFFER_SIZE];

    memset(buffer, 0, BUFFER_SIZE);
    const char* cstr = data.c_str();
    memcpy(buffer, cstr, sizeof(buffer));

    nBytes = write(this->cliSocket, buffer, BUFFER_SIZE);

    if (nBytes < 0)
    {
        std::cout << "Error while writing on socket." << std::endl;
    }
}

std::string Client::recData() {
    int nBytes;
    char buffer[BUFFER_SIZE];

    memset(buffer, 0, BUFFER_SIZE);
    nBytes = read(this->cliSocket, buffer, BUFFER_SIZE);

    if (nBytes < 0) {
        std::cout << "Error while reading from socket." << std::endl;
    }

    std::string ret{ buffer };
    return ret;
}

void Client::helloServer() {

    std::string msg{ "0 " };        // protocol version
    msg.append("hello ");           // command
    msg.append(getUserName());      // userName

    sendData(msg);
    std::cout << recData() << std::endl << std::endl;
}


void Client::commWithServer() {

    helloServer();

    for (;!this->quit;) {
        //         memset(msg.buffer, 0, sizeof(msg.buffer));  // reset buffer

        // Prints [user@machine] $
        ui.prompt(getUserName(), getServerName());

        try {
            std::string inputBuffer = ui.readOpt();
            parseInput(inputBuffer);
        }
        catch (int& error) {
            if (error == 1) {
                std::cout << std::endl << "Special characteres are not allowed." << std::endl;
            }
            if (error == 2) {
                std::cout << std::endl << "No options or arguments are needed to this command." << std::endl;
            }
            std::cout << "If you're lost, type \"help\" and hit Enter." << std::endl << std::endl;
        }

    }

}
