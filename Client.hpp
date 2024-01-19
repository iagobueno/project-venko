#ifndef CLIENT_H
#define CLIENT_H

#include <netinet/in.h>             // sockaddr_in
#include <string>

class Client {
public:
    Client(const char* port, const char* serverName);
    ~Client();

    void setPort(const int port);
    int getPort();

    void commWithServer();

private:
    int port;                       // port number
    int cliSocket;                  // client socket
    struct sockaddr_in servAddr;    // Structure to server address information
    std::string userName;
    std::string serverName;

    bool checkPort(const int port);
};

#endif