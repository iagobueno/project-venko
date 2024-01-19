#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <string>

class UserInterface {
public:
    UserInterface() = default;
    ~UserInterface() = default;

    void greetings();
    void prompt(std::string userName, std::string serverName);
    void help();
    int readOpt();
    std::string getBuffer();

private:
    bool isAlfaNum(std::string str);
    std::string getFirstWord(const std::string& str);
    std::string buffer;
};

#endif