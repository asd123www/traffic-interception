#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <sys/un.h>

#include <unistd.h> // read(), write(), close()
#define MSG_SIZE 32 * 1024
#define MAX 1024 * 64
#define PORT 8080 


char buff[MAX]; 
#define SA struct sockaddr 
   
// Function designed for chat between client and server. 
void func(int connfd)  {
    int n; 
    // infinite loop for chat 
    for (;;) {
        bzero(buff, MAX); 
        // read the message from client and copy it in buffer 
        uint32_t read_len = read(connfd, buff, sizeof(buff)); 
        // and send that buffer to client 
        write(connfd, buff, read_len); 
    }
}

// Driver function 
int main() {
    int sockfd, connfd, len; 
    // struct sockaddr_in servaddr, cli; 
    struct sockaddr_un server_addr, cli;
   
    // socket create and verification 
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0);
    }

    /* Setup the address structure */
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "asd123www_test_unix_socket");

    if(bind(sockfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_un)) != 0) {
        printf("bind() failed\n");
        return 1;
    }
   
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    }

    len = sizeof(cli);
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, &len); 
    if (connfd < 0) { 
        printf("server accept failed...\n"); 
        exit(0); 
    }
   
    // Function for chatting between client and server 
    func(connfd); 
   
    // After chatting close the socket 
    close(sockfd);
    return 0;
}