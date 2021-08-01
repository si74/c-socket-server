#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

const int PORT = 8080;

int main(int argc, char *argv[]) {
   extern int errno;
   int errnum;
   int sock;
   int status;
   struct sockaddr_in server;
   char server_reply[2000];
   fd_set master, readfds;

   char message[1000] = "Hi";

// TODO(sneha): create parent fd set

   // For some reason SOCKSTREAM | O_NONBLOCK doesn't work
   // despite what the man page says (https://man7.org/linux/man-pages/man2/socket.2.html).
   // Hence, using fcntl() instead.
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

   // TODO(sneha): Add to fd set
   FD_SET(sock, &master);
   int fd_max = sock;
   int counter = 0;
   // TODO(sneha: Do select in loop to see if socket is connected/ready
   for (;;) {
      if (counter < 10)
         printf("in loop %d\n", counter);
      counter++;
      readfds = master; // copy master set

      if (select(fd_max+1, &readfds, NULL, NULL, NULL) == -1) {
         errnum = errno;
         perror("listen");
         fprintf(stderr, "errno: %s\n", strerror(errnum));
         exit(4);
      }
      printf("select isnt blocking\n"); 
      // when fd ready, check sockopt for errors
      if (FD_ISSET(sock, &readfds)) {
         printf("sock is in readfds\n");
         break;
      }
      if (counter < 10)
         printf("not ready\n");
   }

   // send message
   if (send(sock, "Hello, world!", 13, 0) == -1) {
      perror("send");
   }


   printf("sendng message\n");
   if (send(sock, message, strlen(message), 0) < 0) {
      printf("send failed\n");
      return 1;
   }

   printf("receiving message\n");
   if (recv(sock, server_reply, 2000, 0) < 0) {
      printf("recv failed\n");
      return 1;
   }

   printf("Server reply: %s\n", server_reply);
   close(sock);
   return 0;
}

// connect is blocking
// set socket to non blocking
// add fd to master
// connect
// blocking select in for loop
// when fd is ready, check sockopt for errors
