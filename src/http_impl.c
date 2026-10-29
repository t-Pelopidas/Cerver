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

void print_request(HttpRequest *r){
    switch(r->method){
        case GET:
            printf("GET ");
            break;
        case POST:
            printf("POST ");
            break;
        case HEAD:
            printf("HEAD ");
            break;
    }
    for(uint8_t i = 0; i < r->header_count; ++i){
        printf("%s:%s", r->header->title, r->header->content); }

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
    printf("0:get, 1:post, 2:head { %d\n", request->method);
    return;
}

//this function shouldnt be int32_t. It returns an 8-bit uint
void get_requested_file(HttpRequest *request, const char *raw, uint16_t length, int *pos){
    //get_Http_method will point *pos one position behind the filename
    ++(*pos);
    int16_t index = 0,
           r_count = 0,
           n_count = 0;
    while(raw[*pos] != ' ' && *pos <= length){
        request->requested_file[index++] = raw[(*pos)++];

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
    request->requested_file[index] = '\0';
    request->file_name_length = index;
}

void get_Http_version(HttpRequest *request, const char *raw, int *pos){
    ++(*pos);
    uint8_t index = 0;

    for(;;){
        request->http_version[index++] = raw[(*pos)++];
        if(raw[*pos] == '\r' && raw[(*pos)+1] == '\n'){
            request->http_version[index] = '\0';
            *pos += 2;
            break;
        }
    }
}
void parse_header_title(HttpRequest *request, const char *raw, int *pos){
    uint16_t index = 0;
    while(1){
        request->header->title[index++] = raw[(*pos)++];
        if(raw[*pos] == ':'){
            request->header->title[index] = '\0';
            *pos +=2;
            return;
        }
    }
}

void parse_header_content(HttpRequest *request, const char *raw, int *pos){
    uint16_t index = 0;
    while(1){
        request->header->content[index++] = raw[(*pos)++];
        if(raw[*pos] == '\r' && raw[(*pos)+1] == '\n'){
            request->header->title[index] = '\0';
            *pos +=2;
            return;
        }
    }
}

void get_requested_headers(HttpRequest *request, const char *raw, int *pos){
    //initial malloc memory chunck
    uint16_t allocation_chunk = 512;
    //Allocate one block for the header. header->title is the head of the point
    //and header->content = (header->title + strlen(header->title));
    //printf("title pointer: %p\n", request->header->title);
    request->header->title = (char *)malloc(allocation_chunk);
    parse_header_title(request, raw, pos);
    request->header->content = request->header->title + strlen(request->header->title);
    printf("title: %p, content: %p, strlen: %zu\n", request->header->title, request->header->content, strlen(request->header->title));
    parse_header_content(request, raw, pos);
    //
    //
    //Some bullshit is going on here. The title sticks to the content when printed. idk if its a print issue though.
    //Also the content isnt being parsed correctly. Its gets chopped at the ':' part of the 'Host' header.
    printf("title: %s, content: %s\n", request->header->title, request->header->content);
    return;
}
void parse_request(HttpRequest *request, const char *raw, uint16_t length){
    int32_t pos = 0;

    printf("%s\n", raw);
    // uint16_t i = 0;
    // while(raw[i] != 0){
    //     printf("%x ", raw[i]);
    //     if(raw[i] == '\n')
    //         printf("\n");
    //     ++i;
    // }

    get_Http_method(request, raw, &pos);
    get_requested_file(request, raw, length, &pos);
    //
    //THIS FAILS. Make sure that the file pointer in the request struct points to writable memory
    //assert(0 && "Fix writable memory problem");
    //
    printf("requested_file: %s\n", request->requested_file);
    get_Http_version(request, raw, &pos);
    printf("Http version: %s\n", request->http_version);
    get_requested_headers(request, raw, &pos);
    return;
}

// void build_response(HttpRequest *request, HttpResponse *response){
//         server_response =
//             "HTTP/1.0 200 OK\r\n"
//             "Content-Type: text/html; charset=utf-8\r\n"
//             "Content-Length: 126\r\n"
//             "Connection: close\r\n"
//             "\r\n"
//             "<!DOCTYPE html>\n"
//             "<html>\n"
//             "<head><title>Cerver</title></head>\n"
//             "<body>\n"
//             "<h1>It works!</h1>\n"
//             "<p>Hello from Cerver.</p>\n"
//             "</body>\n"
//             "</html>\n";
//     return;
// }

// void send_response(int clientfd, HttpResponse *response){
//     return;
// }
