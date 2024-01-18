#include <iostream>
#include <unistd.h>         // gethostname(), read(), write()
#include <stdlib.h>         // atoi()
#include <sys/socket.h>     // socket(), socklen_t
#include <netdb.h>          // gethostbyname()
#include <string.h>         // memset(), memcpy()

int main(int argc, char* argv[]) {


    // Check command line arguments
    if (argc != 3) {
        std::cout << "[  EXIT  ] Invalid number of arguments. You Must Provide a port and an address to connect." << std::endl;
        std::cout << "Example: ./client <port> <address>" << std::endl;
        exit(1);
    }

    // Gets the port passed through command line
    int port = atoi(argv[1]);
    if (port < 1024 || port > 49151) {
        std::cout << "[  EXIT  ] Invalid port range. Make sure that 1023 < port < 49152." << std::endl;
        exit(2);
    }

    int cliSocket;

    // Creates a Socket using IPv4 and TCP protocol
    cliSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (cliSocket < 0) {
        std::cout << "[  EXIT  ] Error opening the socket." << std::endl;
        exit(3);
    }

    /* Resolve the name */
    struct hostent* server;
    if ((server = gethostbyname(argv[2])) == NULL)
    {
        std::cout << "[  EXIT  ] Error resolving hostname." << std::endl;
        exit(4);
    }

    struct sockaddr_in servAddr;       // Structure to server address information
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;              // IPv4 Family
    servAddr.sin_port = htons(port);
    memcpy(&servAddr.sin_addr, server->h_addr, server->h_length);

    if (connect(cliSocket, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        std::cout << "[  EXIT  ] Error connecting." << std::endl;
        exit(4);
    }

    char buffer[256];

    int i;
    int nBytes;
    for (i = 0;i < 2;++i) {
        memset(buffer, 0, sizeof(buffer));  // reset buffer
        std::cin >> buffer;
        nBytes = write(cliSocket, buffer, strlen(buffer));

        if (nBytes < 0)
        {
            std::cout << "[  EXIT  ] Error while writing from socket." << std::endl;
            exit(9);
        }

        memset(buffer, 0, sizeof(buffer));  // reset buffer

        nBytes = read(cliSocket, buffer, 255);
        if (nBytes < 0) {
            std::cout << "[  EXIT  ] Error while reading from socket." << std::endl;
            exit(8);
        }

        std::cout << "Server: " << buffer << std::endl;
    }

    close(cliSocket);

    return 0;
}