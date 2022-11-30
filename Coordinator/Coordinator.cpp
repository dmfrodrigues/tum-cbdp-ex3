#include "Coordinator.h"
#include "../CurlEasyPtr.h"
#include "../Worker/Worker.h"

#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <sys/un.h>
#include <cassert>
#include <algorithm>


using namespace std;

Coordinator::Coordinator(const std::string &name, const int port) {
   socket.bind(name, port);
   #ifdef LOG
   cout << "[C] " << name << " " << port << endl;
   #endif

   // pollSockets.reserve(20);

   // Build first pollfd to the listening connection
   struct pollfd pfd = {};
   pfd.fd = socket.getSd();
   pfd.events = POLLIN;
   pollSockets.push_back(pfd);
}

void Coordinator::acceptConnection() {
   Socket worker = socket.accept();
   
   // Add entry to the workers map
   workerDetails wd = {};
   wd.socket = worker;
   wd.work = {list<string>(),};
   workers.insert({worker.getSd(), wd});

   #ifdef LOG
   cout << "[C] " << "Accepted worker with sd " << worker.getSd() << endl;
   #endif

   // Add worker pollfd
   struct pollfd pfd = {};
   pfd.fd = worker.getSd();
   pfd.events = POLLIN;
   pollSockets.push_back(pfd);

   sendWork(worker.getSd());
}

void Coordinator::sendWork(int sd) {
   if (chunks.eof()) return;
   
   string nextChunk;
   getline(chunks, nextChunk, '\n');
   
   if (nextChunk.empty()) return;
   
   unfinishedChunks += 1;

   workerDetails &wd = workers.at(sd);
   wd.work.push_back(nextChunk);

   MessageWork m(Message::Type::REQUEST);
   m.chunkURLs = vector<string>({nextChunk});
   wd.socket.send(&m);

   #ifdef LOG
   cout << "[C] Dispatched chunk '" << nextChunk << "' to worker " << sd << endl;
   #endif
}

void Coordinator::processWorkerResult(int sd) {
   workerDetails &wd = workers.at(sd);

   MessageWork *m = dynamic_cast<MessageWork*>(wd.socket.receive()); 
   assert(m != nullptr);

   auto it = find(wd.work.begin(), wd.work.end(), m->chunkURLs.at(0));
   if (it == wd.work.end()) {
      cerr << "[C] Worker " << sd << " returned unexpected chunk '" << m->chunkURLs.at(0) << "'" << endl;
      delete m;
      return;
   }

   wd.work.erase(it);
   totalResults += m->result;
   unfinishedChunks -= 1;

   #ifdef LOG
   cout << "[C] Worker " << sd << " processed chunk '" << m->chunkURLs[0] << "' with result " << m->result << endl;
   #endif

   delete m;

   sendWork(sd);
}

void Coordinator::loop() {

   int rc = poll(pollSockets.data(), pollSockets.size(), -1);
   if (rc < 0) {
      perror("  poll() failed");
      return;
   }

   bool workersAwaitingConnection = false;
   for (const auto &element : pollSockets) {
      if (element.revents == POLLIN)
      {
         if (element.fd == socket.getSd()) {
            // Worker attempting to connect
            workersAwaitingConnection = true;
         } else {
            // Worker socket input
            processWorkerResult(element.fd);
         }
      } else if (element.revents == POLLHUP) {
         // Process peer closed socket

      } else {
         // Process critital error

      }
   }

   if (workersAwaitingConnection) acceptConnection();
   
}

void Coordinator::cleanup() {
   // TODO:
}

size_t Coordinator::processFile(std::string listUrl) {
   // TODO:
   //    1. Allow workers to connect
   //       socket(), bind(), listen(), accept(), see: https://beej.us/guide/bgnet/html/#system-calls-or-bust
   //    2. Distribute the following work among workers
   //       send() them some work
   //    3. Collect all results
   //       recv() the results
   // Hint: Think about how you track which worker got what work

   totalResults = 0;
   unfinishedChunks = 0;
   
   // Download the file list
   auto curl = CurlEasyPtr::easyInit();
   curl.setUrl(listUrl);
   chunks = curl.performToStringStream();


   do { // Until result
      loop();
   } while ((unfinishedChunks > 0 || !chunks.eof()));

   // Cleanup
   cleanup();

   #ifdef LOG
   cout << "[C] Finished processing file, found " << totalResults << " matches" << endl;
   #endif

   return totalResults; // TODO: retunr result
}
