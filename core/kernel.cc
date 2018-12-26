/**
 * @file   kernel.c
 * @author Kevin Dai \<kevindai02@outlook.com\>
 * 
 * @date Created on Sunday, October 7th 2018, 4:12:12 pm
 * 
 * @date Last modified by:   Kevin Dai
 * @date Last modified time: 2018-11-13T21:07:22-05:00
 */

#include "platform.h"

// Random ctor shit
using ctor_func = void (*)();
extern ctor_func _ctors_start;
extern ctor_func _ctors_end;
__NO_OPTIMIZE __NOINLINE void dummy_ctor(void) { } EXPORT_CTOR(dummy_ctor);

extern "C" void kernel_main(int sig, void* ptr)
{
    // Execute ctors, these really only initializes printf
    for(ctor_func* func_arr = &_ctors_start; func_arr != &_ctors_end; func_arr++) (*func_arr)();

    platform::get_console().clear();
    arch::early_init(sig, ptr);
    platform::early_init();
    platform::init();    
    arch::init();
    
    for(;;);
}
