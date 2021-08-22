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

const int STDIN = 0;

char* get_in_addr(struct sockaddr *sa) {
   if (sa->sa_family == AF_INET) {
      char* ip4 = malloc(INET_ADDRSTRLEN);
      inet_ntop(AF_INET, &(((struct sockaddr_in*)sa)->sin_addr), ip4, INET_ADDRSTRLEN);
      return ip4;
   }
   char* ip6 = malloc(INET6_ADDRSTRLEN);
   inet_ntop(AF_INET6, &(((struct sockaddr_in6*)sa)->sin6_addr), ip6, INET6_ADDRSTRLEN);
   return ip6;
}

int main() {
   int sockfd;
   struct timeval tv;
   struct sockaddr_in server;
   char server_reply[2000];
   char* ret_ip_addr;

   fd_set readfds;
   fd_set master;
   int listenfd;
   int newfd;
   struct sockaddr_storage remoteaddr;

   // relevant metrics being tracked
   int total_messages_received = 0; 
   int total_messages_sent = 0;

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
      listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      fcntl(listenfd, F_SETFL, O_NONBLOCK); //setting to be non-blocking
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
   int counter = 0;

   // main loop
   for (;;) {
      counter++;
      readfds = master; // copy master set
      
      // select waiting for a fd ready for reading
      if (select(fd_max+1, &readfds, NULL, NULL, NULL) == -1) {
         errnum = errno;
         perror("listen");
         fprintf(stderr, "errno: %s\n", strerror(errnum));
         exit(4);
      }

      // check the read
      for (int i = 0; i <= fd_max; i++) {
         // read data from listener
         if (FD_ISSET(i, &readfds)) {
            // new incoming connection
            if (i == listenfd) {
               int addrlen = sizeof remoteaddr;
               newfd = accept(listenfd, (struct sockaddr*)&remoteaddr, &addrlen);
               fcntl(newfd, F_SETFL, O_NONBLOCK); // nonblocking socket
               if (newfd == -1) {
                  errnum = errno;
                  perror("accept");
                  fprintf(stderr, "errno: %s\n", strerror(errnum));

               } else {
                  FD_SET(newfd, &master); // add to master set
                  if (newfd > fd_max) {
                     fd_max = newfd;
                  }
                  ret_ip_addr = get_in_addr((struct sockaddr*)&remoteaddr);
                  printf("select server: new connection from %s on socket %d\n", ret_ip_addr, newfd);
               }

            // data ready to read from client and send back to client (i.e. i != listenfd)
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
	               
                  total_messages_received++;
                  printf("value rcvd, %s\n", buf);
                  char* msg = "Hello, world from server!\n";
                  int len = strlen(msg);
                  if (send(i, msg, len, 0) == -1) {
                     perror("send");
                  }
                  
                  total_messages_sent++;
                  printf("successfully sent\n");
                  printf("total_messages_received: %d\n", total_messages_received);
                  printf("total_messages_sent: %d\n", total_messages_sent);
	            }
	         } 
         } 
      } 
   }

   free(ret_ip_addr);
   return 0;
}
