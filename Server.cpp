#include "Server.hpp"
#include <stdlib.h>         // atoi()
#include <sys/socket.h>     // socket(), socklen_t
#include <unistd.h>         // gethostname(), read(), write()
#include <netdb.h>          // gethostbyname()
#include <string.h>         // memset(), memcpy()
#include <iostream>
#include <sstream>
#include <arpa/inet.h>

#include <filesystem>

Server::Server(const char* port, ServerLog* log) {

    // Set port that process will use to communicate
    int p = atoi(port);
    setPort(p);

    // set serverlog
    this->log = log;

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

    // Set the size of the client address structure
    this->cliLength = sizeof(cliAddr);

    // Server is now listening on masterSocket
    if (listen(this->masterSocket, MAX_CONNECTIONS) < 0) {
        throw (int)6;
    }

}

Server::~Server() {
    close(masterSocket);
}

// Check if port is out of allowed range
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

// Mastersocket await fopr a new connection and then creates a new socket to handle it
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

// Send data across network
void Server::sendData(std::string data) {
    int nBytes;
    char buffer[BUFFER_SIZE];

    // clear buffer, convert data string into char* and copy it to buffer
    memset(buffer, 0, BUFFER_SIZE);
    const char* cstr = data.c_str();
    memcpy(buffer, cstr, sizeof(buffer));

    // with data copied to buffer, we could send it
    nBytes = write(this->newSocket, buffer, BUFFER_SIZE);

    if (nBytes < 0)
    {
        this->log->writeLog("Error while writing on socket.");
    }

}

void Server::setCurrentUser(const std::string cUser) {
    this->currentUser = cUser;
}

std::string Server::getCurrentUser() {
    return this->currentUser;
}

std::string Server::helloClient() {
    // build string Connection accepted for client <username> From <IP Address>
    std::string helloBuffer{ "Session opened for user " };
    helloBuffer.append(getCurrentUser());
    helloBuffer.append(" from ");

    // Append IP address to hello Buffer
    char cliAddress[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(cliAddr.sin_addr), cliAddress, INET_ADDRSTRLEN);
    helloBuffer.append(cliAddress);
    helloBuffer.append(".");

    // send data across network and return to write on log
    sendData(helloBuffer);
    return helloBuffer;
}

void Server::checkUserDir() {
    std::string userDirPath{ "./" };
    userDirPath.append(getCurrentUser());

    // Check if the directory already exists
    if (!std::filesystem::exists(userDirPath)) {

        // Create the directory
        if (std::filesystem::create_directory(userDirPath)) {
            std::string logDir{ "Directory for user " };
            logDir.append(getCurrentUser());
            logDir.append(" was created successfully.");
            this->log->writeLog(logDir);
        }
        else {
            std::string logDir{ "Error creating directory for user " };
            logDir.append(getCurrentUser());
            logDir.append(".");
            this->log->writeLog(logDir);
        }
    }
}

void Server::listFiles() {
    std::string userDirPath{ "./" };
    userDirPath.append(getCurrentUser());

    std::string files;
    try {
        // Iterate over the files of the directory
        for (const auto& entry : std::filesystem::directory_iterator(userDirPath)) {
            // Concatenate the filename to the string with a space separator
            files += entry.path().filename().string() + " ";
        }

        // Trim the trailing space if there are files
        if (!files.empty()) {
            files.pop_back();
        }

        sendData(files);
    }
    catch (const std::filesystem::filesystem_error& ex) {
        std::string logBuffer{ "Error accessing directory:" };
        logBuffer.append(userDirPath);
        this->log->writeLog(logBuffer);
    }
}

void Server::parseCliMsg() {
    std::string s{ this->buffer };              // converts char buffer into string
    std::istringstream iss(s);                  // then converts string into iss stream
    std::string protocol, command, arg;         // separate protocol, command and argument
    iss >> protocol; iss >> command; iss >> arg;

    if (command == "hello") {
        setCurrentUser(arg);
        this->log->writeLog(helloClient());
        checkUserDir();
    }
    if (command == "list" || command == "ls") {
        listFiles();
    }
}

void Server::handleCliComm() {
    // Reset buffer
    memset(this->buffer, 0, BUFFER_SIZE);

    int nBytes, i;
    for (i = 0;i < 2;++i) {
        // clear buffer and read from socket
        memset(buffer, 0, BUFFER_SIZE);
        nBytes = read(this->newSocket, buffer, BUFFER_SIZE);

        if (nBytes < 0) {
            this->log->writeLog("Error while reading from socket.");
        }

        parseCliMsg();

        // nBytes = write(this->newSocket, "I got your message.", 19);

        // if (nBytes < 0)
        // {
        //     log->writeLog("Error while writing on socket.");
        // }
    }

    close(this->newSocket);
}