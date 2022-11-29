#pragma once

#include "../Message/Message.h"
#include "../Message/MessageFactory.h"

#include <string>
#include <netdb.h>

class Socket {
private:
    static const int BACKLOG = 10;
    static const int NUMBER_RETRIES_CONNECT = 10;
    static const useconds_t SLEEP_MICROS = 100000;

    static MessageFactory messageFactory;

    void init(const std::string &name, int port);

    addrinfo *req = nullptr;
    int fd;

public:
    Socket();
    Socket(int fd);

    void bind(const std::string &name, int port);
    void connect(const std::string &name, int port);

    Socket accept();

    void send(const Message *m);
    Message* receive();

    ~Socket();
};
