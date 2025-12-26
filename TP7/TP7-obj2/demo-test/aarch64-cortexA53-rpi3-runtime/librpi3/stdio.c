#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <librpi3/string.h>
#include <librpi3/stdio.h>


//=======================================================================
// Very low level hex printer
//=======================================================================
void uint64hex(int coreid, uint64_t num, char*out_buf) {
  unsigned char res[16] ;
  int len = 0 ;
  do {
    res[len++] = num & 0xf ;
    num >>= 4 ;
  } while (num) ;
  int j ;
  out_buf[0] = 'C' ;
  out_buf[1] = '0' + coreid ;
  out_buf[2] = ':' ;  
  out_buf[3] = '0' ;
  out_buf[4] = 'x' ;
  for(j=0;j<len;j++) {
    if(res[j]<10) {
      out_buf[len+4-j] = '0' + res[j] ;
    } else {
      out_buf[len+4-j] = 'a' + (res[j]-10) ;
    }
  }
  out_buf[len+5] = '\n' ;
  out_buf[len+6] = 0 ;
}


//=======================================================================
// The main integer printing routine
//=======================================================================

// We are on a 64-bit architecture
#define UNSIGNED uint64_t

// Needed to establish max storage needed locally.
// To print an unsigned integer, the worst case is when we print
// MAXUNSIGNED in base 2. In this case, we need one digit for each bit
// of the UNSIGNED representation. Storing this string in memory
// requires at most UINT_BIT+1 chars.
#define UINT_BIT (8*sizeof(UNSIGNED))

// The base must allow representation with digits and
// letters, so here are the bounds on it
#define MIN_BASE 2
#define MAX_BASE ('z'-'a'+'9'-'0'+1)

// Return value is the length of the string,
// not including the trailing \0. Provided buffer must
// accomodate the printing of the worst-case string.
int convert2ascii(UNSIGNED num,       /* Unsigned number to print. */
		  UNSIGNED base,      /* Base. */
		  UNSIGNED min_length,/* Min number of characters. */
		  UNSIGNED max_length,/* Max number of characters. */
		  bool         sign,      /* Should I add a '-' ?  */
		  char * out_buffer       /* Out buffer. Should allow the representation of
					   * max_length+1 characters.  */
		  ) {
  // Local storage for converting integers to strings, counted in the
  // worst case. Attention - the stack must be large enough, especially
  // on a multi-core, where each core may do the same at the same
  // time, on its own stack.
  // The buffer has size UINT_BIT+2. The extra 2 digits are for the
  // optional sign and the trailing \0.
  char ascii_buf[UINT_BIT+2] ;
  
  // Construction of the result is done from the end, so I start by
  // setting up the last byte to \0.
  char *ptr = ascii_buf+UINT_BIT ;
  *ptr-- = '\0';
  int size = 0 ; // The number of generated characters
  
  // First check whether the base is good. It has to be at least 2,
  // but small enough to allow printing using characters '0'-'9' and
  // 'a'-'z'.
  if((base>=MIN_BASE)&&(base<=MAX_BASE)){
    // The base is correct. Perform the conversion.
    if(num) {
      // If the number is non-zero, then there are non-zero
      // digits
      while(num) {
	unsigned rem = num % base ;
	num = num / base ;
	if(rem<10) {
	  *ptr-- = '0' + rem ;
	} else {
	  rem -= 10 ;
	  *ptr-- = 'A' + rem ;
	}
	if(++size > max_length) goto error ;
      }
      // The sign is added only on numbers that are different
      // from 0, even if it is requested
      if(sign) {
	*ptr-- = '-';
	if(++size > max_length) goto error ;
      }
    } else {
      // No digits would be generated for this one using the
      // loop above. However, I still have to place a zero.
      *ptr-- = '0' ;
      if(++size > max_length) goto error ;
    }

    // Normalize the result for given size
    {
      // Normalize the length to avoid underflow.
      if(min_length > UINT_BIT) min_length = UINT_BIT ;
      // Add leading spaces, if needed
      while(min_length > size) {
	*ptr-- = ' ' ;
	if(++size > max_length) goto error ;
      }
    }
  } else {
    // Base is incorrect. Print a sign of error.
    goto error ;
  }
  
  // Reposition ptr to account for the last, useless ptr-- .
  // After this, ptr points on the start of the string.
  ptr++ ;
  memcpy(out_buffer,ptr,size+1) ;
  return size;

 error:
  // Any error results in a stunted string
  memcpy(out_buffer,"#",2) ;
  return 1;
}

//=======================================================================
// PRINTF - simple implementation
//=======================================================================

/* Partial implementation of vsnprintf. Only handles a couple of
 * format strings, sometimes not in the standard way. On errors,
 * introduces a '#' (and there should be enough place for it,
 * always). */
