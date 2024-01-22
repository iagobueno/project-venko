#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <string>

class UserInterface {
public:
    UserInterface() = default;
    ~UserInterface() = default;

    void greetings();
    std::string getUserName();
    void prompt(std::string userName, std::string serverName);
    void help();
    std::string readOpt();
    std::string getBuffer();

    constexpr static int MAX_USER_NAME{ 32 };

private:
    bool isAlfaNum(std::string str);
    std::string getFirstWord(const std::string& str);
    std::string buffer;
    bool verifyUserName(const std::string uname);
};

#endif