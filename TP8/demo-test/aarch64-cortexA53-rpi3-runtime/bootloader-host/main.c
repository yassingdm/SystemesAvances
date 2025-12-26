#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h> // For mmap
#include <stdlib.h>   // For random
#include <time.h>     // For time
#include <signal.h>   // To catch signals
#include "tty.h"
#include "../bootloader-rpi-fixed/magic.h"
#include "../bootloader-rpi-fixed/loader.h"
#include "../librpi3/crc16.c"


//=======================================================
// Constants and global declarations used by threads.

// When in manual mode, these provide default names for
// some constants and file names
const char default_load_file[] = "boot.img" ;
const uint32_t default_load_addr = 0x8000 ;
const char default_script[] = "load.cfg" ;

// The tty file descriptor
int global_tty_fd ;
// When in echo mode, this is the thread performing the echo
// operation.
pthread_t t_rpi ;
// Mutex used to synchronize the end of RPI operations with
// the control on the host side.
pthread_mutex_t op_wait_lock = PTHREAD_MUTEX_INITIALIZER ;

// Needed to create the thread
struct tty_rpi_args rargs ;


//=======================================================
// Printf routine with mutual exclusion for threads.
pthread_mutex_t printf_lock = PTHREAD_MUTEX_INITIALIZER ;
#define PRINTF(fmt, ...) \
  pthread_mutex_lock(&printf_lock);   \
  fprintf(stderr,fmt, ##__VA_ARGS__); \
  pthread_mutex_unlock(&printf_lock)

//=======================================================
// Sync function.
void tty_sync(int tty_fd) {
  // Init random number generator, and choose a random
  // byte that is different from 0.
  srand((unsigned) time(NULL));
  unsigned char flush_random_magic = (unsigned char)rand() ;
  if(flush_random_magic == 0) flush_random_magic+=0x11 ;

  // Buffer flush routine - writing part.
  // Start by sending a sequence of 4 bytes. The first one
  // is the random one, the following are obtained by
  // increasing by 1. The chance to have it randomly is
  // slim.
  {
    PRINTF("SYNC: start flush writing\n") ;
    unsigned char i ;
    for(i=0;i<4;i++) {
      unsigned char c1 = flush_random_magic+i ;
      //PRINTF("SYNC: writing %u\n",(unsigned int)c1) ;
      write(tty_fd,&c1,1) ;
    }
    PRINTF("SYNC: finished writing\n") ;
  }

  // Buffer flush routine - reading part.
  // Wait until I receive the same sequence I sent. At that
  // moment, I know that both outgoing and inbound buffers
  // are flushed (empty).
  {
    PRINTF("SYNC: flush reading: start\n") ;
    unsigned char a ;
    unsigned char b = 0 ;
    do {
      switch(read (tty_fd, &a, 1)){
      case 1:
	//PRINTF("SYNC: read %u\n",(unsigned int)a) ;	
	if(a == (flush_random_magic+b))
	  b++ ;   // If it's the good character, wait for the next
	else
	  b = 0 ; // If it's not, then reset the process
	break ;
      case 0:
	// Error, but no error code
	// PRINTF("SYNC:tty_reader reads 0 bytes - error.\n") ;
	break ;
      default:
	// Error
	PRINTF("SYNC:tty_reader error.\n") ;
	break ;
      }
    } while(b<4) ;
    PRINTF("SYNC: FLUSH COMPLETE\n") ;
  }

  // Signal to the RPi that we can now enter in sync
  // (the RPi does not yet know we flushed the buffers).
  // Write a pre-established magic word.
  {
    PRINTF("SYNC: sync with RPi write: start.\n") ;
    int i ;
    // also write the trailing \0
    for(i=0;i<=strlen(write_magic);i++) {
      write(tty_fd,write_magic+i,1) ;
    }
    PRINTF("SYNC: sync with RPi write: finished.\n") ;    
  }

  // Wait for the RPi to signal that it understood, by
  // reading a pre-established magic word.
  {
    PRINTF("SYNC: sync reading: start\n") ;
    unsigned char a ;
    int b = 0 ;
    do {
      //PRINTF("SYNC: start reading") ;
      switch(read (tty_fd, &a, 1)){
      case 1:
	//PRINTF("SYNC: read %u\n",(unsigned int)a) ;	
	if(a == read_magic[b]) b++ ;
	else b = 0 ;
	break ;
      case 0:
	// Error, but no error code
	// PRINTF("SYNC:tty_reader reads 0 bytes - error.\n") ;
	break ;
      default:
	// Error
	PRINTF("SYNC:tty_reader error.\n") ;
	break ;
      }
    } while(b<=strlen(read_magic)) ;
    PRINTF("SYNC: SYNC COMPLETE\n") ;
  }
  return ;
}

//=======================================================
// This thread reads ASCII text from the RPi and prints
// it, line by line. It is dissociated from the main 
// thread to avoid blocking.
struct tty_rpi_args {
  int tty_fd ;
} ;
#define BUFSIZE 256
void* tty_rpi(void*pargs) {
  struct tty_rpi_args* args = (struct tty_rpi_args*)pargs ;

  // Transfer data to screen in echo mode
  char c ;
  char linebuf[BUFSIZE] ;
  int  lineptr = 0 ;
  for(;;) {
    switch(read (args->tty_fd, &c, 1)){
    case 1:
      // puts(ascii_print(c)) ;
      // fflush(stdout) ;
      if(c==rpi_completed) {
	// PRINTF("RPI: completed marker\n") ;
	pthread_mutex_unlock(&op_wait_lock) ;
      } else {      
	linebuf[lineptr] = c ;
	lineptr ++ ;
	if(c == 0) {
	  // Remove the character
	  lineptr -- ;
	  
	  // PRINTF("RPI:%s",linebuf) ;
	  // lineptr = 0 ;
	} else if(c == '\n') {
	  linebuf[lineptr] = 0 ;
	  PRINTF("RPI:%s",linebuf) ;
	  lineptr = 0 ;
	}
	// Line has overflowed
	if(lineptr == 80) {
	  linebuf[lineptr] = 0 ;
	  PRINTF("RPI:%s\n",linebuf) ;
	  lineptr = 0 ;
	}
      }
      break ;
    case 0:
      // Nothing to read - some timeout elapsed
      break ;
    default:
      // Error
      PRINTF("RPI thread: error.\n") ;
      break ;
    }
  }
}

//=======================================================
// Cleanup routine. It can be called at any point,
// and since the main thread is always waiting for
// input, I can always shut down everything in a clean
// way (the tty does not remain locked).
//
// It is called in case of error, and also when the
// program receives the SIGINT signal
void cleanup_handler(int unused) {
  PRINTF("Exiting - cleanup routine called...\n") ;
  pthread_cancel(t_rpi) ;
  close(global_tty_fd) ;
  exit(-1) ;
}


void action_sync() {     
  PRINTF("===========================================================\n") ;
  // Synchronize with a freshly-started RPi
  // Stop the thread that prints everything coming from RPi
  pthread_cancel(t_rpi) ;
  // Perform the sync routine
  tty_sync(global_tty_fd) ;
  // Restart the thread that prints everything coming from RPi
  if(pthread_create(&t_rpi,NULL,tty_rpi,(void*)&rargs)) {
    perror("pthread_create (1):") ;
    cleanup_handler(0) ;
  }
}

//=======================================================
// 
void action_str(char* buf) {
  PRINTF("===========================================================\n") ;
  // Some printing
  PRINTF("SENDING string %s\n",buf) ;

  // Send a string towards the RPi, including the
  // trailing \0
  char sendbuf[64] ;
  sendbuf[0] = DATA_STRING ;
  strncpy(sendbuf+1,buf,62) ;
  sendbuf[63] = 0 ; // Make sure it's NULL-terminated
  
  int i ;
  for(i=0;i<=strlen(sendbuf);) {
    switch(write(global_tty_fd,sendbuf+i,1)) {
    case 1:
      i++ ;
      break ;
    case 0:
      break ;
    default:
      perror("error writing string:") ;
      cleanup_handler(0) ;
    }
  }
}


//=======================================================
// 
void action_u32(uint32_t u32) {
  PRINTF("===========================================================\n") ;
  // Some printing
  PRINTF("SENDING uint32_t 0x%x\n",u32) ;
  
  unsigned char data[5] ;
  data[0] = DATA_UINT32 ;
  data[1] = (unsigned char)(u32&0xff) ;
  data[2] = (unsigned char)((u32>>8)&0xff) ;
  data[3] = (unsigned char)((u32>>16)&0xff) ;
  data[4] = (unsigned char)((u32>>24)&0xff) ;
  int i ;
  for(i=0;i<5;) {
    switch(write(global_tty_fd,data+i,1)) {
    case 1:
      i++ ;
      break ;
    case 0:
      break ;
    default:
      perror("error writing u32:") ;
      cleanup_handler(0) ;
    }
  }
}

//=======================================================
// 
void action_run(uint32_t addr) {
  // Some printing
  PRINTF("===========================================================\n") ;
  PRINTF("RUN command with address 0x%x\n",addr) ;

  // Send the value
  unsigned char data[5] ;
  data[0] = CMD_RUN ;
  data[1] = (unsigned char)(addr&0xff) ;
  data[2] = (unsigned char)((addr>>8)&0xff) ;
  data[3] = (unsigned char)((addr>>16)&0xff) ;
  data[4] = (unsigned char)((addr>>24)&0xff) ;
  int i ;
  for(i=0;i<5;) {
    switch(write(global_tty_fd,data+i,1)) {
    case 1:
      i++ ;
      break ;
    case 0:
      break ;
    default:
      perror("error writing u32:") ;
      cleanup_handler(0) ;
    }
  }
}


//=======================================================
// 
void action_send_file(char*fname,uint32_t addr) {     
  PRINTF("===========================================================\n") ;
  // Read the file in RAM and compute CRC16
  {
    int send_fd = open(fname,O_RDONLY) ;
    if(send_fd<0) {
      perror("Failed to open file.") ;
      return ;
    }
    off_t  size = lseek(send_fd, 0, SEEK_END);
    lseek(send_fd, 0, SEEK_SET);
    unsigned char *filebuf = mmap(NULL, size, PROT_READ, MAP_PRIVATE, send_fd, 0);
	
    {
      // Some printing
      unsigned int crc = crc16(filebuf,size) ;
      PRINTF("UPLOAD %s to  ADDR:0x%x SIZE:0x%lx CRC: 0x%x\n",fname,addr,(unsigned long)size,crc) ;
    }
    
    // Actual sending
    {
      unsigned char data[9] ;
      data[0] = DATA_FILE ;
      data[1] = (unsigned char)((addr>> 0)&0xff) ;
      data[2] = (unsigned char)((addr>> 8)&0xff) ;
      data[3] = (unsigned char)((addr>>16)&0xff) ;
      data[4] = (unsigned char)((addr>>24)&0xff) ;
      
      data[5] = (unsigned char)((size>> 0)&0xff) ;
      data[6] = (unsigned char)((size>> 8)&0xff) ;
      data[7] = (unsigned char)((size>>16)&0xff) ;
      data[8] = (unsigned char)((size>>24)&0xff) ;
      
      int i ;
      for(i=0;i<9;) {
	switch(write(global_tty_fd,data+i,1)) {
	case 1:
	  i++ ;
	  break ;
	case 0:
	  break ;
	default:
	  perror("error writing u32:") ;
	  cleanup_handler(0) ;
	}
      }
      usleep(10000) ;

      PRINTF("1ko blocks:");
      for(i=0;i<size;) {
	switch(write(global_tty_fd,filebuf+i,1)) {
	case 1:
	  i++ ;
	  // The following test introduces the needed delay
	  // so that the writing does not overflow.
	  if((i & 0b1111111111) == 0) {
	    PRINTF("X");
	  }
	  break ;
	case 0:
	  break ;
	default:
	  perror("error writing u32:") ;
	  cleanup_handler(0) ;
	}
      }
      PRINTF("\n") ;
    }
    // Deallocate the mmap
    munmap(filebuf,size) ;
  }
}

//=======================================================
//
void action_script(char*fname) {
  PRINTF("EXECUTING SCRIPT %s\n",fname) ;

  // Open the script file
  FILE* fis = fopen(fname,"r") ;
  if(fis == NULL) {
    perror("Failed to open script file.") ;
    return ;
  }

  while(!feof(fis)) {
    char str[128] ;
    fscanf(fis,"%s",str) ;
    if(strcmp(str,"sync") == 0) {
      action_sync() ;
      
    } else if(strcmp(str,"file") == 0) {
      // Send a file towards the RPi, at a specific address     
      uint32_t addr ;
      // Read the uploading address
      {
	unsigned int addr_u ;
	if(fscanf(fis,"%x",&addr_u) != 1) {
	  PRINTF("Error reading config file.\n") ;
	  goto out ;
	}
	addr = addr_u ;
      }
      // Read the file name 
      {
	if(fscanf(fis,"%s",str) != 1) {
	  PRINTF("Error reading config file.\n") ;
	  goto out ;
	}
      }

      pthread_mutex_lock(&op_wait_lock) ;
      action_send_file(str,addr) ;
      pthread_mutex_lock(&op_wait_lock) ;
      pthread_mutex_unlock(&op_wait_lock) ;

    } else if(strcmp(str,"run") == 0) {
      // Send an uint32_t address towards the RPi
      // and branch to it.

      uint32_t addr ;
      // Read the uploading address
      {
	unsigned int addr_u ;
	if(fscanf(fis,"%x",&addr_u) != 1) {
	  PRINTF("Error reading config file.\n") ;
	  goto out ;
	}
	addr = addr_u ;
      }

      pthread_mutex_lock(&op_wait_lock) ;
      action_run(addr) ;
      pthread_mutex_lock(&op_wait_lock) ;
      pthread_mutex_unlock(&op_wait_lock) ;
      
    } else {
      PRINTF("Error reading config file.\n") ;
      goto out ;
    }
  }
 out:
  fclose(fis) ;
  return ;
}


//=======================================================
// Main routine, which is actually quite simple
int main(int argc, char**argv) {
  // Minor sanity check
  if(argc != 2) {
    printf("Wrong argument number. "
	   "Expected only the TTY file name, "
	   "but received %d arguments\n",argc) ;
    exit(0) ;
  }

  // First action is to set up the interrupt handler. 
  // What we do here can easily hang up the computer, so
  // we need to avoid it.
  signal(SIGINT,cleanup_handler) ;
  
  // Initialize the TTY used to communicate with the reader
  // and writer threads in MODE_ECHO mode.
  char *portname = argv[1] ;
  printf("TTY file to open: %s\n", portname);
  global_tty_fd = set_interface_attribs (portname, B115200, 0);
  if(global_tty_fd < 0) {
    printf("Failed to open TTY device %s. Exiting...\n", portname);
    exit(-1) ;
  }
  
  // Fill in the pthreads args structures and create printer
  // thread
  rargs.tty_fd = global_tty_fd ;
  if(pthread_create(&t_rpi,NULL,tty_rpi,(void*)&rargs)) {
    perror("pthread_create:") ;
    cleanup_handler(0) ;
  }

  // Global loop 
  for(;;) {
    char command[64] ;
    printf("COMMAND:"); fflush(stdout) ;
    fgets(command,63,stdin) ;
    command[strnlen(command,62)-1] = 0 ;
    if(strcmp(command,"script") == 0) {
      printf("\tScript to execute(%s):",default_script) ; fflush(stdout) ;
      char buf[128] ; buf[127] = 0 ;
      fgets(buf,127,stdin) ;
      buf[strnlen(buf,127)-1] = 0 ; // Remove the trailing \n

      if(strnlen(buf,127) == 0) {
	strcpy(buf,default_script) ;
      }
      action_script(buf) ;
      
    } else if(strcmp(command,"sync") == 0) {

      action_sync() ;
      
    } else if(strcmp(command,"str") == 0) {
      printf("\tString to transmit:") ; fflush(stdout) ;

      char buf[64] ;
      fgets(buf,62,stdin) ;
      buf[strnlen(buf,62)-1] = 0 ; // Remove the trailing \n

      pthread_mutex_lock(&op_wait_lock) ;
      action_str(buf) ;
      pthread_mutex_lock(&op_wait_lock) ;
      pthread_mutex_unlock(&op_wait_lock) ;

    } else if(strcmp(command,"u32") == 0) {
      // Send an uint32_t towards the RPi
      printf("\tValue to send:") ; fflush(stdout) ;
      fgets(command,63,stdin) ;
      unsigned int u ;
      sscanf(command,"%x",&u) ;
      uint32_t u32 = u ;

      pthread_mutex_lock(&op_wait_lock) ;
      action_u32(u32) ;
      pthread_mutex_lock(&op_wait_lock) ;
      pthread_mutex_unlock(&op_wait_lock) ;
      
    } else if(strcmp(command,"file") == 0) {
      // Send a file towards the RPi, at a specific address
      
      uint32_t addr ;
      {
	fprintf(stdout,"\taddress (0x%x):",default_load_addr) ;
	fflush(stdout) ;
	if(fgets(command,63,stdin)) {
	  unsigned int addr_u ;
	  if(sscanf(command,"%x",&addr_u) != 1) {
	    addr_u = 0x8000 ;
	  }
	  addr = addr_u ;
	} else {
	  perror("error in fgets:") ;
	  cleanup_handler(0) ;
	}
      }
      
      char fname[128] ;
      {
	fprintf(stdout,"\tname (%s):",default_load_file) ;
	fflush(stdout) ;
	if(fgets(command,63,stdin)) {
	  if(sscanf(command,"%s",fname) != 1) {
	    strcpy(fname,default_load_file) ;
	  }
	} else {
	  perror("error in fgets(2):") ;
	  cleanup_handler(0) ;
	}
      }

      pthread_mutex_lock(&op_wait_lock) ;
      action_send_file(fname,addr) ;
      pthread_mutex_lock(&op_wait_lock) ;
      pthread_mutex_unlock(&op_wait_lock) ;

    } else if(strcmp(command,"run") == 0) {
      // Send an uint32_t address towards the RPi
      // and branch to it.

      uint32_t u32 ;
      {
	fprintf(stdout,"\taddress (0x%x):",default_load_addr) ;
	fflush(stdout) ;
	if(fgets(command,63,stdin)) {
	  unsigned int addr_u ;
	  if(sscanf(command,"%x",&addr_u) != 1) {
	    addr_u = 0x8000 ;
	  }
	  u32 = addr_u ;
	} else {
	  perror("error in fgets(3):") ;
	  cleanup_handler(0) ;
	}
      }

      pthread_mutex_lock(&op_wait_lock) ;
      action_run(u32) ;
      pthread_mutex_lock(&op_wait_lock) ;
      pthread_mutex_unlock(&op_wait_lock) ;
      
    } else if(strcmp(command,"quit") == 0) {
      PRINTF("NORMAL EXIT.\n") ;
      cleanup_handler(0) ;
      
    } else {
      PRINTF("ERROR: unrecognized command %s\n",command) ;
    }
  }
}
