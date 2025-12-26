#include <stdint.h>
#include <stddef.h>

typedef struct { int _dummy; } Print__print_fast_out;
typedef struct { int _dummy; } Print__print_gnc_out;
typedef struct { int _dummy; } Print__print_thermal_out;

typedef struct { int _dummy; } Print__print_fast_mem;
typedef struct { int _dummy; } Print__print_gnc_mem;
typedef struct { int _dummy; } Print__print_thermal_mem;



void Print__print_fast_reset(Print__print_fast_mem *self);
void Print__print_fast_step(int idx, int x_in, int y_out,
                            Print__print_fast_out *out);


void Print__print_gnc_reset(Print__print_gnc_mem *self);
void Print__print_gnc_step(int idx, int y_in, int x_out,
                           Print__print_gnc_out *out);


void Print__print_thermal_reset(Print__print_thermal_mem *self);
void Print__print_thermal_step(int idx,
                               Print__print_thermal_out *out);
