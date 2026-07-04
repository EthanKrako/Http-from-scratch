#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

typedef struct {
    char* method;
    char* url;
    char* version;
    char* headers;
    char* body;
} http_message;

int parse_http_request(const char* request, http_message* msg);

#endif // HTTP_PARSER_H