/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   scanmap.h
 * Author: Kevin
 *
 * Created on September 17, 2016, 10:48 AM
 */

#ifndef SCANMAP_H
#define SCANMAP_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Here, we map all the scancodes to a specific range. So the number range would be 0x02 -> 0x0B which is 1234567890
 * And then we map the non-letter characters, and then finally we map the lettered characters. All the mapped characters
 * have a corresponding ASCII char value to it, which can be found in the char array below
 */
enum EN_SCANCODES
{
    FIRST_NUMBER = 0x02,         //1
    LAST_NUMBER = 0x0B,          //0
    DASH = 0x0C,                 //-
    EQUALS = 0x0D,               //=
    FIRST_KEY_TOP_ROW = 0x10,    //q
    LAST_KEY_TOP_ROW = 0x19,     //p
    FIRST_KEY_MIDDLE_ROW = 0x1E, //a
    LAST_KEY_MIDDLE_ROW = 0x26,  //l
    FIRST_KEY_BOTTOM_ROW = 0x2C, //z
    LAST_KEY_BOTTOM_ROW = 0x32,  //m
    
    //If you want to remap, this is the range you probably want to remap
    TC_S = 0x1A, TC_E = 0x1B, // Top char starts and ends
    MC_S = 0x27, MC_E = 0x29, // Middle row chars...
    BC_S = 0x33, BC_E = 0x39, // Bottom row
    
    SCH = 0x2B, // Special character (*_forward)
    
    LSHIFT = 0x2A, RSHIFT = 0x36, //The modifiers (shifts)
    SPACE = 0x39, ENTER = 0x1C,
    BCKSPACE = 0x0E, CAPS = 0x3A
};

#ifdef __cplusplus
}
#endif

#endif /* SCANMAP_H */

