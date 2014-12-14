#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <nss.h>
#include <netdb.h>
#include <arpa/inet.h>

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

    FILE *fp = fopen("/tmp/gethostbyname", "a+");
    if (fp) {
        fprintf(fp, "Looking up %s\n", name);
        fclose(fp);
    }

    /* *result = (struct hostent){ */
    /*     .h_name = NULL, */
    /*     .h_aliases = NULL, */
    /*     .h_addrtype = af, */
    /*     .h_length = af == AF_INET ? INADDRSZ : IN6ADDRSZ; */
    /*     .h_addr_list = NULL */
    /* }; */

    {
        *errnop = EAFNOSUPPORT;
        *h_errnop = NO_DATA;
        return NSS_STATUS_UNAVAIL;
    }
}

enum nss_status _nss_blacklist_gethostbyname_r(const char *name,
                                               struct hostent *result,
                                               char *buffer,
                                               size_t buflen,
                                               int *errnop,
                                               int *h_errnop)
{
    return _nss_blacklist_gethostbyname2_r(name,
                                           AF_INET,
                                           result,
                                           buffer,
                                           buflen,
                                           errnop,
                                           h_errnop);
}

enum nss_status _nss_blacklist_gethostbyaddr_r(const void *addr,
                                                socklen_t len,
                                                int af,
                                                struct hostent *result,
                                                char *buffer,
                                                size_t buflen,
                                                int *errnop,
                                                int *h_errnop)
{
    *errnop = EAFNOSUPPORT;
    *h_errnop = NO_DATA;
    return NSS_STATUS_UNAVAIL;
}
