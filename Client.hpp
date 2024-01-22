#ifndef CLIENT_H
#define CLIENT_H

#include <netinet/in.h>                         // sockaddr_in
#include <string>
#include "UserInterface.hpp"

class Client {
public:
    Client(const char* port, const char* serverName);
    ~Client();

    void setPort(const int port);
    int getPort();

    std::string getUserName();
    std::string getServerName();

    void sendData(std::string data);
    std::string recData();

    void helloServer();
    void listFiles();
    void parseInput(std::string inputBuffer);
    void commWithServer();                      // handle all communication with server

    constexpr static int BUFFER_SIZE{ 256 };

private:
    int port;                                   // port number
    int cliSocket;                              // client socket
    struct sockaddr_in servAddr;                // Structure to server address information
    std::string userName;
    std::string serverName;
    UserInterface ui;
    bool quit;

    bool checkPort(const int port);
};

#endif