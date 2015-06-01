#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <nss.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "boyermoore.h"
#include "memory.h"

#define IN6ADDRSZ 16
#define INADDRSZ 4

enum nss_status _nss_blacklist_gethostbyname2_r(const char *name,
                                                int af,
                                                struct hostent *result,
                                                char *buffer,
                                                size_t buflen,
                                                int *errnop,
                                                int *h_errnop);


static inline size_t align_ptr(size_t l)
{
    const size_t a = sizeof(void *) - 1;
    return (l + a) & ~a;
}

static inline bool match_in_file(const char *pattern)
{
    size_t patlen = strlen(pattern);

    memblock_t mem;
    if (memblock_open_file(&mem, "/etc/someonewhocares.org") < 0) {
        return false;
    }

    boyer_moore_t bm;
    boyer_moore_init(&bm, mem.mem, mem.len, pattern);

    bool found = false;
    while (1) {
        const char *ret = boyer_moore_next(&bm);
        if (!ret)
            break;

        if (ret == mem.mem || ret[-1] == '\n') {
            if (ret[patlen] != '\n' && ret[patlen] != '\0')
                continue;

            found = true;
            break;
        }
    }

    memblock_close(&mem);
    return found;
}

enum nss_status _nss_blacklist_gethostbyname2_r(const char *name,
                                                int af,
                                                struct hostent *result,
                                                char *buffer,
                                                size_t buflen,
                                                int *errnop,
                                                int *h_errnop)
{
    /* uintptr_t pad = -(uintptr_t) buffer % __alignof__ (struct hostent_data); */
    /* buffer += pad; */
    /* buflen = buflen > pad ? buflen - pad : 0; */

    /* char tmp_buffer_stack[tmp_buflen] __attribute__ ((__aligned__ (__alignof__ (struct hostent_data)))) */

    if (!match_in_file(name)) {
         *errnop = ESRCH;
         *h_errnop = HOST_NOT_FOUND;
         return NSS_STATUS_NOTFOUND;
    }

    memset(buffer, '\0', buflen);

    size_t idx = 0;
    size_t addrlen;
    char *r_name, **r_aliases, *r_addr, **r_addr_list;

    size_t ms, l = strlen(name);
    /* ms = align_ptr(l + 1) +  ... */
    /* if (buflen < ms) { */
    /*     *errnop = ENOMEM; */
    /*     *h_errnop = TRY_AGAIN; */
    /*     return NSS_STATUS_TRYAGAIN; */
    /* } */

    /* First place the address */
    r_addr = buffer;
    if (af == AF_INET) {
        addrlen = INADDRSZ;
        inet_pton(af, "127.0.0.1", r_addr);
    } else {
        addrlen = IN6ADDRSZ;
        inet_pton(af, "::1", r_addr);
    }
    idx += align_ptr(addrlen);

    /* Second, place the address list */
    r_addr_list = (char **)(buffer + idx);
    r_addr_list[0] = r_addr;
    r_addr_list[1] = NULL;
    idx += sizeof(char *) * 2;

    /* Third, place the alias list */
    r_aliases = (char **)(buffer + idx);
    r_aliases[0] = NULL;
    idx += sizeof(char *);

    r_name = buffer + idx;
    memcpy(r_name, name, l + 1);

    result->h_name = r_name;
    result->h_aliases = (char **)r_aliases;
    result->h_addrtype = af;
    result->h_length = addrlen;
    result->h_addr_list = (char **)r_addr_list;

    *errnop = 0;
    *h_errnop = NETDB_SUCCESS;
    h_errno = 0;

    return NSS_STATUS_SUCCESS;
}

/* enum nss_status _nss_blacklist_gethostbyname_r(const char *name, */
/*                                                struct hostent *result, */
/*                                                char *buffer, */
/*                                                size_t buflen, */
/*                                                int *errnop, */
/*                                                int *h_errnop) */
/* { */
/*     return _nss_blacklist_gethostbyname2_r(name, */
/*                                            AF_INET, */
/*                                            result, */
/*                                            buffer, */
/*                                            buflen, */
/*                                            errnop, */
/*                                            h_errnop); */
/* } */

/* enum nss_status _nss_blacklist_gethostbyaddr_r(const void *addr, */
/*                                                 socklen_t len, */
/*                                                 int af, */
/*                                                 struct hostent *result, */
/*                                                 char *buffer, */
/*                                                 size_t buflen, */
/*                                                 int *errnop, */
/*                                                 int *h_errnop) */
/* { */
/*     *errnop = EAFNOSUPPORT; */
/*     *h_errnop = NO_DATA; */
/*     return NSS_STATUS_UNAVAIL; */
/* } */
