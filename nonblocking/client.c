#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>

const int PORT = 8080;

int main(int argc, char *argv[]) {
   int sockfd;
   struct sockaddr_in server;
   char server_reply[2000];

   sockfd = socket(PF_INET, SOCK_STREAM, 0);
   fcntl(sockfd, F_SETFL, O_NONBLOCK);

   while 
   select()
    

}
