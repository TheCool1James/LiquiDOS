/**
 * @file   idt.h
 * @author Kevin Dai \<kevindai02@outlook.com\>
 * 
 * @date Created on Sunday, November 26th 2017, 9:42:00 pm
 * 
 * @date Last modified by:   Kevin Dai
 * @date Last modified time: 2018-10-27T20:02:41-04:00
 */

#pragma once

#include "common.h"
#include "arch/arch_types.h"

// Define each IRQ so its more organized
#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

typedef struct
{
    uint16_t base_lo;             //!< The lower 16 bits of the address to jump to when this interrupt fires.
    uint16_t sel;                 //!< Kernel segment selector.
    uint8_t  always0;             //!< This must always be zero.
    uint8_t  flags;               //!< More flags. See Intel developers manual.
    uint16_t base_hi;             //!< The upper 16 bits of the address to jump to.
} __PACKED idt_entry_t;

typedef struct
{
    uint16_t limit;
    uint32_t base;
} __PACKED idt_ptr_t;

typedef struct regs
{
    uint32_t gs, fs, es, ds;                             //!< Pushed the segs last
    uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax; //!< Pushed by 'pusha'
    uint32_t int_no, err_code;                           //!< Our 'push byte #' and ecodes do this
    uint32_t eip, cs, eflags, esp, ss;                   //!< Pushed by the processor automatically
} __PACKED regs_t;

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*irq_t) (regs_t *r);

void init_idt(void);
void init_irq(void);
void idt_set_gate(int idx, uint32_t base, uint16_t sel, uint8_t flags);

void install_irqhandler(int irq, irq_t handler);
void uninstall_irqhandler(int irq);

#ifdef __cplusplus
}
#endif