int vsnprintf(char* restrict buf,
	      size_t buf_size,
	      const char* restrict fmt,
	      va_list va) {
  // The number of actual characters (different from the
  // trailing \0) I can still write
  int remaining_size = buf_size - 1 ;
  // Write pointer in the output buffer
  char* result = buf ;
  // Aux character used to decode the format
  char ch;
  // Indicator of print failure, used to exit the printing
  // loop
  bool print_failure = false ;
  // Local buffer allowing the printing of one integer
  char local_buf[UINT_BIT+1] ;
  // Aux variable
  char* aux ;

  // Loop over the format string. Scanning stops when either
  // the end of format is reached, or when there is no place
  // left for output.
  while ((remaining_size>0) && (ch=*(fmt++)) && !print_failure ) {
    if (ch!='%') {  
      // No format string. Copy to output.
      *result++ = ch ;
      // Update available place
      remaining_size -- ;
    } else {
      // Format string. Read the rest of the format.
      int min_length = 0 ;
      ch=*(fmt++);
      // If I have a length requirement. Extract it, character
      // by character.
      while(('0'<=ch)&&('9'>=ch)) {
	min_length = min_length*10 + (int)(ch-'0') ;
	ch=*(fmt++);
      }
      int length ;
      UNSIGNED abs_value ;
      bool sign ;
      int base ; 
      switch (ch) {
      case 'd':
	{
	  int val = va_arg(va,int) ;
	  sign = (val < 0) ;
	  abs_value = (sign)?-val:val ;
	  base = 10 ;
	  goto doit ;
	}
      case 'x':
	abs_value = va_arg(va,UNSIGNED) ;
	sign = 0 ;
	base = 16 ;
	goto doit ;
      case 'u':
	abs_value = va_arg(va,UNSIGNED) ;
	sign = 0 ;
	base = 10 ;
	
      doit:	
	length = convert2ascii(abs_value,
			       base,
			       min_length,
			       UINT_BIT,
			       sign,
			       local_buf) ;
	if(remaining_size>=length) {
	  // I can fit the length of the hex string (without the
	  // Ox prefix, according to printf rules).
	  memcpy(result,local_buf,length) ;
	  result += length ;
	  remaining_size -= length ;
	} else {
	  // I cannot fit the length, end the processing
	  print_failure = true ;
	}
	break ;
      case 's':
	aux = va_arg(va, char*) ;
	length = strnlen(aux,remaining_size) ;
	if((length == remaining_size) && (aux[length] != '\0')) {
	  // I cannot print the full string, so I print nothing and
	  // complete the printing process
	  print_failure = true ;
	} else {
	  // I can fit the full string
	  memcpy(result,aux,length) ;
	  result += length ;
	  remaining_size -= length ;
	}
	break;
      default:
	print_failure = true ;
	break ;
      }
    }
  }

  // In case of failure, place a # sign to mark it.
  if(print_failure) {
    *result++ = '#' ;
  }

  // NULL-terminate the string
  *result = '\0' ;

  // Number of characters of output, not including \0
  return result-buf ; 
}

/* The sprintf function that is exported by the header file. 
 */
int snprintf(char* restrict buffer,
	     size_t buf_size,
	     const char * restrict fmt, ...) {
  va_list va;
  va_start(va,fmt);
  int i = vsnprintf(buffer,buf_size,fmt,va);
  va_end(va);
  return i ;
}

//=======================================================================
// SSCANF - simple implementation
//=======================================================================

static inline int is_space(char c) {
  return 
    (c == ' ') ||(c == '\t')||
    (c == '\r')||(c == '\n')||
    (c == '\12') ; // \12 is Form Feed
}

int convert_digit(char digit,int base) {
  int result = 0 ;
  if((digit>='0')&&(digit<='9')) {
    result = digit - '0';
  } else if((digit>='a')&&(digit<='f')) {
    result = digit - 'a' + 10 ;
  } else if((digit>='A')&&(digit<='F')) {
    result = digit - 'A' + 10 ;
  } else {
    // No such digit in any base
    return -1 ;
  }
  if(result>=base) return -1 ;
  //  debug_printf("convert_digit: base:%d found digit: %d\n",
  //	       base,result) ;
  return result ;
}

/* Very simple vsscanf implementation. Its format can
   include all ascii characters except those represented
   with escape sequences. The following
   specifiers are supported: %s %d %u %x %c.
   
   Space characters are a difficult issue. I assume
   that:
   - If the format contains a space character,
     then the string may contain zero or more 
     space characters.
   - The string may contain space characters before
     and after a specifier, even though the format
     does not.
 */

