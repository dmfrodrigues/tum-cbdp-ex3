#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <string>

#include "../Socket/Socket.h"

class Coordinator {
private:
   Socket socket;

public:
   Coordinator(const std::string& name, const int p);
   size_t processFile(const std::string listUrl);

   void run();
};

#endif