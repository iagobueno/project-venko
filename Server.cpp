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

    // Set serverlog
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

    this->quit = false;
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

    // Clear buffer, convert data string into char* and copy it to buffer
    memset(buffer, 0, BUFFER_SIZE);
    const char* cstr = data.c_str();
    memcpy(buffer, cstr, sizeof(buffer));

    // With data copied to buffer, we could send it
    nBytes = write(this->newSocket, buffer, BUFFER_SIZE);

    if (nBytes < 0)
    {
        this->log->writeLog("Error while writing on socket");
    }

}

void Server::setCurrentUser(const std::string cUser) {
    this->currentUser = cUser;
}

std::string Server::getCurrentUser() {
    return this->currentUser;
}

void Server::helloClient() {
    // Build string Connection accepted for client <username> From <IP Address>
    std::string helloBuffer{ "Session opened for user " };
    helloBuffer.append(getCurrentUser());
    helloBuffer.append(" from ");

    // Append IP address to hello Buffer
    char cliAddress[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(cliAddr.sin_addr), cliAddress, INET_ADDRSTRLEN);
    helloBuffer.append(cliAddress);
    helloBuffer.append(".");

    // Send data across network and return to write on log
    this->log->writeLog(helloBuffer);
    sendData(helloBuffer);
}

void Server::checkUserDir() {
    std::string userDirPath{ "./home/" };
    userDirPath.append(getCurrentUser());

    // Check if the directory already exists
    if (!std::filesystem::exists(userDirPath)) {

        // Create the directory
        if (std::filesystem::create_directory(userDirPath)) {

            // Create log string and write log
            std::string logDir{ "Directory for user " };
            logDir.append(getCurrentUser());
            logDir.append(" was created successfully");
            this->log->writeLog(logDir);
        }
        else {
            // Create error log string and write log
            std::string logDir{ "Error creating directory for user " };
            logDir.append(getCurrentUser());
            this->log->writeLog(logDir);
        }
    }
}

void Server::removeFile(std::string fileName) {

    std::string filePath{ "./home/" };
    filePath.append(getCurrentUser());
    filePath.append("/");
    filePath.append(fileName);

    std::ifstream inputFileStream(filePath);
    if (!(inputFileStream.is_open())) {
        std::string logBuffer{ "Error while removing file " };
        logBuffer.append(fileName);
        logBuffer.append(" by user ");
        logBuffer.append(getCurrentUser());

        // Write log and send message through network
        this->log->writeLog(logBuffer);
        sendData(logBuffer);
        return;
    }

    // Remove the file
    std::filesystem::remove(filePath);

    // Create log string
    std::string logBuffer{ "File " };
    logBuffer.append(fileName);
    logBuffer.append(" removed successfully by user ");
    logBuffer.append(getCurrentUser());

    // Write log and send message through network
    this->log->writeLog(logBuffer);
    sendData("OK");
}

void Server::listFiles() {
    std::string userDirPath{ "./home/" };
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

void Server::closeConnection() {
    this->quit = true;

    std::string logBuffer{ "Client " };
    logBuffer.append(getCurrentUser());
    logBuffer.append(" CLOSED connection with Server");
    log->writeLog(logBuffer);
    sendData("Thanks for using Venko Project 1.0.0.");
}

void Server::sendFile(std::string fileName) {

    // Creates local file to send the payload of server
    std::string filePath{ "./home/" };
    filePath.append(getCurrentUser());
    filePath.append("/");
    filePath.append(fileName);

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::string logBuffer{ "Error opening file: " };
        logBuffer.append(fileName);
        logBuffer.append(" for user ");
        logBuffer.append(getCurrentUser());
        this->log->writeLog(logBuffer);
        sendData(logBuffer);
        return;
    }

    sendData("OK");

    // Get the size of the file
    file.seekg(0, std::ios::end);
    int fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Calculate the number of chunks and send it to server
    int numChunks = (fileSize + CHUNK_SIZE - 1) / CHUNK_SIZE;
    write(newSocket, &numChunks, sizeof(numChunks));

    // Send the file in chunks
    char buffer[CHUNK_SIZE];
    int nBytes;
    for (int i = 0; i < numChunks; ++i) {

        // Clear buffer
        memset(buffer, 0, sizeof(buffer));

        // Read a chunk from the file and send over the network
        file.read(buffer, sizeof(buffer));
        nBytes = write(newSocket, buffer, sizeof(buffer));
        if (nBytes < 0)
        {
            throw(int)5;
        }
    }

    file.close();
    std::string logBuffer{ "File " };
    logBuffer.append(fileName);
    logBuffer.append(" sended to user ");
    logBuffer.append(getCurrentUser());
    this->log->writeLog(logBuffer);

}

void Server::receiveFile(std::string fileName) {

    // Creates local file to get the payload of user
    std::string filePath{ "./home/" };
    filePath.append(getCurrentUser());
    filePath.append("/");
    filePath.append(fileName);

    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error creating file: " << filePath << std::endl;
        throw(int)1;
    }

    // Receive the total number of chunks
    int numChunks;
    read(newSocket, &numChunks, sizeof(numChunks));

    char buffer[CHUNK_SIZE];
    int nBytes;
    // Receive and write each chunk to the file
    for (int i = 0; i < numChunks; ++i) {

        // Clean the buffer and gets a chunk
        memset(buffer, 0, sizeof(buffer));
        nBytes = read(newSocket, buffer, sizeof(buffer));

        if (nBytes < 0) {
            throw(int)6;
        }

        // Write the chunk to the file
        file.write(buffer, nBytes);
    }
    file.close();

    // Write on log
    std::string logBuffer{ "File " };
    logBuffer.append(fileName);
    logBuffer.append(" received from user ");
    logBuffer.append(getCurrentUser());
    this->log->writeLog(logBuffer);
}

void Server::parseCliMsg() {
    std::string s{ this->buffer };              // Converts char buffer into string
    std::istringstream iss(s);                  // Then converts string into iss stream
    std::string protocol, command, arg;         // Separate protocol, command and argument
    iss >> protocol; iss >> command; iss >> arg;

    if (command == "hello") {
        setCurrentUser(arg);
        checkUserDir();
        helloClient();
    }
    else if (command == "list" || command == "ls") {
        listFiles();
    }
    else if (command == "quit" || command == "exit") {
        closeConnection();
    }
    else if (command == "remove" || command == "rm") {
        removeFile(arg);
    }
    else if (command == "get") {
        sendFile(arg);
    }
    else if (command == "upload") {
        receiveFile(arg);
    }
}

void Server::handleCliComm() {
    // Reset buffer
    memset(this->buffer, 0, BUFFER_SIZE);

    int nBytes;
    for (;!this->quit;) {
        // Clear buffer and read from socket
        memset(buffer, 0, BUFFER_SIZE);
        nBytes = read(this->newSocket, buffer, BUFFER_SIZE);

        if (nBytes < 0) {
            this->log->writeLog("Error while reading from socket");
        }

        try {
            parseCliMsg();
        }
        catch (int& error) {
            if (error == 1) {
                std::string logBuffer{ "Error creating file for user " };
                logBuffer.append(getCurrentUser());
                this->log->writeLog(logBuffer);
            }
        }
    }

    close(this->newSocket);
}