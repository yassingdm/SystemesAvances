#include <stdint.h>
#include <stddef.h>
#include <librpi3/crc16.h>

#define CRC16 0x8005

uint16_t crc16(const uint8_t *data, uint32_t size) {
  uint16_t out = 0;
  int bits_read = 0, bit_flag;
  
  /* Sanity check: */
  if(data == NULL)
    return 0;
  
  while(size > 0) {
    bit_flag = out >> 15;
    
    /* Get next bit: */
    out <<= 1;
    out |= (*data >> (7 - bits_read)) & 1;
    
    /* Increment bit counter: */
    bits_read++;
    if(bits_read > 7)
      {
	bits_read = 0;
	data++;
	size--;
      }
    
    /* Cycle check: */
    if(bit_flag)
      out ^= CRC16;
  }
  
  return out;
}
