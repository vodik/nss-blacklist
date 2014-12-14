#include "memory.h"

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>

int memblock_open_fd(memblock_t *memblock, int fd)
{
    struct stat st;
    fstat(fd, &st);
    memblock->len = st.st_size;
    memblock->mem = mmap(NULL, memblock->len, PROT_READ, MAP_SHARED | MAP_POPULATE, fd, 0);

    return memblock->mem == MAP_FAILED ? -errno : 0;
}

int memblock_open_file(memblock_t *memblock, const char *filename)
{
    int ret = 0, fd = open(filename, O_RDONLY);
    if (fd < 0)
        return -errno;

    ret = memblock_open_fd(memblock, fd);
    close(fd);
    return ret;
}

int memblock_close(memblock_t *memblock)
{
    if (memblock->mem != MAP_FAILED)
        return munmap(memblock->mem, memblock->len);
    return 0;
}
