#pragma once

#include <stddef.h>
#include <limits.h>

typedef struct boyer_moore {
    size_t length;
    char *pattern;
    int *delta2;
    int delta1[CHAR_MAX];
} boyer_moore_t;

int boyer_moore_compile(boyer_moore_t *bm, const char *pat, size_t patlen);
const char *boyer_moore_search(const char *haystack, size_t len, boyer_moore_t *bm);
