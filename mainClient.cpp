#include <iostream>
#include "Client.hpp"

int main(int argc, char* argv[]) {


    // Check command line arguments
    if (argc != 3) {
        std::cout << std::endl << "Client could not start properly." << std::endl;
        std::cout << "Please specify a port and an address to connect." << std::endl << std::endl;
        std::cout << "Make sure that 1023 < port < 49152 and address is reachable." << std::endl << std::endl;
        std::cout << "./client <port> <address>" << std::endl << std::endl;
        exit(1);
    }

    // Create an instance of a client.
    Client* c{ nullptr };
    try {
        c = new Client{ argv[1], argv[2] };
    }
    catch (int& error) {
        if (error == 1) {
            std::cout << "FAILED to Connect. Invalid port range at command line." << std::endl;
            std::cout << "Make sure that port is a number between [1023.. 49152]." << std::endl;
        }
        else if (error == 2) {
            std::cout << "FAILED to Connect. Error while opening socket.";
        }
        else if (error == 3) {
            std::cout << "FAILED to Connect. Error resolving address.";
        }
        else if (error == 4) {
            std::cout << "FAILED to Connect. Error while opening a connection on socket file descriptor.";
        }
        delete(c);
        exit(1);
    }

    c->commWithServer();

    delete(c);

    return 0;
}