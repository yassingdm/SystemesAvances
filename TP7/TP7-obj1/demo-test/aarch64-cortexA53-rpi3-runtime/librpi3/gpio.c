#include <stdint.h>
#include <librpi3/assert.h>
#include <librpi3/bcm2837.h>
#include <librpi3/gpio.h>

//================================================================
// GPIO programming registers.
// From:
// - BCM2835 ARM Peripheral manual
//================================================================

struct __attribute__((__packed__, aligned(4))) GPIORegisters {
  uint32_t GPFSEL[6];
  uint32_t reserved1;
  uint32_t GPSET[2];
  uint32_t reserved2;
  uint32_t GPCLR[2];
  uint32_t reserved3;
  const uint32_t GPLEV[2];
  uint32_t reserved4;
  uint32_t GPEDS[2];
  uint32_t reserved5;
  uint32_t GPREN[2];
  uint32_t reserved6;
  uint32_t GPFEN[2];
  uint32_t reserved7;
  uint32_t GPHEN[2];
  uint32_t reserved8;
  uint32_t GPLEN[2];
  uint32_t reserved9;
  uint32_t GPAREN[2];
  uint32_t reserved10;
  uint32_t GPAFEN[2];
  uint32_t reserved11;
  uint32_t GPPUD;
  uint32_t GPPUDCLK[2];
}; 

//----------------------------------------------------------------
// Static consistency tests on register layout
static_assert(sizeof(struct GPIORegisters) == 0xA0,
	      "Structure GPIORegisters should be 0xA0 bytes in size");

//================================================================
// Map the layouts at the good memory addresses.
// The addresses are defined in bcm2837.h
//================================================================
#define GPIO ((volatile struct GPIORegisters*)RPi_GPIO_BASE)


//================================================================
// GPIO routines
//================================================================

// Configure a GPIO port with a given mode.
// Return true if consistency checks succeed, but it does
// not check result.
bool gpio_setup (uint_fast8_t gpio, GPIOMODE mode) {
  // Check GPIO pin number valid, return false if invalid
  if (gpio >= 54) return false;
  // Check requested mode is valid, return false if invalid
  if (mode > GPIO_ALTFUNC3) return false;
  // Create bit mask
  uint_fast32_t bit = ((gpio % 10) * 3);
  // Read register
  uint32_t mem = GPIO->GPFSEL[gpio / 10];
  // Clear GPIO mode bits for that port
  mem &= ~(7 << bit);
  // Logical OR GPIO mode bits
  mem |= (mode << bit);
  // Write value to register
  GPIO->GPFSEL[gpio / 10] = mem; 
  return true; // Return true
}

// Output a logic level on a given port.
// Return true if consistency checks succeed, but it does
// not check result.
bool gpio_output (uint_fast8_t gpio, bool on) {
  // Check GPIO pin number valid, return false if invalid
  if (gpio >= 54) return false;
  // Register number
  uint_fast32_t regnum = gpio / 32;
  // Create mask bit
  uint_fast32_t bit = 1 << (gpio % 32);
  // Create temp pointer
  volatile uint32_t* p; 
  if (on) {
    // On == true means set
    p = &GPIO->GPSET[regnum]; 
  } else {
    // On == false means clear
    p = &GPIO->GPCLR[regnum];
  }
  // Set the bit
  *p = bit;	
  return true;
}

// Read the logic level of a GPIO port.


// Read the logic level of a GPIO port.
// Return true if consistency check succeeds and the
// input port is high. Returns false otherwise.
bool gpio_input (uint_fast8_t gpio) {
  // Check GPIO pin number valid, return false if invalid
  if (gpio >= 54) return false;
  // Register number
  uint_fast32_t regnum = gpio / 32;
  // Create mask bit
  uint_fast32_t bit = 1 << (gpio % 32);
  // Read level word
  uint32_t mem = GPIO->GPLEV[regnum];
  // Select only the bit that interests us
  if (mem & bit) return true;
  return false ;
}
