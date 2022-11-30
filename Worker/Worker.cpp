#include "Worker.h"

#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <string_view>

#include <cassert>

using namespace std;
using namespace std::literals;

Worker::Worker(const std::string &coordName, const int coordPort) :
   curl(CurlEasyPtr::easyInit())
{
   socket.connect(coordName, coordPort);
   // MessageHeartbeat m(Message::Type::REQUEST);
   // socket.send(&m);
}

void Worker::run() {
   while(true){
      MessageWork *m = dynamic_cast<MessageWork*>(socket.receive()); 
      if (m == nullptr) return;

      const string chunkURL = m->chunkURLs.at(0);
      delete m;

      cout << "[W] Received chunk '" << chunkURL << "'" << endl;

      curl.setUrl(chunkURL);
      std::stringstream ss = curl.performToStringStream();
      size_t result = processChunk(ss);

      MessageWork response(Message::Type::RESPONSE);
      response.result = result;
      response.chunkURLs = vector<string>({chunkURL});
      socket.send(&response);

      cout << "[W] Sent response of chunk '" << chunkURL << "'" << endl;

      sched_yield();
   }
}

size_t Worker::processChunk(std::stringstream &chunkName) {
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
