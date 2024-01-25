#include "ServerLog.hpp"

#include <ctime>
#include <iostream>
#include <fstream>
#include <string>

ServerLog::ServerLog()
    : mode{ 0 }, pid{ getpid() }, t{ std::time(nullptr) } {

    // abre servidor em modo append
    this->logFile.open("./log/server.log", std::ios::app);
    if (!logFile.is_open()) {
        throw(int)1;
    }
}

ServerLog::~ServerLog() {
    this->logFile.close();
}

void ServerLog::setMode(const int m) {
    this->mode = m;
}

int ServerLog::getMode() {
    return this->mode;
}

int ServerLog::getPid() {
    return this->pid;
}

void ServerLog::Help() {
    std::cout << std::endl << "Server could not start properly." << std::endl;
    std::cout << "Please specify a port to run." << std::endl;
    std::cout << "Make sure that 1023 < port < 49152." << std::endl << std::endl;
    std::cout << "./server <port>" << std::endl << std::endl;
    std::cout << "The following flag can be enabled:" << std::endl;
    std::cout << "* quiet | Does not show any output at terminal." << std::endl << std::endl;
    std::cout << "./server <port> quiet" << std::endl << std::endl;
}

std::string ServerLog::getTime() {
    std::time_t result = std::time(nullptr);
    char buffer[80]; // Buffer to store the formatted time string
    std::strftime(buffer, sizeof(buffer), "%a %b %d %T %Y", std::localtime(&result));

    return buffer;
}

void ServerLog::writeLog(std::string msg) {
    msg.append(".");
    if (!getMode()) {
        std::cerr << msg << std::endl;
    }
    this->logFile << getTime() << " Venko server[" << getPid() << "]: " << msg << std::endl;
}