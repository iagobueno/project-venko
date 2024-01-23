#include "UserInterface.hpp"
#include <iostream>
#include <climits>          // INT_MAX
#include <sstream>

void UserInterface::help() {
    std::cout << std::endl << "The following commands are available:" << std::endl;
    std::cout << "* get <fileName>" << std::endl;
    std::cout << "* upload <fileName>" << std::endl;
    std::cout << "* remove <fileName>" << std::endl;
    std::cout << "* list" << std::endl;
    std::cout << "* help" << std::endl;
    std::cout << "* exit" << std::endl << std::endl;
    std::cout << "You're allowed to do the following in your <fileName>:" << std::endl;
    std::cout << "* use alphabetic characters a-z" << std::endl;
    std::cout << "* use numeric characters 0-9" << std::endl;
    std::cout << "* use the special chars - and ." << std::endl;
    std::cout << "* use a name with a maximum length of 32" << std::endl << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "  upload file12.pdf" << std::endl << std::endl;
}

void UserInterface::greetings() {
    std::cout << std::endl << "Hello, welcome to Venko Project 1.0.0. server..." << std::endl;
    std::cout << "Please specify the operation you want to perform." << std::endl;
    std::cout << "If you're lost, type \"help\" and hit Enter." << std::endl << std::endl;
}

bool UserInterface::verifyUserName(const std::string uname) {

    // if is above max user name chars
    if (uname.length() == 0 || uname.length() > MAX_USER_NAME) {
        return false;
    }

    // if there is any special char
    for (char ch : uname) {
        if (!std::isalnum(ch)) {
            return false;
        }
    }
    return true;
}

std::string UserInterface::getUserName() {
    std::cout << std::endl << "Please specify a username." << std::endl;
    std::cout << "You're allowed to do the following in your username:" << std::endl;
    std::cout << "* use alphabetic characters a-z" << std::endl;
    std::cout << "* use numeric characters 0-9" << std::endl;
    std::cout << "* use a name with a maximum length of 32" << std::endl << std::endl;
    std::cout << "username: ";

    std::string uname;
    std::cin >> uname;
    while (!verifyUserName(uname))
    {
        std::cout << "Invalid username! Please specify a valid one." << std::endl << std::endl;
        std::cout << "username: ";
        std::cin >> uname;
    }
    std::cin.ignore(INT_MAX, '\n');

    return uname;
}


void UserInterface::prompt(std::string userName, std::string serverName) {
    std::cout << "[" << userName << "@" << serverName << "]$ ";
}

std::string UserInterface::getBuffer() {
    return this->buffer;
}

bool UserInterface::sanitizeBuffer(std::string str) {

    char ch;
    for (size_t i = 0; i < str.length(); ++i) {
        ch = str[i];
        if (!std::isalnum(ch)) {

            if (ch != '.' && ch != '-') {
                return false;
            }

            if (ch == '.') {
                if (i + 1 < str.length() && str[i + 1] == ch) {
                    return false;
                }
            }
        }

    }
    return true;
}

std::string UserInterface::readOpt() {
    // read the whole line
    std::string inputBuffer;
    std::getline(std::cin, inputBuffer);

    // extract command and possible argument
    std::istringstream iss(inputBuffer);
    std::string command, arg;
    iss >> command;
    iss >> arg;

    // Checks if there is any special character
    if (!sanitizeBuffer(command)) {
        throw(int)1;
    }

    // Checks if there is any special character
    if (!sanitizeBuffer(arg)) {
        throw(int)1;
    }

    return inputBuffer;
}