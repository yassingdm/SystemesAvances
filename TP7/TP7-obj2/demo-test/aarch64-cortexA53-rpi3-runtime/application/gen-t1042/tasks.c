#include "tasks.h"
#include <librpi3/svc.h>
#include <librpi3/stdio.h>

void task0(void) {
    char buf[64];
    snprintf(buf, 63, "Core %d : Task 0 (Period 7)\n", (int)get_cpuid());
    console_puts(get_cpuid(), buf);
    for(volatile int i=0; i<5000; i++);
}

void task1(void) {
    char buf[64];
    snprintf(buf, 63, "Core %d : Task 1 (Period 5)\n", (int)get_cpuid());
    console_puts(get_cpuid(), buf);
    for(volatile int i=0; i<5000; i++);
}

void task2(void) {
    char buf[64];
    snprintf(buf, 63, "Core %d : Task 2 (Period 6)\n", (int)get_cpuid());
    console_puts(get_cpuid(), buf);
    for(volatile int i=0; i<5000; i++);
}

void task3(void) {
    char buf[64];
    snprintf(buf, 63, "Core %d : Task 3 (Period 8)\n", (int)get_cpuid());
    console_puts(get_cpuid(), buf);
    for(volatile int i=0; i<5000; i++);
}

void task4(void) {
    char buf[64];
    snprintf(buf, 63, "Core %d : Task 4 (Period 10)\n", (int)get_cpuid());
    console_puts(get_cpuid(), buf);
    for(volatile int i=0; i<5000; i++);
}