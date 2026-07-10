#include <stdlib.h>
#include <string.h>
#include "utils.h"

char* my_strdup(const char* str) {
    if (str == NULL) {
        return NULL;
    }

    size_t len = strlen(str);
    char* copy = malloc(len + 1);
    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, str, len + 1);
    return copy;
}

char* my_strndup(const char* str, size_t n) {
    if (str == NULL) {
        return NULL;
    }

    size_t len = 0;
    while (len < n && str[len] != '\0') {
        len++;
    }

    char* copy = malloc(len + 1);
    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, str, len);
    copy[len] = '\0';
    return copy;
}