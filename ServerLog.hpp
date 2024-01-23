#ifndef SERVERLOG_H
#define SERVERLOG_H

#include <fstream>
#include <string>

#include <sys/types.h>
#include <unistd.h>
#include <ctime>

class ServerLog {

public:
    ServerLog();
    ~ServerLog();

    void Help();
    void writeLog(std::string msg);

    void setMode(const int m);
    int getMode();
    pid_t getPid();
    std::string getTime();

private:
    int mode;
    pid_t pid;
    std::time_t t;
    std::ofstream logFile;
};

#endif