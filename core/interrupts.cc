/**
 * Copyright (c) 2019 The cxkernel Authors. All rights reserved.
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 *
 * @file   interrupts.cc
 * @author Kevin Dai \<kevindai02@outlook.com\>
 * @date   Created on January 03 2019, 2:54 PM
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "platform.h"
#include "platform/interrupts.h"
using namespace platform;

void Irq::install_handler(int n, irq_handler_t handler) { }
void Irq::remove_handler(int n) { }
irq_handler_t Irq::get_handler(int n) { }

extern "C" void irq_install_handler(int n, irq_handler_t handler)
{
    platform::get_irq().install_handler(n, handler);
}

extern "C" void irq_remove_handler(int n)
{
    platform::get_irq().remove_handler(n);
}

extern "C" irq_handler_t irq_get_handler(int n)
{
    platform::get_irq().get_handler(n);
}