// I do not accept scanning %s strings longer than 128 characters.
#define MAX_STRING_SCANF 128
// Aux definitions
#define SKIP_SPACES(str) while(is_space(*str))str++
#define CHECK_STRING_END(str) if(*str=='\0') return scanned_items
// The function itself
int vsscanf(const char* restrict string, 
	    const char* restrict fmt,
	    va_list va) {
  char ch;
  int scanned_items = 0 ;
  char str[MAX_STRING_SCANF] ;

  while ((ch=*(fmt++))) {
    if(is_space(ch)) {
      //      debug_printf("\tsscanf(100)\n") ;
      // One or more spaces in the format allow
      // one or more spaces in the string.
      SKIP_SPACES(string) ;
      // As an optimization, skip all successive format spaces.
      SKIP_SPACES(fmt) ;
    } else {
      //      debug_printf("\tsscanf(200)\n") ;
      switch(ch) {
      case '%':
	//	debug_printf("\tsscanf(300)\n") ;
	// Should be a format specifier
	SKIP_SPACES(string) ;
	// No space can be left between % and the format
	// qualifier.
	CHECK_STRING_END(fmt) ;
	ch = *(fmt++) ;
	switch(ch) {
	case 'i':
	case 'd':
	case 'x':
	case 'u':
	  //	  debug_printf("\tsscanf(310)\n") ;
	  {
	    // Common start for the numerical types.
	    int sign = 1 ; //1 for positive, -1 for negative.
	    CHECK_STRING_END(string) ;
	    if(*string=='-') {
	      sign = -1 ;
	      string ++ ;
	    }
	    int base = 10 ;
	    // Check if it's hex or octal.
	    CHECK_STRING_END(string) ;
	    if(*string == '0') {
	      if(*(string+1)!='\0') {
		if((*(string+1) == 'x')||
		   (*(string+1) == 'X')) {
		  base = 16 ;
		  string += 2 ;
		} else {
		  if((*(string+1) >= '1')&&
		     (*(string+1) <= '7')) {
		    base = 8 ;
		    string += 1 ;
		  } 
		}
	      }
	    }
	    // Now, read the digits until there remain none
	    uint32_t number = 0 ;
	    int x ;
	    do {
	      CHECK_STRING_END(string) ;
	      x = convert_digit(*string,base) ;
	      if(x>=0) {
		number = number*base + x ;
		string ++ ;
	      }
	    } while(x>=0) ;
	    // I have finished reading the number
	    if((ch=='x')||(ch=='u')) {
	      uint32_t* p_result = (uint32_t*)va_arg(va,uint32_t*) ;
	      *p_result = number ;
	    } else {
	      int32_t* p_result = (int32_t*)va_arg(va,int32_t*) ;
	      *p_result = sign*number ;
	    }
	    scanned_items ++ ;
	  }
	  break ;
	case 's':
	  //	  debug_printf("\tsscanf(320)\n") ;
	  {
	    char * ptr = str ;
	    for(ptr = str ;
		((*string!='\0')
		 &&(!is_space(*string))
		 &&(ptr-str<MAX_STRING_SCANF-1));
		ptr++,string++){
	      *ptr = *string ;
	    }
	    *ptr = '\0';
	    char* p_result = (char*)va_arg(va,char*) ;

	    memcpy(p_result,str,strnlen(str,MAX_STRING_SCANF-1)+1) ;
	    scanned_items ++ ;
	  }
	  break ;
	case 'c':
	  //	  debug_printf("\tsscanf(330)\n") ;
	  {
	    CHECK_STRING_END(string) ;
	    char* p_result = (char*)va_arg(va,char*) ;
	    *p_result = *string++ ;
	    scanned_items ++ ;
	  }
	  break ;
	default:
	  //	  debug_printf("\tsscanf(340)\n") ;
	  // Unhandled format or error in the format.
	  return scanned_items ;
	}
	SKIP_SPACES(string) ;
	break ;
      case '\\':
	//	debug_printf("\tsscanf(400)\n") ;
	// An escape sequence
	CHECK_STRING_END(fmt) ;
	ch=*(fmt++) ;
	switch(ch) {
	case '\"':
	  CHECK_STRING_END(string) ;
	  if(*string++ != '\"') return scanned_items ;
	default:
	  // Unknown escape sequence
	  return scanned_items ;
	}
	break ;
      default:
	//debug_printf("\tsscanf(500)\n") ;
	// Other ASCII characters
	// I have to read the same in the string, or fail.
	CHECK_STRING_END(string) ;
	//	debug_printf("\tsscanf(500): compare %x and %x\n",
	//	     *string,ch) ;
	if(*string++ != ch) return scanned_items ;
      }
    }
  }
  // Finished the format string
  return scanned_items ;
}

/* The sprintf function that is exported by the header file. 
 */
int sscanf(const char* restrict string, 
	   const char * restrict fmt, ...) {
  va_list va;
  va_start(va,fmt);
  int i = vsscanf(string,fmt,va);
  va_end(va);
  return i ;
}

