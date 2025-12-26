#ifndef LOADER_H
#define LOADER_H

// Types of objects transmitted from host to RPi
// through UART
enum data_kind {
  DATA_STRING = 1, // NULL-terminated string
  DATA_UINT32 = 2, // 4-byte unsigned int, little endian
  DATA_FILE = 3,   // file with size and destination addr
  CMD_RUN = 4,     // run command with target address
} ;

//==============================================================
// Loader protocol function 
__attribute__((noreturn))
void loader_protocol(void) ;

#endif
