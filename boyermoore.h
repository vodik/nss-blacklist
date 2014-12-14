#pragma once

#include <stddef.h>
#include <limits.h>

typedef struct boyer_moore {
    const char *block;
    size_t block_len;
    const char *pattern;
    size_t pattern_len;
    int *delta2;
    int delta1[CHAR_MAX];
} boyer_moore_t;

int boyer_moore_init(boyer_moore_t *bm, const char *haystack, size_t len, const char *pattern);
const char *boyer_moore_next(boyer_moore_t *bm);
