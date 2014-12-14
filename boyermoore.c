#include "boyermoore.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

static inline int max(int a, int b) { return (a < b) ? b : a; }

static inline bool is_prefix(const char *word, size_t wordlen, size_t pos)
{
    size_t suffixlen = wordlen - pos;
    return strncmp(word, &word[pos], suffixlen) == 0;
}

static size_t suffix_length(const char *word, size_t wordlen, size_t pos)
{
    size_t i;
    for (i = 0; word[pos - i] == word[wordlen - 1 - i] && i < pos; i++);
    return i;
}

static void make_delta1(int delta[static CHAR_MAX], const char *pat, size_t patlen)
{
    size_t i;

    for (i = 0; i < CHAR_MAX; i++)
        delta[i] = patlen;
    for (i = 0; i < patlen - 1; i++)
        delta[(int)pat[i]] = patlen - 1 - i;
}

static void make_delta2(int *delta, const char *pat, size_t patlen)
{
    size_t pos, last_prefix_index = patlen - 1;

    for (pos = patlen; pos > 0; pos--) {
        if (is_prefix(pat, patlen, pos))
            last_prefix_index = pos;

        delta[pos - 1] = last_prefix_index + patlen - pos;
    }

    for (pos = 0; pos < patlen - 1; pos++) {
        size_t slen = suffix_length(pat, patlen, pos);

        if (pat[pos - slen] != pat[patlen - 1 - slen])
            delta[patlen - 1 - slen] = patlen - 1 - pos + slen;
    }
}

int boyer_moore_compile(boyer_moore_t *bm, const char *pat, size_t patlen)
{
    bm->delta2 = malloc(sizeof(int) * patlen);
    bm->pattern = strndup(pat, patlen);
    bm->length = patlen;

    make_delta1(bm->delta1, pat, patlen);
    make_delta2(bm->delta2, pat, patlen);

    return 0;
}

const char *boyer_moore_search(const char *haystack, size_t len, boyer_moore_t *bm)
{
    size_t i = bm->length, j;

    while (i < len) {
        j = bm->length;

        for (; j > 0 && haystack[i - 1] == bm->pattern[j - 1]; --i, --j);

        if (j == 0)
            return &haystack[i];
        i += max(bm->delta1[(int)haystack[i - 1]], bm->delta2[j - 1]);
    }

    return NULL;
}
