#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

typedef enum {
    HTTP_OK = 0,
    HTTP_ERROR_INVALID_ARGUMENT = -1,
    HTTP_ERROR_MISSING_HEADER_TERMINATOR = -2,
    HTTP_ERROR_EMPTY_HEADERS = -3,
    HTTP_ERROR_MALFORMED_START_LINE = -4,
    HTTP_ERROR_MEMORY_ALLOCATION = -5,
} http_parse_error;

void handle_http_error(http_parse_error error);

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