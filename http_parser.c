#include <stdio.h>
#include <assert.h>
#include "http_parser.h"

enum {
    HTTP_OK = 0,
    HTTP_ERROR_INVALID_ARGUMENT = -1,
};

int parse_http_request(const char* request, http_message* msg) {
    if (request == NULL || msg == NULL) {
        return HTTP_ERROR_INVALID_ARGUMENT;
    }

    const char* header_separator = "\r\n\r\n";
    const char* header_end = strstr(request, header_separator);

    return HTTP_OK;
}