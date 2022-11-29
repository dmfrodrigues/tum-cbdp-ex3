#include "Socket.h"

#include <netdb.h>
#include <sys/un.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

MessageFactory Socket::messageFactory = MessageFactory();

Socket::Socket() {}

Socket::Socket(int fd) : fd(fd) {
}

void Socket::init(const string &name, int port) {
   addrinfo hints{};
   memset(&hints, 0, sizeof(addrinfo));

   hints.ai_family = AF_INET;
   hints.ai_addrlen = sizeof(struct sockaddr_in);

   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = IPPROTO_TCP;

   if (getaddrinfo(name.c_str(), to_string(port).c_str(), &hints, &req) != 0) {
      throw runtime_error("getaddrinfo() failed");
   }

   fd = socket(req->ai_family, req->ai_socktype, req->ai_protocol);
   if (fd == -1) {
      throw runtime_error("socket() failed");
   }

   // allow kernel to rebind address even when in TIME_WAIT state
   int yes = 1;
   if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
      throw runtime_error("setsockopt() failed");
   }
}

void Socket::bind(const string &name, int port) {
   init(name, port);

   if (::bind(fd, req->ai_addr, req->ai_addrlen) == -1) {
      throw runtime_error("perform_bind() failed");
   }

   if (listen(fd, BACKLOG) == -1) {
      throw runtime_error("perform_listen() failed");
   }
}

Socket Socket::accept() {
   int new_fd = ::accept(fd, nullptr, nullptr);
   return Socket(new_fd);
}

void Socket::connect(const string &name, int port) {
   init(name, port);

   bool connected = false;
   int i = 0;
   while (!connected && i < NUMBER_RETRIES_CONNECT) {
      if (::connect(fd, req->ai_addr, req->ai_addrlen) == -1) {
         perror("connect() failed");
         usleep(SLEEP_MICROS);
         ++i;
      } else {
         connected = true;
      }
   }

   if (!connected)
      throw runtime_error("connect() failed");
}

void Socket::send(const Message *m){
   string msg = m->serialize();
   const size_t &sz = msg.size();
   write(fd, &sz, sizeof(sz));
   write(fd, msg.c_str(), sz);
}

Message* Socket::receive(){
   size_t sz;
   read(fd, &sz, sizeof(sz));
   char buf[Message::MAX_SIZE];
   read(fd, buf, sz);
   buf[sz] = '\0';
   string bufStr(buf);
   return messageFactory.factoryMethod(bufStr);
}

Socket::~Socket(){
   freeaddrinfo(req);
}

