/*
 * File:   llio.c
 * Author: Kevin Dai
 * Email:  kevindai02@outlook.com
 *
 * Created on 29-Jul-2017 10:50:27 PM
 *
 * @ Last modified by:   Kevin Dai
 * @ Last modified time: 2017-11-26T12:39:52-05:00
*/

#include "arch/x86/llio.h"

inline void io_wait(void)
{
    // Port 0x80 is used for 'checkpoints' during POST
    // The Linux kernel seems to think it is free for use :-/
    //// %%al instead of %0 makes no difference. Does the register need to be zeroed?
    asm volatile ("outb %%al, $0x80" : : "a" (0));
}

inline void outb(uint16_t port, uint8_t value)
{
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

inline void outw(uint16_t port, uint16_t value)
{
    asm volatile ("outw %1, %0" : : "dN" (port), "a" (value));
}

inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

inline uint32_t read_cr0(void)
{
    uint32_t val;
    asm volatile ("mov %%cr0, %0" : "=r" (val));
    return val;
}

inline uint32_t read_cr1(void)
{
    uint32_t val;
    asm volatile ("mov %%cr1, %0" : "=r" (val));
    return val;
}

inline uint32_t read_cr2(void)
{
    uint32_t val;
    asm volatile ("mov %%cr2, %0" : "=r" (val));
    return val;
}

inline uint32_t read_cr3(void)
{
    uint32_t val;
    asm volatile ("mov %%cr3, %0" : "=r" (val));
    return val;
}

inline uint32_t read_cr4(void)
{
    uint32_t val;
    asm volatile ("mov %%cr4, %0" : "=r" (val));
    return val;
}
