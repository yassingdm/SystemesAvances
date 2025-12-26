#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "tty.h"

//================================================================
// Initial configuration of the TTY.
int set_interface_attribs (char* ttyname, int speed, int parity) {
  struct termios tty;
  int fd ;

  fd = open (ttyname, O_RDWR | O_NOCTTY | O_SYNC);
  if (fd < 0) {
    perror("Abort: open error:") ;
    return -1;
  }
  
  bzero(&tty,sizeof(tty));
  if (tcgetattr (fd, &tty) != 0) {
    perror("Abort:tcgetattr(2) error:") ;
    close(fd) ; // cleanup
    return -4;
  }
  
  cfsetospeed (&tty, speed);
  cfsetispeed (&tty, speed);

  // Build up new configuration for TTY
  { 
    // 8-bit chars
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     
    // disable IGNBRK for mismatched speed tests;
    // otherwise receive break as \000 chars
    tty.c_iflag &= ~IGNBRK; // disable break processing
    tty.c_lflag = 0;        // no signaling chars, no echo,
    // no canonical processing
    tty.c_oflag = 0;        // no remapping, no delays
    tty.c_cc[VMIN]  = 0;    // read doesn't block
    tty.c_cc[VTIME] = READ_TIMEOUT;
    
    // shut off xon/xoff ctrl
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); 
    
    // ignore modem controls
    tty.c_cflag |= (CLOCAL | CREAD);
    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
  }

  // Set the new configuration
  if (tcsetattr (fd, TCSANOW, &tty) != 0) {
    perror("Abort: tcsetattr(2) error:") ;
    close(fd) ; // cleanup
    return -5 ;
  }

  printf("Opening tty %s succeeded.\n", ttyname) ;
  fflush(stdout) ;
  
  // Config succeeded, return the file descriptor
  return fd ;
}


//================================================================
// Reconfigure the TTY to change whether reading is blocking
// or not.
int set_blocking (int fd, int should_block) {
  struct termios tty;
  bzero(&tty,sizeof(tty));
  if (tcgetattr (fd, &tty) != 0) {
    perror("Abort: tcgetattr error:");
    return -2;
  }
  tty.c_cc[VMIN]  = should_block ? 1 : 0;
  tty.c_cc[VTIME] = READ_TIMEOUT;
  if (tcsetattr (fd, TCSANOW, &tty) != 0){
    perror("Abort: tcsetattr error:") ;
    return -3 ;
  }
  return 0 ; // success
}
