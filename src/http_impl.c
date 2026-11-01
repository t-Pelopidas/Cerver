#include<assert.h>
#include<stdint.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include"HTTPlib.h"


#define IS_END(r, p)             (\
    r[p] == '\r' && r[p+1] == '\n'\
)
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
    printf("0:get, 1:post, 2:head {%d}\n", request->method);
    ++(*pos);
    return;
}

void get_requested_file(HttpRequest *request, const char *raw, uint16_t length, int *pos){
    //get_Http_method will point *pos one position behind the filename
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
    ++(*pos);
}

void get_Http_version(HttpRequest *request, const char *raw, int *pos){
    uint8_t index = 0;

    for(;;){
        request->http_version[index++] = raw[(*pos)++];
        if(IS_END(raw, *pos)){
            request->http_version[index] = '\0';
            *pos += 2;
            break;
        }
    }
}
void parse_header_title(Header *header, const char *raw, int *pos){
    uint16_t index = 0;
    while(raw[*pos] != ':'){
        header->title[index++] = raw[(*pos)++];
    }
    header->title[index] = '\0';
    (*pos) += 2;
    return;
}

void parse_header_content(Header *header, const char *raw, int *pos){
    uint16_t index = 0;
    while(raw[*pos] != '\r'){
        header->content[index++] = raw[(*pos)++];
    }
    header->content[index] = '\0';
    (*pos) += 2;
    return;
}

void get_requested_headers(HttpRequest *request, const char *raw, int *pos){
    //initial malloc memory chunck. 8kb per/header limit and 16kb total limit
    uint16_t total_memory_allocated = 64, per_header_allocation_chuck = total_memory_allocated;

    request->header_count = 1;
    if((request->header = (Header *)malloc(sizeof(Header))) == NULL){
        printf("Allocation failed\n");
        die("malloc", errno);
    }
    Header *current_header = request->header;

    //Actually implement the per_header and total_memory counting
    while(per_header_allocation_chuck < 16384 && total_memory_allocated < 32768){
        per_header_allocation_chuck = 64;
        if((current_header->title = (char *)malloc(per_header_allocation_chuck)) == NULL){
            printf("Allocation returned a null pointer\n");
            die("malloc", errno);
        }

        parse_header_title(current_header, raw, pos);
        current_header->content = current_header->title + strlen(current_header->title) + 1;

        parse_header_content(current_header, raw, pos);

        if (IS_END(raw, *pos))
            break;

        ++request->header_count;
        if((current_header->next_header = (Header *)malloc(per_header_allocation_chuck)) == NULL){
            printf("Mallocation for a new header failed\n");
            die("malloc", errno);
        }
        current_header = current_header->next_header;
    }
    current_header->next_header = NULL;
    return;
}
void parse_request(HttpRequest *request, const char *raw, uint16_t length){
    int32_t pos = 0;

    printf("%s\n", raw);
    // Hex-print of the request
    // uint16_t i = 0;
    // while(raw[i] != 0){
    //     printf("%x ", raw[i]);
    //     if(raw[i] == '\n')
    //         printf("\n");
    //     ++i;
    // }

    get_Http_method(request, raw, &pos);
    get_requested_file(request, raw, length, &pos);
    get_Http_version(request, raw, &pos);

    //If there are header files
    if(!IS_END(raw, pos)){
        get_requested_headers(request, raw, &pos);
    }
    else{
        request->header = NULL;
        request->header_count = 0;
    }
    Header *current = request->header;
    while(current != NULL){
        printf("%s :: %s\n", current->title, current->content);
        current = current->next_header;
    }
    return;
}

// void build_response(HttpRequest *request, HttpResponse *response){
//     return;
// }

// void send_response(int clientfd, HttpResponse *response){
//     return;
// }
