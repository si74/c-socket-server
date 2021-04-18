#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

const int PORT = 8080;

int main(int argc, char *argv[]) {
   int sock;
   struct sockaddr_in server;
   char server_reply[2000];

   char message[1000] = "Hi";

   sock = socket(AF_INET, SOCK_STREAM, 0);
   if (sock == -1) {
      printf("couldn't create socket\n");
      return 1;
   }
   printf("socket created\n");

   server.sin_addr.s_addr = inet_addr("127.0.0.1");
   server.sin_family = AF_INET;
   server.sin_port = htons(PORT);

   if (connect (sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
      perror("connect failed. error\n");
      return 1;
   }
   printf("connected\n");

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
