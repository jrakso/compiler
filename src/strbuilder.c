#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "strbuilder.h"

#define SB_INITIAL_CAP 128

static void sb_reserve(StringBuilder *sb, size_t extra) {
    if (sb->len + extra + 1 > sb->capacity) {
        while (sb->len + extra + 1 > sb->capacity)
            sb->capacity *= 2;
        char *new_data = realloc(sb->data, sb->capacity);
        if (!new_data) {
            perror("realloc");
            free(sb->data);
            exit(EXIT_FAILURE);
        }
        sb->data = new_data;
    }
}

void sb_init(StringBuilder *sb) {
    sb->capacity = SB_INITIAL_CAP;
    sb->len = 0;
    sb->data = malloc(sb->capacity);
    if (!sb->data) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    sb->data[0] = '\0';
}

void sb_append(StringBuilder *sb, const char *str) {
    size_t len = strlen(str);
    sb_reserve(sb, len);
    memcpy(sb->data + sb->len, str, len + 1);  // copy including '\0'
    sb->len += len;
}

void sb_append_fmt(StringBuilder *sb, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    va_list copy;
    va_copy(copy, args);
    int needed = vsnprintf(NULL, 0, fmt, copy);
    va_end(copy);

    if (needed < 0) {
        perror("vsnprintf");
        exit(EXIT_FAILURE);
    }

    sb_reserve(sb, (size_t)needed);
    vsnprintf(sb->data + sb->len, needed + 1, fmt, args);
    va_end(args);
    sb->len += (size_t)needed;
}

const char *sb_data(const StringBuilder *sb) {
    return sb->data;
}

void sb_free(StringBuilder *sb) {
    free(sb->data);
    sb->data = NULL;
    sb->len = sb->capacity = 0;
}