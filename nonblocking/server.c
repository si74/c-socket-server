#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

const int PORT = 8080;
const int STDIN = 0;

int main(int argc, char *argv[]) {
   int sockfd;
   struct timeval tv;
   struct sockaddr_in server;
   char server_reply[2000];

   fd_set readfds;
   fd_set master;
   int listenfd;
   int newfd;
   struct sockaddr_storage remoteaddr;

   // select will block for 2.5s
   tv.tv_sec = 2;
   tv.tv_usec = 500000;

   int yes = 1;
   int i, j, rv;

   FD_ZERO(&readfds); // clear all entries from set
   FD_ZERO(&master);
   FD_SET(STDIN, &readfds); // add fd to the set 

   // listenfd
   struct addrinfo hints; // properties of listenfd
   struct addrinfo* ai;
   struct addrinfo* p;
   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_INET; // ivp4 addresses
   hints.ai_socktype = SOCK_STREAM; // TCP
   hints.ai_flags = AI_PASSIVE; // accepting connections

   for (p = ai; p != NULL; p = p->ai_next) {
      listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      if (listenfd < 0) {
         continue;
      }
      setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
      if (bind(listenfd, p->ai_addr, p->ai_addrlen) < 0) {
         close(listenfd);
	 continue;
      }
      break;
   }   
   freeaddrinfo(ai);

   // listen
   if (listen(listenfd, 10) == -1) {
      perror("listen");
      exit(3);
   }

   FD_SET(listenfd, &master); // add listener to master set

   int fd_max = listenfd;

   readfds = master; // copy master set
   select(fd_max+1, &readfds, NULL, NULL, NULL); // pull ready fd
   for (int i = 0; i < fd_max; i++) {
      printf("loop itr: %d\n", i);
      if (!FD_ISSET(i, &readfds)) { 
         continue;
      }
      if (i != listenfd) { // new listener
         continue;
      } // todo old listener
      int addrlen = sizeof remoteaddr;
      newfd = accept(listenfd, (struct sockaddr*)&remoteaddr, &addrlen);
      if (newfd == -1) {
         perror("accept");
	 continue;
      }
      FD_SET(newfd, &master); // add to master set
      if (newfd > fd_max) {
         fd_max = newfd;
      }

//      printf("select server: new connection from %s on socket %d\n", 
//		      inet_ntop(remoteaddr.ss_family, 
//	              get_in_addr((struct sockaddr*)&remoteaddr), 
//		      remoteIP, INET_ADDRSTRLEN), newfd);

   } // for 
   return 0;
}
