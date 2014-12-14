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
#include "memory.h"

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i) {
        const char *pattern = argv[i];
        size_t patlen = strlen(pattern);

        memblock_t mem;
        memblock_open_file(&mem, "someonewhocares.org");

        boyer_moore_t bm;
        boyer_moore_init(&bm, mem.mem, mem.len, pattern);

        while (1) {
            const char *ret = boyer_moore_next(&bm);
            if (!ret)
                break;

            if (ret == mem.mem || ret[-1] == '\n') {
                if (ret[patlen] != '\n' && ret[patlen] != '\0')
                    continue;

                printf("RET: %.*s\n", (int)patlen, ret);
                return 0;
            }
        }
    }
}
