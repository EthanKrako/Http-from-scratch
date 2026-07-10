#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "http_parser.h"
#include "utils.h"

typedef enum {
    HTTP_OK = 0,
    HTTP_ERROR_INVALID_ARGUMENT = -1,
    HTTP_ERROR_MISSING_HEADER_TERMINATOR = -2,
    HTTP_ERROR_EMPTY_HEADERS = -3,
    HTTP_ERROR_MALFORMED_START_LINE = -4,
    HTTP_ERROR_MEMORY_ALLOCATION = -5,
} http_parse_error;

const char* HEADER_SEPARATOR = "\r\n\r\n";

http_parse_error handle_header_request_line(char* line, http_message* msg) {
    if (line == NULL || msg == NULL) {
        return HTTP_ERROR_INVALID_ARGUMENT;
    }

    const char* method_end = strchr(line, ' ');
    if (method_end == NULL) {
        return HTTP_ERROR_MALFORMED_START_LINE;
    }

    const char* url_end = strchr(method_end + 1, ' ');
    if (url_end == NULL) {
        return HTTP_ERROR_MALFORMED_START_LINE;
    }

    const size_t method_length = method_end - line;
    if (method_length == 0) {
        return HTTP_ERROR_MALFORMED_START_LINE;
    }

    const size_t url_length = url_end - (method_end + 1);
    if (url_length == 0) {
        return HTTP_ERROR_MALFORMED_START_LINE;
    }

    const size_t version_length = strlen(url_end + 1);
    if (version_length == 0) {
        return HTTP_ERROR_MALFORMED_START_LINE;
    }

    msg->method = my_strndup(line, method_length);
    msg->url = my_strndup(method_end + 1, url_length);
    msg->version = my_strndup(url_end + 1, version_length);

    if (msg->method == NULL || msg->url == NULL || msg->version == NULL) {
        free(msg->method);
        free(msg->url);
        free(msg->version);
        msg->method = NULL;
        msg->url = NULL;
        msg->version = NULL;
        return HTTP_ERROR_MEMORY_ALLOCATION;
    }

    return HTTP_OK;
}

http_parse_error parse_http_request(const char* request, http_message* msg) {
    http_parse_error return_value = HTTP_OK;

    if (request == NULL || msg == NULL) {
        return HTTP_ERROR_INVALID_ARGUMENT;
    }

    memset(msg, 0, sizeof(http_message));

    const char* header_end = strstr(request, HEADER_SEPARATOR);

    if (header_end == NULL) {
        return HTTP_ERROR_MISSING_HEADER_TERMINATOR;
    }

    size_t header_length = header_end - request;

    char* header = malloc(header_length + 1);

    if (header == NULL) {
        return HTTP_ERROR_MEMORY_ALLOCATION;
    }

    strncpy(header, request, header_length);
    header[header_length] = '\0';

    char* line = strtok(header, "\r\n");

    if (line == NULL) {
        return_value = HTTP_ERROR_EMPTY_HEADERS;
        goto leave_header;
    }

    http_parse_error result = handle_header_request_line(line, msg);

    if (result != HTTP_OK) {
        return_value = result;
        goto leave_header;
    }

    leave_header:
    free(header);

    return return_value;
}