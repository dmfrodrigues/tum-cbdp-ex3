#include "Worker/Worker.h"
#include "CurlEasyPtr.h"
#include <iostream>

/// Worker process that receives a list of URLs and reports the result
/// Example:
///    ./worker localhost 4242
/// The worker then contacts the leader process on "localhost" port "4242" for work
int main(int argc, char* argv[]) {
   if (argc != 3) {
      std::cerr << "Usage: " << argv[0] << " <host> <port>" << std::endl;
      return 1;
   }

   CurlGlobalSetup curlSetup;

   // TODO Use dynamic addresses instead. Ta se a ver a pass no runTest.sh, onde usam localhost e nao funfa
   Worker w("127.0.0.1", atoi(argv[2]));
   w.run();

   // TODO:
   //    1. connect to coordinator specified by host and port
   //       getaddrinfo(), connect(), see: https://beej.us/guide/bgnet/html/#system-calls-or-bust
   //    2. receive work from coordinator
   //       recv(), matching the coordinator's send() work
   //    3. process work
   //       see coordinator.cpp
   //    4. report result
   //       send(), matching the coordinator's recv()
   //    5. repeat

   return 0;
}