#ifndef SERVER_H
#define SERVER_H

class Server {

public:
    Server(const char* port);
    void setPort(const int port);

private:
    int port;
    int masterSocket;
    int incSocket;

    bool checkPort(const int port);

};

#endif