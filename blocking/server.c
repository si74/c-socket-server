#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

const int PORT = 8080;

/*
 * create socket
 * bind
 * listen
 * accept
*/

int main() {
   int socket_desc , client_sock , c , read_size;
   struct sockaddr_in server , client;
   char client_message[2000];
   
   //Create socket
   socket_desc = socket(AF_INET , SOCK_STREAM , 0);
   if (socket_desc == -1)
   {
   	printf("Could not create socket");
   }
   puts("Socket created");
   
   //Prepare the sockaddr_in structure
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = INADDR_ANY;
   server.sin_port = htons(PORT);
   
   //Bind
   if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
   {
   	//print the error message
   	perror("bind failed. Error");
   	return 1;
   }
   puts("bind done");
   
   //Listen
   listen(socket_desc , 3);
   
   //Accept and incoming connection
   puts("Waiting for incoming connections...");
   c = sizeof(struct sockaddr_in);
  
   while(1) { 
      //accept connection from an incoming client
      client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
      if (client_sock < 0)
      {
      	perror("accept failed");
      	return 1;
      }
      puts("Connection accepted");

      // todo use select to toggle between fd
      if (!fork()) { // this is the child process
         close(socket_desc); // child doesn't need the listener
         if (send(client_sock, "Hello, world!", 13, 0) == -1)
            perror("send");
         close(socket_desc);
         exit(0);
      }
      close(client_sock); // parent doesn't need this

     }
   return 0;
}
