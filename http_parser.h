#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

typedef struct {
    char* key;
    char* value;
} http_header;

typedef struct {
    char* method;
    char* url;
    char* version;

    http_header* headers;
    size_t header_count;

    char* body;
    size_t body_length;
} http_message;

int parse_http_request(const char* request, http_message* msg);

#endif // HTTP_PARSER_H