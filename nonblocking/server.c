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
#include <errno.h>

#define PORT "8080"
//const int PORT = 8080;
const int STDIN = 0;

void* get_in_addr(struct sockaddr *sa) {
   if (sa->sa_family == AF_INET) {
      return &(((struct sockaddr_in*)sa)->sin_addr);
   }
   return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

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
   int i, rv;

   extern int errno;
   int errnum;

   // client data buffer
   char buf[256];

   int nbytes;

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

   if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
      fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
      exit(1);
   }

   int j = 0;
   for (p = ai; p != NULL; p = p->ai_next) {
      printf("listen for loop itr: %d\n", j++);
      printf("p ai_family: %d\n", p->ai_family);
      listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      printf("set listenfd\n");
      if (listenfd < 0) {
	 errnum = errno;
	 perror("socket error");
         fprintf(stderr, "errno: %s\n", strerror(errnum));
         continue;
      }
      setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
      if (bind(listenfd, p->ai_addr, p->ai_addrlen) < 0) {
	 errnum = errno;
	 perror("bind error");
	 fprintf(stderr, "errno: %s\n", strerror(errnum));
         close(listenfd);
	 continue;
      }
      printf("bind succeeded\n");
      break;
   }   
   freeaddrinfo(ai);

   // listen
   if (listen(listenfd, 10) == -1) {
      errnum = errno;
      perror("listen");
      fprintf(stderr, "errno: %s\n", strerror(errnum));
      exit(3);
   }

   FD_SET(listenfd, &master); // add listener to master set

   int fd_max = listenfd;

   printf("select\n");

   int counter = 0;

   // main loop
   for (;;) { 
      counter++;
      printf("outer loop value: %d\n", counter);
      readfds = master; // copy master set
      // pull ready fd
      if (select(fd_max+1, &readfds, NULL, NULL, NULL) == -1) {
         errnum = errno;
         perror("listen");
         fprintf(stderr, "errno: %s\n", strerror(errnum));
         exit(4); 
      }
      // print readfds 
      for (int i = 0; i <= fd_max; i++) {
         printf("loop itr: %d\n", i);
	 // read data from listener
         if (FD_ISSET(i, &readfds)) {
            printf("fd in readfds\n");
	    if (i == listenfd) {
	       // new listener
	       printf("accept new listener\n");	 
	       int addrlen = sizeof remoteaddr;
               newfd = accept(listenfd, (struct sockaddr*)&remoteaddr, &addrlen);
               if (newfd == -1) {
                  errnum = errno;     
                  perror("accept");
                  fprintf(stderr, "errno: %s\n", strerror(errnum));
               } else {
                  FD_SET(newfd, &master); // add to master set
                  if (newfd > fd_max) {
                     fd_max = newfd;
                  }
		  printf("select server: new connection from %s on socket %d\n", get_in_addr((struct sockaddr*)&remoteaddr), newfd);
               }
	    } else { // i != listenfd
	       char sendBuf[] = "hello world!";
	       nbytes = strlen(sendBuf);
	       printf("i != listenfd\n");
	       if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
	          if (nbytes == 0) {
	             printf("selectserver: socket %d hung up \n", i);
	          } else {
	             perror("recv");
	          }
	          close(i);
	          FD_CLR(i, &master); // rm from master set
	       } else { // valid data from client
		  printf("value rcvd, %s\n", buf);
	          int k;
	          for (k = 0;  k <= fd_max; k++) {
	             printf("entered send loop\n");
	             if (FD_ISSET(k, &master)) {
			printf("fd is set\n");
	                if (k != listenfd && k != i) {
			   printf("start sending sendBuf\n");
	        	   if (send(k, sendBuf, nbytes, 0) == -1) {
	        	      perror("send");
	        	   }
	        	} else {
			   printf("k: %d, listenfd: %d, i: %d\n", k, listenfd, i);
			}
	             } 
	          } // for loop
	       }
	    } // i != listenfd
         } // FD_ISSET(i, &readfds)
      } // select loop
   } // main loop
   return 0;
}
