/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "system/PANIC.h"

extern void panic(const char *message, const char *file, uint32_t line)
{
    // We encountered a massive problem and have to stop.
    asm volatile("cli"); // Disable interrupts.

    console_write("PANIC (");
    console_write(message);
    console_write(") at ");
    console_write(file);
    console_write(":");
    console_write_dec(line);
    console_write("\n");
    console_write("Kernel panic can be fixed (usually) by a restart. CPU is going to halt now...");
    // Halt by going into an infinite loop.
    halt();
}

extern void panic_assert(const char *file, uint32_t line, const char *desc)
{
    // An assertion failed, and we have to panic.
    asm volatile("cli"); // Disable interrupts.

    console_write("ASSERTION-FAILED(");
    console_write(desc);
    console_write(") at ");
    console_write(file);
    console_write(":");
    console_write_dec(line);
    console_write("\n");
    // Halt by going into an infinite loop.
    halt();
}