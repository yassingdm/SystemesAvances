#include <librpi3/string.h>

size_t strnlen(const char *s, size_t maxlen) {
    // there is no ets_strnlen
    const char *cp;
    for (cp = s; maxlen != 0 && *cp != '\0'; cp++, maxlen--);
    return (size_t)(cp - s);
}

void* memcpy(void *restrict dst, const void *restrict src, size_t n) {
  int i ;
  for(i=0;i<n;i++) ((char*)dst)[i] = ((char*)src)[i] ;
  return dst ;
}

void bzero(void *s, size_t n) {
  while(n) {
    n-- ;
    ((char*)s)[n] = 0 ;
  }
}


