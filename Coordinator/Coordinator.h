#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <string>

#include "../Socket/Socket.h"

class Coordinator {
private:
   static const int BACKLOG = 10;
   const int port;
   int socket_fd;

   Socket socket;

public:
   Coordinator(const std::string& name, const int p);
   size_t parseFile(const std::string listUrl);

   void run();
};

#endif