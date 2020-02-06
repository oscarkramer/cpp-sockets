/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int initListener(const char* portid)
{
   struct sockaddr_in serv_addr;

   // Create socket for this server:
   int sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0)
      error("ERROR opening socket");

   // Establish full server address including port:
   int portno = atoi(portid);
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);

   // Bind the server's socket to the specified port number:
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
      error("ERROR on binding");

   return sockfd;
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

     sockfd = initListener(argv[1]);

     // Start listening:
     listen(sockfd,5);

     // Message received at server socket, establish connection to client's socket:
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0)
        error("ERROR on accept");

     while (1)
     {
        // Clear the input buffer and read the message sent:
        memset(buffer, 0, 256);
        n = recv(newsockfd,buffer,256,0);
        if (n < 0)
        {
           printf("recv returned -1 from port %d.\n", newsockfd);
        }

        // Send acknowledgement back to client:
        printf("Client sent: %s\n",buffer);
        const char* msg = "server: I got your message";
        n = send(newsockfd, msg, strlen(msg), 0);
        if (n < 0)
           error("ERROR writing to socket");
     }
     close(newsockfd);

     close(sockfd);
     return 0;
}
