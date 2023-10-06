#include <arpa/inet.h> 
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <sys/socket.h> 
#include <sys/time.h>
#include <unistd.h> 

#include "client.h"

int main(int argc, char *argv[]){
    printf("AF_INET: %d SOCK_STREAM: %d SOCK_DGRAM: %d\n", AF_INET, SOCK_STREAM, SOCK_DGRAM);
    char *player = malloc(sizeof(char) * 1024);
    if(argc == 1){
        player = "Sample message\n";
    } else {
        player = argv[1];
    }

    //Player player = { .x = 150, .y = 200 };

    int status, client_fd;
    struct sockaddr_in serv_addr;
    char buffer[1024] = { 0 };
    if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "SOCKET CREATION ERROR\n");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0){
        fprintf(stderr, "Invalid address\n");
        exit(1);
    }

    if((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0){
        fprintf(stderr, "CONNECTION FAILED\n");
        exit(1);
    }

    send(client_fd, player, 1024, 0);
    printf("HELLO MESSAGE SENT\n");
    read(client_fd, buffer, 1024);
    printf("%s\n", buffer);

    close(client_fd);

    return 0;
}
