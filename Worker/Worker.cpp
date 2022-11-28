#include "Worker.h"

#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <string_view>

using namespace std;
using namespace std::literals;

Worker::Worker(const std::string &coordName, const int coordPort) : coordPort(coordPort) {
   addrinfo hints{}, *req = nullptr;
   memset(&hints, 0, sizeof(addrinfo));

   hints.ai_family = AF_INET;
   hints.ai_addrlen = sizeof(struct sockaddr_in);

   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = IPPROTO_TCP;

   cout << coordName << " " << coordPort << endl;

   if (getaddrinfo(coordName.c_str(), std::to_string(coordPort).c_str(), &hints, &req) != 0) {
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

   bool connected = false;
   int i = 0;
   while (!connected && i < NUMBER_RETRIES_CONNECT) {
      if (connect(socket_fd, req->ai_addr, req->ai_addrlen) == -1) {
         perror("connect() failed");
         usleep(SLEEP_MICROS);
         ++i;
      } else {
         connected = true;
      }
   }

   if (!connected)
      throw std::runtime_error("connect() failed");

   freeaddrinfo(req);
}

size_t Worker::parseFile(std::stringstream &chunkName) {
   size_t result = 0;
   for (std::string row; std::getline(chunkName, row, '\n');) {
      std::stringstream rowStream = std::stringstream(std::move(row));

      // Check the URL in the second column
      unsigned columnIndex = 0;
      for (std::string column; std::getline(rowStream, column, '\t'); ++columnIndex) {
         // column 0 is id, 1 is URL
         if (columnIndex == 1) {
            // Check if URL is "google.ru"
            size_t pos = column.find("://"sv);
            if (pos != std::string::npos) {
               std::string_view afterProtocol = std::string_view(column).substr(pos + 3);
               if (afterProtocol.starts_with("google.ru/"))
                  ++result;
            }
            break;
         }
      }
   }

   return result;
}
