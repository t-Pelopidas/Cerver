#include<assert.h>
#include<stdint.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include"HTTPlib.h"

void die(const char* function_name,int exit_code){
    perror(function_name);
    exit(exit_code);
}

int32_t read_raw_request(int fd, char *raw, uint16_t length){
    int32_t count = 0;
    if((count = read(fd, raw, length)) < 0){
        die("read", errno);
    }
    if(count == 0){
        printf("Lost connection with client\n");
        exit(-1);
    }
    if(count == length){
        fprintf(stderr, "Maximum request length reached\n");
        exit(-1);
    }
    raw[++count] = '\0';
    return count;
}

void get_Http_method(HttpRequest *request, const char *raw, int *pos){
    char buff[MAX_BUF] = {0};
    while(raw[*pos] != ' ' && raw[*pos] != '\0' && *pos <= MAX_BUF){
        buff[*pos] = raw[*pos];
        ++(*pos);
    }
    if(!strcmp(buff, "GET")){
        request->method = GET;
    }
    else if(!strcmp(buff, "POST")){
        request->method = POST;
    }
    else if(!strcmp(buff, "HEAD")){
        request->method = HEAD;
    }
    else{
        fprintf(stderr, "HTTP method not recognized: %s\n", buff);
        exit(-1);
        return;
    }
    printf("%d\n", request->method);
    return;
}

//this shouldnt be int32_t. It returns an 8-bit uint
int32_t get_requested_file(HttpRequest *request, const char *raw, uint16_t length,int *pos){
    //get_Http_method will point *pos one position behind the filename
    ++(*pos);
    int8_t index = 0,
           r_count = 0,
           n_count = 0;
    while(raw[*pos] != ' ' && *pos <= length){
        request->file[index++] = raw[(*pos)++];
        if(raw[*pos] == '\r')
            r_count++;
        else if(raw[*pos] == '\n')
            n_count++;
        //At every letter, r_count and n_count get assigned a zero. Not optimized?
        else
            r_count = n_count = 0;
        if(r_count == 2 && n_count == r_count)
            break;
    }
    request->file[index] = '\0';
    return index;
}

void parse_request(HttpRequest *request, const int fd, char *raw, uint16_t length){
    int32_t count, pos = 0;
    count = read_raw_request(fd, raw, length);

    write(STDOUT_FILENO, raw, count);
    printf("%d\n", count);

    get_Http_method(request, raw, &pos);
    count = get_requested_file(request, raw, length, &pos);
    //
    // THIS FAILS. Make sure that the file pointer in the request struct points to writable memory
    //assert(0 && "Fix writable memory problem");
    //
    printf("requested_file: %s\n", request->file);
    //get_headers(request);
    return;
}

void build_response(HttpRequest *request, HttpResponse *response){
        // server_response =
        //     "HTTP/1.0 200 OK\r\n"
        //     "Content-Type: text/html; charset=utf-8\r\n"
        //     "Content-Length: 126\r\n"
        //     "Connection: close\r\n"
        //     "\r\n"
        //     "<!DOCTYPE html>\n"
        //     "<html>\n"
        //     "<head><title>Cerver</title></head>\n"
        //     "<body>\n"
        //     "<h1>It works!</h1>\n"
        //     "<p>Hello from Cerver.</p>\n"
        //     "</body>\n"
        //     "</html>\n";
    return;
}

void send_response(int clientfd, HttpResponse *response){
    return;
}
