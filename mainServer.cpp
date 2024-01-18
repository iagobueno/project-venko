#include <iostream>
#include <string>
#include <stdlib.h>         // atoi()
#include <sys/socket.h>     // socket(), socklen_t
#include <netinet/in.h>     // sockaddr_in
#include <string.h>         // memset(), memcpy()
#include <unistd.h>         // gethostname(), read(), write()
#include <netdb.h>          // gethostbyname()
#include <signal.h>         // sigaction

#include "Server.hpp"

#define MAX_HOST_NAME 30
#define MAX_CONNECTIONS 3
#define TIMEOUT_SECS 15

int main(int argc, char* argv[])
{
    // Check command line arguments
    if (argc != 2) {
        std::cout << "[  EXIT  ] Invalid number of arguments. You Must Provide a port." << std::endl;
        exit(1);
    }

    // Gets the port passed through command line
    int port = atoi(argv[1]);
    if (port < 1024 || port > 49151) {
        std::cout << "[  EXIT  ] Invalid port range. Make sure that 1023 < port < 49152." << std::endl;
        exit(-1);
    }

    int masterSocket, newSocket;                // Socket descriptors

    // Creates a Socket using IPv4 and TCP protocol
    masterSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (masterSocket < 0) {
        std::cout << "[  EXIT  ] Error opening the socket." << std::endl;
        exit(2);
    }

    // Gets localhost on hostname
    char hostname[MAX_HOST_NAME];
    if (gethostname(hostname, MAX_HOST_NAME) != 0) {
        std::cout << "[  EXIT  ] Error getting hostname.";
        exit(3);
    }

    /* Resolve the name */
    struct hostent* host;
    if ((host = gethostbyname(hostname)) == NULL)
    {
        std::cout << "[  EXIT  ] Error resolving hostname." << std::endl;
        exit(4);
    }

    struct sockaddr_in servAddr, cliAddr;       // Structure to server/client address information

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;              // IPv4 Family
    servAddr.sin_addr.s_addr = INADDR_ANY;      // Accepts connection on all network interfaces
    servAddr.sin_port = htons(port);
    memcpy(&servAddr.sin_addr, host->h_addr, host->h_length);

    // Binds master socket to address
    if (bind(masterSocket, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        std::cout << "[  EXIT  ] Error binding address to socket." << std::endl;
        exit(5);
    }

    socklen_t cliLength = sizeof(cliAddr);                     // The size of the client address structure
    char buffer[256];
    int nBytes;                                 // Store number o bytes that will be readen or written
    int pid;

    listen(masterSocket, MAX_CONNECTIONS);

    std::cout << "I am listening just fine!" << std::endl;

    // The ideia is to listen till timeout
    for (;;) {

        newSocket = accept(masterSocket, (struct sockaddr*)&cliAddr, &cliLength);
        if (newSocket < 0) {
            std::cout << "[  EXIT  ] Error accepting connection." << std::endl;
            exit(6);
        }

        std::cout << "I accepted the connection!" << std::endl;

        pid = fork();
        if (pid < 0) {
            std::cout << "[  EXIT  ] Error while creating a new process." << std::endl;
            exit(7);
        }

        // Child process handles communication with new client
        if (pid == 0) {
            close(masterSocket);

            int i;
            for (i = 0;i < 2;++i) {
                memset(buffer, 0, 256);
                nBytes = read(newSocket, buffer, 255);

                if (nBytes < 0) {
                    std::cout << "[  EXIT  ] Error while reading from socket." << std::endl;
                    exit(8);
                }

                std::cout << "I receive the message: " << buffer << std::endl;

                nBytes = write(newSocket, "I got your first message", 18);

                if (nBytes < 0)
                {
                    std::cout << "[  EXIT  ] Error while writing from socket." << std::endl;
                    exit(9);
                }
            }

            close(newSocket);
            exit(0);
        }
        // Parent process goes back to accept new connections
        else {
            close(newSocket);
        }

    }

    close(masterSocket);

    return 0;
}