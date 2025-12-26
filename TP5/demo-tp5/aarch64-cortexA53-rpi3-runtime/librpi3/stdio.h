#ifndef STDIO_H
#define STDIO_H

#include <stdint.h> // For integer types
#include <stddef.h> // For size_t
#include <stdarg.h> // For the defintion for va_list

//=======================================================================
// Very low level hex printer
//=======================================================================
void uint64hex(int coreid, uint64_t num, char*out_buf) ;

//=======================================================================
// Simple implementations of sprintf and vsprintf that only 
// cover the formats %u %d %x/%X %s. The only supported
// qualifiers are # and a number between 1 and 9 which
// give the min size of the field. */
//=======================================================================
int snprintf(char* restrict buffer,
	    size_t buf_size,
	    const char * restrict fmt, ...) ;
int vsnprintf(char* restrict result,
	      size_t buf_size,
	      const char * restrict fmt,
	      va_list va) ;

//=======================================================================
// Simple sscanf functions
//=======================================================================
int sscanf(const char * restrict s, 
	   const char * restrict format, ...);
int vsscanf(const char * restrict s, 
	   const char * restrict format, va_list va);

#endif
