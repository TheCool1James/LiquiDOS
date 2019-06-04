/**
 * Copyright (c) 2019 The cxkernel authors. All rights reserved.
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 *
 * @file   debug.h
 * @author Kevin Dai \<kevindai02@outlook.com\>
 * @date   Created on October 27 2018, 11:41 AM
 */

#pragma once
#include "common.h"

__BEGIN_CDECLS

void initialize_debugger(void);
void breakpoint(void);

__END_CDECLS