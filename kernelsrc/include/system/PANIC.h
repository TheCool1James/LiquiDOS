/*
 * File:   PANIC.h
 * Author: Kevin
 *
 * Created on September 2, 2016, 6:08 PM
 */

#include "system.h"

#ifndef PANIC_H
#define PANIC_H

#ifdef __cplusplus
extern "C" {
#endif

void panic(const char *message);
void assert(bool assertStatus, const char *message);
#define PANIC panic
#define ASSERT assert

#ifdef __cplusplus
}
#endif

#endif /* PANIC_H */
