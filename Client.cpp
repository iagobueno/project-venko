#include "Client.hpp"
#include "UserInterface.hpp"
#include "message.h"

// Streams handle
#include <sstream>
#include <fstream>
#include <iostream>

// C functions
#include <sys/socket.h>     // socket(), socklen_t
#include <netdb.h>          // gethostbyname()
#include <unistd.h>         // gethostname(), read(), write(), NULL
#include <stdlib.h>         // atoi()
#include <string.h>         // memset(), memcpy()

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

    // Resolve the name
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

void Client::sendData(std::string data) {

    // Creates buffer and makes sure it is clean
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    // Cast string data to char
    const char* cstr = data.c_str();
    memcpy(buffer, cstr, sizeof(buffer));

    // Writes char of data on client Socket
    int nBytes;
    nBytes = write(this->cliSocket, buffer, BUFFER_SIZE);
    if (nBytes < 0)
    {
        throw(int)5;
    }
}

std::string Client::recData() {

    char buffer[BUFFER_SIZE];
    int nBytes;

    memset(buffer, 0, BUFFER_SIZE);
    nBytes = read(this->cliSocket, buffer, BUFFER_SIZE);

    if (nBytes < 0) {
        throw(int)6;
    }

    std::string ret{ buffer };
    return ret;
}

void Client::endConnection() {

    std::string msg{ "0 " };        // protocol version
    msg.append("quit ");            // command

    // Send and receive data to confirm that connection was ended
    sendData(msg);
    std::cout << recData() << std::endl;

    this->quit = true;
}

void Client::removeFile(std::string fileName) {
    std::string msg{ "0 " };      // protocol version
    msg.append("remove ");           // command
    msg.append(fileName);           // fileName

    sendData(msg);
    std::cout << recData() << std::endl;
}

void Client::listFiles() {

    std::string msg{ "0 " };      // protocol version
    msg.append("list");           // command

    sendData(msg);
    std::cout << recData() << std::endl;
}

void Client::helloServer() {

    std::string msg{ "0 " };        // protocol version
    msg.append("hello ");           // command
    msg.append(getUserName());      // userName

    // Send and receive data to confirm that connection was established
    sendData(msg);
    std::cout << recData() << std::endl << std::endl;
}

void Client::getFile(std::string fileName) {
    std::string msg{ "0 " };        // protocol version
    msg.append("get ");             // command
    msg.append(fileName);           // fileName

    // Send string asks for a file to Server
    sendData(msg);

    // Creates local file to get the payload of server
    std::string filePath{ "./" };
    filePath.append(fileName);
    std::ofstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error creating file: " << filePath << std::endl;
        return;
    }

    // Receive the total number of chunks
    int numChunks;
    read(cliSocket, &numChunks, sizeof(numChunks));

    char buffer[CHUNK_SIZE];
    int nBytes;
    // Receive and write each chunk to the file
    for (int i = 0; i < numChunks; ++i) {

        // clean the buffer and gets a chunk
        memset(buffer, 0, sizeof(buffer));
        nBytes = read(cliSocket, buffer, sizeof(buffer));

        if (nBytes < 0) {
            throw(int)6;
        }

        // Write the chunk to the file
        file.write(buffer, nBytes);
    }

    file.close();
    std::cout << "File downloaded just fine." << std::endl;
}

void Client::uploadFile(std::string filePath) {

    // Opens file
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }

    std::string msg{ "0 " };        // protocol version
    msg.append("upload ");          // command
    msg.append(filePath);           // userName

    // Send string to annouce that a file is going to be uploaded
    sendData(msg);

    // Get the size of the file
    file.seekg(0, std::ios::end);
    int fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Calculate the number of chunks and send it to server
    int numChunks = (fileSize + CHUNK_SIZE - 1) / CHUNK_SIZE;
    write(cliSocket, &numChunks, sizeof(numChunks));

    // Send the file in chunks
    char buffer[CHUNK_SIZE];
    int nBytes;
    for (int i = 0; i < numChunks; ++i) {

        // clear buffer
        memset(buffer, 0, sizeof(buffer));

        // Read a chunk from the file and send over the network
        file.read(buffer, sizeof(buffer));
        nBytes = write(cliSocket, buffer, sizeof(buffer));
        if (nBytes < 0)
        {
            throw(int)5;
        }
    }

    file.close();
    std::cout << "File uploaded just fine." << std::endl;
}

void Client::parseInput(std::string inputBuffer) {

    // extract command and possible argument
    std::istringstream iss(inputBuffer);
    std::string command, arg;
    iss >> command;
    iss >> arg;

    if (arg.length() > MAX_FILE_NAME) {
        throw (int)3;
    }

    if (command == "help" || command == "h") {
        if (arg != "") {
            throw(int)2;
        }
        ui.help();
    }
    else if (command == "list" || command == "ls") {
        if (arg != "") {
            throw(int)2;
        }
        listFiles();
    }
    else if (command == "quit" || command == "exit") {
        if (arg != "") {
            throw(int)2;
        }
        endConnection();
    }
    else if (command == "remove" || command == "rm") {
        removeFile(arg);
    }
    else if (command == "upload") {
        uploadFile(arg);
    }
    else if (command == "get") {
        getFile(arg);
    }
    else {
        throw(int)4;
    }
}

void Client::commWithServer() {

    // signal that the connection was established successfully
    helloServer();

    for (;!this->quit;) {

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
            else if (error == 2) {
                std::cout << std::endl << "No options or arguments are needed to this command." << std::endl;
            }
            else if (error == 3) {
                std::cout << "Filename must have a maximum length of 32" << std::endl << std::endl;
            }
            else if (error == 4) {
                std::cout << "Command not found." << std::endl << std::endl;
            }
            else if (error == 5) {
                std::cout << "Error while writing on socket." << std::endl << std::endl;
            }
            else if (error == 6) {
                std::cout << "Error while reading on socket." << std::endl << std::endl;
            }
            else {
                std::cout << "Unknown error while parsing input." << std::endl << std::endl;
            }

            std::cout << "If you're lost, type \"help\" and hit Enter." << std::endl << std::endl;
        }
    }
}
