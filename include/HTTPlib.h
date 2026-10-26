#include<stdint.h>

#define MAX_CONNECTIONS 1
#define PORT 8000
#define MAX_BUF 512

typedef uint8_t StatusCode;

typedef enum{
    GET,
    POST,
    HEAD
}HTTP_method;

typedef struct{
    char *title;
    char *content;
}Header;

typedef struct {
    HTTP_method method;
    char *file;
    char *http_version;
    Header *headers;
    char *content_type;
}HttpRequest;

typedef struct {
    char *http_version;
    StatusCode status_code;
    Header *headers;
    char *data;
}HttpResponse;

void die(const char* function_name,int exit_code);
int32_t read_raw_request(int fd, char *raw, uint16_t length);
void get_Http_method(HttpRequest *request, const char *raw, uint16_t length, int *pos);
int32_t get_requested_file(HttpRequest *request, const char *raw, uint16_t length,int *pos, char *filename);
void parse_request(HttpRequest *request, const int fd, char* raw, uint16_t length);
void build_response(HttpRequest *request, HttpResponse *response);
void send_response(int clientfd, HttpResponse *response);
