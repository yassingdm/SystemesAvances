// Magic to sync with the RPi part. The first string
// is written towards the Pi, and sync is completed when
// I receive both the first and the second.
const char write_magic[] = "Big" ;
const char read_magic[] = "Deal" ;

// This value is sent from RPi to the host to notify the
// end of an operation. Since the RPi only sends ASCII
// (not data) this is OK.
// All operations except SYNC and RUN acknowledge their
// end.
const char rpi_completed = (char)255 ;
