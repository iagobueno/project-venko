#include "UserInterface.hpp"
#include <iostream>
#include <sstream>

void UserInterface::help() {
    std::cout << std::endl << "The following commands are available:" << std::endl;
    std::cout << "* list" << std::endl;
    std::cout << "* remove <fileName>" << std::endl;
    std::cout << "* get <fileName>" << std::endl;
    std::cout << "* upload <fileName>" << std::endl;
    std::cout << "* help" << std::endl << std::endl;
    std::cout << "You're allowed to do the following in your <fileName>:" << std::endl;
    std::cout << "* use alphabetic characters a-z" << std::endl;
    std::cout << "* use numeric characters 0-9" << std::endl;
    std::cout << "* use a name with a maximum length of 32" << std::endl << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "  upload file12" << std::endl << std::endl;
}

void UserInterface::greetings() {
    std::cout << std::endl << "Hello, welcome to Venko Project 1.0.0. server..." << std::endl;
    std::cout << "Please specify the operation you want to perform." << std::endl;
    std::cout << "If you're lost, type \"help\" and hit Enter." << std::endl << std::endl;
}

void UserInterface::prompt(std::string userName, std::string serverName) {
    std::cout << "[" << userName << "@" << serverName << "]$ ";
}

std::string UserInterface::getBuffer() {
    return this->buffer;
}

bool UserInterface::isAlfaNum(std::string str) {
    for (char ch : str) {
        if (!std::isalnum(ch)) {
            return false;
        }
    }
    return true;
}

int UserInterface::readOpt() {
    // read the whole line
    std::string inputBuffer;
    std::getline(std::cin, inputBuffer);

    // Checks if there is any special character
    if (!isAlfaNum(inputBuffer)) {
        throw(int)1;
    }

    // extract first(command) and second word (possible fileName)
    std::istringstream iss(inputBuffer);
    std::string command, fileName;
    iss >> command;
    iss >> fileName;

    this->buffer = fileName;

    if (command == "list" || command == "ls") {
        if (fileName != "") {
            throw(int)2;
        }
        return 1;
    }
    else if (command == "remove" || command == "rm") {
        return 2;
    }
    else if (command == "get") {
        return 3;
    }
    else if (command == "upload") {
        return 4;
    }
    return 0;
}