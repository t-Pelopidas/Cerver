#include<stdint.h>

#define MAX_CONNECTIONS 1
#define PORT 8000
#define MAX_BUF 512
#define MAX_HTTP_BUF 16384

typedef uint8_t StatusCode;

typedef enum Http_method{
    GET,
    POST,
    HEAD
}Http_method;

typedef struct Header{
    char *title;
    char *content;
    struct Header *next_header;
}Header;

typedef struct HttpRequest{
    Http_method method;
    char *requested_file;
    uint16_t file_name_length;
    char *http_version;
    Header *header;
    Header *content_type;
    uint8_t header_count; // maybe unused
}HttpRequest;

typedef struct HttpResponse{
    char *http_version;
    StatusCode status_code;
    Header *header;
    char *data;
}HttpResponse;

void die(const char* function_name,int exit_code);

int32_t read_raw_request(int fd, char *raw, uint16_t length);
void parse_request(HttpRequest *request, const char *raw, uint16_t length);
void parse_header_title(Header *header, const char *raw, int *pos, uint16_t *per_header_allocation_chuck);
void parse_header_content(Header *header, const char *raw, int *pos, uint16_t *per_header_allocation_chuck);
void get_Http_method(HttpRequest *request, const char *raw, int *pos);
void get_request_file(HttpRequest *request, const char *raw, uint16_t length, int *pos);
void get_request_http_version(HttpRequest *request, const char *raw);
void get_request_headers(HttpRequest *request, const char *raw, int *pos);
void get_Http_version(HttpRequest *request, const char *raw, int *pos);
void build_response(HttpRequest *request, HttpResponse *response);
void send_response(int clientfd, HttpResponse *response);
