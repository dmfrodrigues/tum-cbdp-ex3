#ifndef WORKER_H
#define WORKER_H

#include <sstream>

class Worker {
    private:
        static const int NUMBER_RETRIES_CONNECT = 10;
        static const useconds_t SLEEP_MICROS = 100000;
        const int coordPort;
        int socket_fd;
    public:
        Worker(const std::string &coordName, const int coordPort);
        size_t parseFile(std::stringstream &chunkName);
};

#endif