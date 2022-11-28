#include "Coordinator/Coordinator.h"
#include <iostream>

/// Leader process that coordinates workers. Workers connect on the specified port
/// and the coordinator distributes the work of the CSV file list.
/// Example:
///    ./coordinator http://example.org/filelist.csv 4242
int main(int argc, char* argv[]) {
   if (argc != 3) {
      std::cerr << "Usage: " << argv[0] << " <URL to csv list> <listen port>" << std::endl;
      return 1;
   }

   Coordinator coordinator = Coordinator("127.0.0.1", atoi(argv[2]));
   size_t result = coordinator.parseFile(std::string(argv[1]));
   std::cout << result << std::endl;

   return 0;
}
