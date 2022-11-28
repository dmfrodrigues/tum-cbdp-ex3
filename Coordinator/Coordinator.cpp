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
   addrinfo hints{}, *req = nullptr;
   memset(&hints, 0, sizeof(addrinfo));

   hints.ai_family = AF_INET;
   hints.ai_addrlen = sizeof(struct sockaddr_in);

   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = IPPROTO_TCP;

   cout << name << " " << port << endl;

   if (getaddrinfo(name.c_str(), std::to_string(port).c_str(), &hints, &req) != 0) {
      throw std::runtime_error("getaddrinfo() failed");
   }

   socket_fd = socket(req->ai_family, req->ai_socktype, req->ai_protocol);
   if (socket_fd == -1) {
      throw std::runtime_error("socket() failed");
   }

   // allow kernel to rebind address even when in TIME_WAIT state
   int yes = 1;
   if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
      throw std::runtime_error("setsockopt() failed");
   }

   if (bind(socket_fd, req->ai_addr, req->ai_addrlen) == -1) {
      throw std::runtime_error("perform_bind() failed");
   }

   if (listen(socket_fd, BACKLOG) == -1) {
      throw std::runtime_error("perform_listen() failed");
   }

   free(req);
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
