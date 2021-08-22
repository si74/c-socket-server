#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

const int PORT = 8080;

int main() {
   extern int errno;
   int errnum;
   int sock;
   int status;
   struct sockaddr_in server;
   char server_reply[2000];
   fd_set master, writefds, readfds;

   /* 
    * For some reason SOCKSTREAM | O_NONBLOCK doesn't work
    * despite what the man page says (https://man7.org/linux/man-pages/man2/socket.2.html).
    * Hence, using fcntl() instead.
    */
   sock = socket(AF_INET, SOCK_STREAM, 0);
   if (sock == -1) {
     perror("socket error");
     return 1;
   }
   status = fcntl(sock, F_SETFL, O_NONBLOCK);
   if (status < 0) {
      perror("error setting socket to nonblocking");
      return 1;
   }
   printf("non-blocking socket created\n");

   server.sin_addr.s_addr = inet_addr("127.0.0.1");
   server.sin_family = AF_INET;
   server.sin_port = htons(PORT);

   if (connect (sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
      errnum = errno;
      // TODO(sneha): check type of error - i.e. in progress and continue if that is the case
      if (errno == EINPROGRESS) {
         printf("operation still in progress. continuing...");
      } else {
         perror("connect failed\n");
         return 1;
      }
   }
   printf("connected\n");

   // add to fd set
   FD_SET(sock, &master);
   int fd_max = sock;
   int counter = 0;

   // check for writes
   for (;;) {
      counter++;
      writefds = master; // copy master set
      readfds = master;
      if (select(fd_max+1, NULL, &writefds, NULL, NULL) == -1) {
         errnum = errno;
         perror("listen");
         fprintf(stderr, "errno: %s\n", strerror(errnum));
         exit(4);
      }

      // write to sock in write set 
      if (FD_ISSET(sock, &writefds)) {
         break; 
      }
   }

   // send message to server
   char* msg = "Hello, world from client!\n";
   int len = strlen(msg);
   if (send(sock, msg, len, -1) == -1) {
      perror("send");
   }

   // check for reads
   counter = 0;
   for (;;) {
      counter++;
      writefds = master; // copy master set
      readfds = master; 

      if (select(fd_max+1, &readfds, NULL, NULL, NULL) == -1) {
         errnum = errno;
         perror("listen");
         fprintf(stderr, "errno: %s\n", strerror(errnum));
         exit(4);
      }

      // read from sock in read set
      if (FD_ISSET(sock, &readfds)) {
         break;
               } 
   }

   // read reply from server
   int recv_len = recv(sock, server_reply, 2000, 0);
   if (recv_len < 0) {
      printf("recv failed\n");
      return 1;
   }
   printf("Server reply: %s\n", server_reply);
   printf("Reply len: %d\n", recv_len);
   close(sock);
   return 0;
 
} // main()

