#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "http_parser.h"
#include "utils.h"

const char* HEADER_SEPARATOR = "\r\n\r\n";

typedef enum {
    HTTP_OK = 0,
    HTTP_ERROR_INVALID_ARGUMENT = -1,
    HTTP_ERROR_MISSING_HEADER_TERMINATOR = -2,
    HTTP_ERROR_EMPTY_HEADERS = -3,
    HTTP_ERROR_MALFORMED_START_LINE = -4,
    HTTP_ERROR_MEMORY_ALLOCATION = -5,
} http_parse_error;

static http_parse_error add_header(http_message* msg, const char* name, size_t name_len, const char* value) {
    if (msg == NULL || name == NULL || value == NULL) {
        return HTTP_ERROR_INVALID_ARGUMENT;
    }

    http_header* new_headers = realloc(msg->headers, (msg->header_count + 1) * sizeof(http_header));
    if (new_headers == NULL) {
        return HTTP_ERROR_MEMORY_ALLOCATION;
    }
    msg->headers = new_headers;

    msg->headers[msg->header_count].key = my_strndup(name, name_len);
    msg->headers[msg->header_count].value = my_strdup(value);

    if (msg->headers[msg->header_count].key == NULL || msg->headers[msg->header_count].value == NULL) {
        free(msg->headers[msg->header_count].key);
        free(msg->headers[msg->header_count].value);
        return HTTP_ERROR_MEMORY_ALLOCATION;
    }

    msg->header_count++;
    return HTTP_OK;
}

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

static http_parse_error handle_header_fields(http_message* msg) {
    if (msg == NULL) {
        return HTTP_ERROR_INVALID_ARGUMENT;
    }

    char* line;
    while ((line = strtok(NULL, "\r\n")) != NULL) {
        char* separator = strchr(line, ':');
        if (separator == NULL) {
            continue;
        }

        size_t name_len = separator - line;
        char* value = separator + 1;
        while (*value == ' ') {
            value++;
        }

        http_parse_error result = add_header(msg, line, name_len, value);
        if (result != HTTP_OK) {
            return result;
        }

        // search for content-length header to determine body length
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

    result = handle_header_fields(msg);

    if (result != HTTP_OK) {
        return_value = result;
        goto leave_header;
    }

    leave_header:
    free(header);

    return return_value;
}