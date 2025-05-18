#ifndef _memutil_h
#define _memutil_h

#include "def.h"

static inline void *memcpy(void *dest, const void *src, size_t n) {
    char *d = dest;
    const char *s = src;
    while (n--)
        *d++ = *s++;
    return dest;
}

static inline void *memset(void *dest, int c, size_t n) {
    unsigned char *d = dest;
    while (n--)
        *d++ = (unsigned char) c;
    return dest;
}

static inline int memcmp(const void *ptr1, const void *ptr2, size_t n) {
    const unsigned char *p1 = ptr1, *p2 = ptr2;

    while (n--) {
        if (*p1 != *p2)
            return *p1 - *p2;
        else {
            p1++;
            p2++;
        }
    }
    
    return 0;
}

static inline void *memmove(void *dest, const void *src, size_t n) {
    char *d = dest;
    const char *s = src;
    if (d < s)
        memcpy(dest, src, n);
    else {
        const char *lasts = s + (n - 1);
        char *lastd = d + (n - 1);

        while (n--)
            *lastd-- = *lasts--;
    }
    return dest;
}

static inline int strlen(const char *str) {
    int i = 0;
    while (str[i]) 
        i++;
    return i;
}

#endif
