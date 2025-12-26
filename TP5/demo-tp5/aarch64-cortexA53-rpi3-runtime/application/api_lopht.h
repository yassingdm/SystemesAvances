#ifndef API_H
#define API_H

#include <stdint.h>
#include <librpi3/semaphore.h>
#include <stdbool.h>

#define UPDATE_CPU(var,val) api_stlr(&var,val)
#define WAIT_CPU(var,val) while(api_ldar(&var)<val) 
#define WAIT_END(var) while(api_ldar(&var)!=-1)
#define BRANCH(fun) fun()

typedef int LOPHT_MIF_CNT_TYPE ;
typedef int64_t LOPHT_PC_TYPE ;

#define SECTION_ATTR(str) 
#define DATA_SECTION_ATTR(str) 

#endif
