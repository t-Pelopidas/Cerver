#include <signal.h>
#include<stdint.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<errno.h>
#include"HTTPlib.h"

sig_atomic_t is_running = 1;

// void handle_shutdown(int sig){
//     if(sig == SIGINT || sig == SIGTERM){
//         printf("Shutting down server\n");
//         is_running = 0;
//     }
// }

int main(){
    int32_t serverfd, clientfd;
    int opt = 1, len;
    struct sockaddr_in client, server;
    char raw_data[MAX_BUF] = {0};

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("192.168.1.100"); //CHANGE THIS TO A SPECIFIC ADDRESS
    server.sin_port = htons(PORT);
    ///
    /// socket
    if((serverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        die("socket", errno);
    /// setsockopt
    if(setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        die("setsockopt", errno);
    /// bind
    if((bind(serverfd, (struct sockaddr *)&server, sizeof(server))) != 0)
        die("bind", errno);
    /// listen
    if((listen(serverfd, MAX_CONNECTIONS)) != 0)
        die("listen", errno);
    /// accept
    socklen_t client_length = sizeof(client);

    // signal(SIGTERM, handle_shutdown);
    // signal(SIGINT, handle_shutdown);

    //while(is_running){
        HttpRequest request;
        HttpResponse response;
        //find a way to allocate memory for request.file
        if((clientfd = accept(serverfd, (struct sockaddr *)&client, &client_length)) < 0)
            die("accept", errno);
        printf("Parsing request\n");
        parse_request(&request, clientfd, raw_data, len);
        build_response(&request, &response);
        send_response(clientfd, &response);
    //}
    close(clientfd);
    close(serverfd);
    exit(0);
}
