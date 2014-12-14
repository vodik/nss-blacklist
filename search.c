#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ftw.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "boyermoore.h"

#define _unused_ __attribute__((unused))

char *pattern;
size_t patlen;
boyer_moore_t bm;

typedef struct memblock {
    char *mem;
    size_t len;
} memblock_t;

/* {{{ MMAP HELPER */
static int memblock_open_fd(memblock_t *memblock, int fd)
{
    struct stat st;
    fstat(fd, &st);
    memblock->len = st.st_size;
    memblock->mem = mmap(NULL, memblock->len, PROT_READ, MAP_SHARED | MAP_POPULATE, fd, 0);

    return memblock->mem == MAP_FAILED ? -errno : 0;
}

static int memblock_open_file(memblock_t *memblock, const char *filename)
{
    int ret = 0, fd = open(filename, O_RDONLY);
    if (fd < 0)
        return -errno;

    ret = memblock_open_fd(memblock, fd);
    close(fd);
    return ret;
}

static int memblock_close(memblock_t *memblock)
{
    if (memblock->mem != MAP_FAILED)
        return munmap(memblock->mem, memblock->len);
    return 0;
}
/* }}} */

static void print_line(const char *line)
{
    char copy[BUFSIZ];
    size_t len = strcspn(line, "\n");
    strncpy(copy, line, len);

    copy[len] = 0;
    printf("FOUND: %s\n", copy);
}

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i) {
        pattern = argv[i];
        patlen = strlen(pattern);

        size_t patlen = strlen(pattern);

        memblock_t mem;

        boyer_moore_compile(&bm, pattern, patlen);
        memblock_open_file(&mem, "someonewhocares.org");

        const char *p = mem.mem;
        size_t nbytes = mem.len;

        { // CRAP CODE
            const char *ret = boyer_moore_search(p, nbytes, &bm);
            /* printf("GOT: %c%c%c%c%c\n", ret[0], ret[1], ret[2], ret[3], ret[4]); */
            if (!ret)
                break;

            if (ret != mem.mem && ret[-1] != '\n') {
                break;
            }

            printf("RET: %.*s\n", patlen, ret);
        }
    }
}
