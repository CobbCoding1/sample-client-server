#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdbool.h>

#include "client.h"

#define BUFFER_SIZE 1024 

int main(){
    struct sockaddr_in address;
    int opt = true;
    int master_socket , addrlen , new_socket , client_socket[30] ,  
          max_clients = 30 , valread, activity, sd;
    int max_sd;
    char buffer[BUFFER_SIZE] = { 0 };
    char *message = "Hello from server\n";

    fd_set readfds;

    for (int i = 0; i < max_clients; i++){   
        client_socket[i] = 0;   
    } 

    if((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        fprintf(stderr, "Socket Failed\n");
        exit(1);
    }
    
    if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0){
        fprintf(stderr, "setsockopt\n");
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(bind(master_socket, (struct sockaddr*)&address, sizeof(address)) < 0){
        fprintf(stderr, "Bind Failed\n");
        exit(1);
    }

    if(listen(master_socket, 3) < 0){
        fprintf(stderr, "listen\n");
        exit(1);
    }

    addrlen = sizeof(address);

    while(true){
        FD_ZERO(&readfds);

        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        for(int i = 0; i < max_clients; i++){
            sd = client_socket[i];

            if(sd > 0){
                FD_SET(sd, &readfds);
            }

            if(sd > max_sd){
                max_sd = sd;
            }
        }

        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
        if((activity < 0) && (errno != EINTR)){
            fprintf(stderr, "SELECT ERROR\n");
            exit(1);
        }

        if(FD_ISSET(master_socket, &readfds)){
            if((new_socket = accept(master_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0){
                fprintf(stderr, "ACCEPT ERROR\n");
                exit(1);
            }

            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs 
                  (address.sin_port));   
            
            if(send(new_socket, message, strlen(message), 0) != (int)strlen(message)){
                fprintf(stderr, "SEND ERROR\n");
            }

            printf("WELCOME MESSAGE SENT SUCCESSFULLY\n");

            for(int i = 0; i < max_clients; i++){
                if(client_socket[i] == 0){
                    client_socket[i] = new_socket;
                    printf("ADDING TO LIST OF SOCKETS as %d\n", i);
                    break;
                }
            }
        }

        for(int i = 0; i < max_clients; i++){
            sd = client_socket[i];

            if(FD_ISSET(sd, &readfds)){
                if((valread = read(sd, buffer, BUFFER_SIZE)) == 0){
                    getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" ,  
                          inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 

                    close(sd);
                    client_socket[i] = 0;
                } else {
                buffer[valread] = '\0';
                if(strcmp(buffer, "Hello there") == 0){
                    send(sd, "General kenobi\n", 15, 0);
                } else {
                    send(sd, buffer, strlen(buffer), 0);
                }
                printf("%s\n", buffer);
                }
            }
        }
    }

    return 0;
}
