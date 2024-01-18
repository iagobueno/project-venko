#include <iostream>

#include "Server.hpp"
#include "ServerLog.hpp"
#include <string.h>
#include <fstream>

int main(int argc, char* argv[]) {

    // Create an instance of ServerLog class and open try to open Log file
    ServerLog* log{ nullptr };
    try {
        log = new ServerLog{};
    }
    catch (const std::ofstream::failure& e) {
        std::cout << "Exception opening/reading ./log/server.log" << std::endl;
    }

    // Check number of command line arguments
    if (argc < 2 || argc > 3) {
        log->Help();
        delete log;
        exit(1);
    }

    // Check the quiet flag, so the terminal will not output anything
    if (argv[2] != NULL) {
        if (!strcmp(argv[2], "quiet")) {
            log->setMode(1);
        }
        else {
            log->writeLog("Server FAILED to Start. Invalid flag at command line.");
            delete log;
            exit(1);
        }
    }

    // Create an instance of a server.
    Server* s{ nullptr };
    try {
        s = new Server{ argv[1] };
    }
    catch (int& e) {
        if (e == 1) {
            log->writeLog("Server FAILED to Start. Invalid port range at command line.");
            std::cout << "Make sure that port is a number between [1023.. 49152]." << std::endl;
            delete log;
            delete s;
            exit(1);
        }
    }



    delete log;
    delete s;
    return 0;

}