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

using namespace std;

Coordinator::Coordinator(const std::string &name, const int port) : port(port) {
   socket.bind(name, port);
   cout << "Coordinator, " << name << " " << port << endl;
}

void Coordinator::run(){
   while(true){
      Socket workerSocket = socket.accept();
      cout << "Coordinator: accepted connection" << endl;
      Message *m = workerSocket.receive();
      cout << "Coordinator: got message, operation=" << static_cast<int>(m->operation) << endl;
      cout << "L27" << endl;
   }
}

size_t Coordinator::parseFile(std::string listUrl) {
   // TODO:
   //    1. Allow workers to connect
   //       socket(), bind(), listen(), accept(), see: https://beej.us/guide/bgnet/html/#system-calls-or-bust
   //    2. Distribute the following work among workers
   //       send() them some work
   //    3. Collect all results
   //       recv() the results
   // Hint: Think about how you track which worker got what work
   struct sockaddr_un their_addr;
   socklen_t addr_size = sizeof(their_addr);

   int new_fd = accept(socket_fd, (struct sockaddr *)&their_addr, &addr_size);

   std::cout << "test" << new_fd << std::endl;
   auto curlSetup = CurlGlobalSetup();

   // Download the file list
   auto curl = CurlEasyPtr::easyInit();
   curl.setUrl(listUrl);
   std::stringstream fileList = curl.performToStringStream();

   //size_t result = 0;
   // Iterate over all files
   for (std::string url; std::getline(fileList, url, '\n');) {
      curl.setUrl(url);
      // Download them

      std::stringstream csvData = curl.performToStringStream();
      //Worker w = Worker();
      //result = result + w.parseFile(csvData);
   }

   return 0;
}
