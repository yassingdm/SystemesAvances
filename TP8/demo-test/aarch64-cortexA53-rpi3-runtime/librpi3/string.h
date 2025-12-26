#ifndef STRING_H
#define STRING_H

#include <stddef.h>

size_t strnlen(const char *s, size_t maxlen);
void* memcpy(void *restrict dst, const void *restrict src, size_t n);
void bzero(void*s, size_t n) ;

#endif
