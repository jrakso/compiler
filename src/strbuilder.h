#pragma once

#include <stddef.h>

typedef struct {
    char *data;
    size_t len;
    size_t capacity;
} StringBuilder;

void sb_init(StringBuilder *sb);
void sb_append(StringBuilder *sb, const char *str);
void sb_append_fmt(StringBuilder *sb, const char *fmt, ...);
const char *sb_data(const StringBuilder *sb);
void sb_free(StringBuilder *sb);