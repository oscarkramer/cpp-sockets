#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <fstream>
#include "jsoncpp.h"

using namespace std;

#define MAX_BUF_LEN 4096

static char buffer [MAX_BUF_LEN];


void error(const char *msg)
{
   perror(msg);
   exit(0);
}


void connectToServer(char* hostname, char* portname, int& svrsockfd)
{
   // Establish full server address including port:
   struct addrinfo hints;
   memset(&hints, 0, sizeof hints); // make sure the struct is empty
   hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
   hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
   hints.ai_canonname = hostname;     // fill in my IP for me
   struct addrinfo *res;
   int failed = getaddrinfo(hostname, portname, &hints, &res);
   if (failed)
      error(gai_strerror(failed));

   // Create socket for this server by walking the linked list of available addresses:
   struct addrinfo *server_info = res;
   while (server_info)
   {
      svrsockfd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
      if (svrsockfd >= 0)
         break;
      server_info = server_info->ai_next;
   }
   if ((svrsockfd < 0) || (server_info == NULL))
      error("Error opening socket");

   // Connect to the server:portno:
   struct sockaddr_in* serv_addr = (sockaddr_in*) server_info->ai_addr;
   serv_addr->sin_family = AF_INET;
   if (connect(svrsockfd,(struct sockaddr*) serv_addr, server_info->ai_addrlen) < 0)
      error("ERROR connecting");
}


void requestOp(int svrsockfd)
{
   // Fetch message from console:
   printf("Enter message: ");
   memset(buffer, 0, 256);
   fgets(buffer,MAX_BUF_LEN,stdin);

   // Write to the socket:
   int n = send(svrsockfd, buffer, strlen(buffer), 0);
   if (n < 0)
      error("ERROR writing to socket");
   while (n < (int) strlen(buffer))
   {
      // Partial send, try remainder of buffer:
      int r = send(svrsockfd, &(buffer[n]), strlen(buffer)-n, 0);
      if (r < 0)
         error("ERROR writing to socket");
      n += r;
   }
}


int main(int argc, char *argv[])
{

   if (argc < 3)
   {
      fprintf(stderr,"Usage %s hostname port\n", argv[0]);
      exit(0);
   }

   int svrsockfd = 0;
   connectToServer(argv[1], argv[2], svrsockfd);

   while (1)
   {
      requestOp(svrsockfd);

      memset(buffer, 0, MAX_BUF_LEN);
      int n = recv(svrsockfd, buffer, MAX_BUF_LEN, 0);
      if (n < 0)
      {
         error("ERROR reading from socket");
         break;
      }

      printf("%s\n",buffer);
   }

   close(svrsockfd);
   return 0;
}
