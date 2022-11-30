#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <string>
#include <map>
#include <list>
#include <vector>
#include <set>

#include <sys/poll.h>


#include "../Socket/Socket.h"


struct workerDetails {
   std::list<std::string> work;
   Socket socket;
};
typedef struct workerDetails workerDetails;

class Coordinator {
private:
   Socket socket;
   std::map<int, workerDetails> workers;
   std::vector<struct pollfd> pollSockets;
   std::stringstream chunks;

   size_t totalResults;
   int unfinishedChunks;

   void processWorkerResult(int sd);
   void sendWork(int sd);
   void acceptConnection();
   void loop();

public:
   Coordinator(const std::string& name, const int p);
   size_t processFile(const std::string listUrl);
};

#endif