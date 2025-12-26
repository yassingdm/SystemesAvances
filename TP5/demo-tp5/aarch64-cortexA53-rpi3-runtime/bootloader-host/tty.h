// This TTY code is inspired fom here:
//   https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
// I will have to restore the PL011 UART on the Raspberry Pi 3
// as explained here:
// - https://www.raspberrypi.org/forums/viewtopic.php?p=926637
// - https://github.com/raspberrypi/linux/commit/9f160c14d58a393a160f24f528d238ef44565e61
// by setting the overlay dtoverlay=pi3-miniuart-bt-overlay.
// Thus, the same code should work on both RPi1 and RPi3.


#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// If set, the timeout on individual read operations
// will be 1 seconds
#define READ_TIMEOUT 5

//================================================================
// Initial configuration of the TTY.
// - open the file, checking it exists
// - check it's really a TTY
// - perform its initial configuration
// Return value is one of two:
// - A positive file descriptor.
// - A negative error code. In this case, it also prints an error
//   message on stderr.
int set_interface_attribs (char* ttyname, int speed, int parity) ;

//================================================================
// Reconfigure the TTY to change whether reading is blocking
// or not.
//int set_blocking (int fd, int should_block) ;
