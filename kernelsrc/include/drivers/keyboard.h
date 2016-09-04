/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   keyboard.h
 * Author: Kevin
 *
 * Created on August 26, 2016, 9:05 AM
 */

#include "common.h"
#include "arch/exceptions.h"
#include "system/irq.h"
#include "system/tdisplay.h"


#ifndef KEYBOARD_H
#define KEYBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

void register_keyboard();

#ifdef __cplusplus
}
#endif

#endif /* KEYBOARD_H */

