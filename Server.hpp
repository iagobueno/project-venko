#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>             // socket(), socklen_t
#include <netinet/in.h>             // sockaddr_in

#include "ServerLog.hpp"

class Server {

public:
    Server(const char* port);
    ~Server();

    void setPort(const int port);
    int getPort();

    void acceptNewSession();
    void closeNewSocket();
    void closeMaster();
    void handleCliComm(ServerLog* log);

    constexpr static int MAX_HOST_NAME{ 50 };
    constexpr static int MAX_CONNECTIONS{ 3 };
    constexpr static int BUFFER_SIZE{ 256 };

private:
    int port;                       // port number
    int masterSocket;               // socket to listen for incoming connections
    int newSocket;                  // socket created to communicate with new clients
    struct sockaddr_in servAddr;    // Structure to server address information
    struct sockaddr_in cliAddr;     // Structure to client address information
    socklen_t cliLength;            // The size of the client address structure
    char buffer[256];

    bool checkPort(const int port);
};

#endif