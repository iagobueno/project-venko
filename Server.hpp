#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>             // socket(), socklen_t
#include <netinet/in.h>             // sockaddr_in

#include "ServerLog.hpp"

class Server {

public:
    Server(const char* port, ServerLog* log);
    ~Server();

    void setPort(const int port);
    int getPort();

    // Socket related functions
    void acceptNewSession();
    void closeNewSocket();
    void closeMaster();

    // Operational related functions
    void helloClient();
    void listFiles();
    void closeConnection();
    void removeFile(std::string fileName);
    void sendFile(std::string fileName);
    void receiveFile(std::string fileName);

    // Network message related functions
    void sendData(std::string data);
    void handleCliComm();
    void parseCliMsg();

    void setCurrentUser(const std::string cUser);
    std::string getCurrentUser();

    // Defines
    constexpr static int MAX_HOST_NAME{ 50 };
    constexpr static int MAX_CONNECTIONS{ 3 };
    constexpr static int BUFFER_SIZE{ 256 };
    constexpr static int CHUNK_SIZE{ 1024 };

private:
    int port;                       // port number
    int masterSocket;               // socket to listen for incoming connections
    int newSocket;                  // socket created to communicate with new clients
    struct sockaddr_in servAddr;    // Structure to server address information
    struct sockaddr_in cliAddr;     // Structure to client address information
    socklen_t cliLength;            // The size of the client address structure
    char buffer[256];
    std::string currentUser;        // Current user on session
    ServerLog* log;                 // ServerLog pointer
    bool quit;                      // It's false till user is connected

    bool checkPort(const int port); // Checks port range
    void checkUserDir();            // Checks if user directory exists and create one if don't
};

#endif