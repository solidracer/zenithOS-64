#ifndef _def_h
#define _def_h

#include <stddef.h>
#include <stdint.h>

#define PACKED __attribute__((packed))
#define SECTION(x) __attribute__((section(x)))
#define ALIGNED(x) __attribute__((aligned(x)))
#define UNUSED(x) ((void)(x))
#define ALIGN(mem, align) ((mem + align - 1) & ~(align-1))
#define TOADDR(x) ((size_t)(x))
#define TOPTR(x) ((void*)(size_t)(x))

#define TRYCALL(fun, ...)   \
if (fun(__VA_ARGS__)) {     \
    return;                 \
}                           \

#endif
