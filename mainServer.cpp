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
        }
        else if (e == 2) {
            log->writeLog("Server FAILED to Start. Error while opening socket.");
        }
        else if (e == 3) {
            log->writeLog("Server FAILED to Start. Error getting hostname.");
        }
        else if (e == 4) {
            log->writeLog("Server FAILED to Start. Error resolving hostname.");
        }
        else if (e == 5) {
            log->writeLog("Server FAILED to Start. Error binding address to socket.");
        }
        else if (e == 6) {
            log->writeLog("Server FAILED to Start. Error while listening on master socket.");
        }
        else {
            log->writeLog("Server FAILED to Start. Unknown Error.");
        }

        delete log;
        delete s;
        exit(1);
    }

    std::string logBuffer{ "Server STARTED and is listening on port " };
    logBuffer.append(std::to_string(s->getPort()));
    logBuffer.append(".");
    log->writeLog(logBuffer);

    int pid;
    for (;;) {

        // Server keeps awaiting a new connection
        try {
            s->acceptNewSession();
        }
        catch (int& e) {
            if (e == 1) {
                log->writeLog("ERROR accepting new connection.");
                delete(log);
                delete(s);
                exit(1);
            }
        }

        log->writeLog("Server Accepted a new connection!");

        // Creates a new process to handle new session with client
        pid = fork();
        if (pid < 0) {
            log->writeLog("ERROR While creating a new process. Server stopped.");
            exit(2);
        }

        // Child process handles communication with new client
        if (pid == 0) {
            s->closeMaster();
            s->handleCliComm(log);
            log->writeLog("Client CLOSED connection with Server.");
            exit(0);
        }

        // Parent process goes back to accept new connections
        else {
            s->closeNewSocket();
        }
    }

    delete log;
    delete s;
    return 0;
}