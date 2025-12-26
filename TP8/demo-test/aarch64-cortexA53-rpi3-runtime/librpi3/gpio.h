#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>
#include <stdint.h>

//=============================================================
// GPIO mode, needed when configuring it.
//=============================================================
typedef enum {
  GPIO_INPUT    = 0b000,
  GPIO_OUTPUT   = 0b001,
  GPIO_ALTFUNC5 = 0b010,
  GPIO_ALTFUNC4 = 0b011,
  GPIO_ALTFUNC0 = 0b100,
  GPIO_ALTFUNC1 = 0b101,
  GPIO_ALTFUNC2 = 0b110,
  GPIO_ALTFUNC3 = 0b111,
} GPIOMODE;

//=============================================================
// GPIO access functions
//=============================================================

// Configure a GPIO port with a given mode.
// Return true if consistency checks succeed, but it does
// not check result.
bool gpio_setup (uint_fast8_t gpio, GPIOMODE mode);

// Output a logic level on a given port.
// Return true if consistency check succeeds, but it does
// not check result.
bool gpio_output (uint_fast8_t gpio, bool on);

// Read the logic level of a GPIO port.
// Return true if consistency check succeeds and the
// input port is high. Returns false otherwise.
bool gpio_input (uint_fast8_t gpio);

#endif
