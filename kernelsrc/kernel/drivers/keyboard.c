/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "arch/exceptions.h"
#include "system/irq.h"
#include "system/tdisplay.h"

#include "drivers/keyboard.h"
#include "localization/en_scan.h"

bool SFLAG = false; // Shift flag
bool CAPSF = false; // CAPS flag

uint8_t scan_to_ascii(uint8_t key)
{
    // Here, we evaluate the different combinations of shift and CAPS flag statues
    // and decide which characters we should return. If its shift and caps, we return the modified special characters i.e., !@$:"|}{
    // but we don't return modified letter i.e., qwertyuiop, however when shift is on, we return modified for everything i.e., ASD{}!)(*)
    // and finally, when CAPS is on, we return only the modified letters i.e., QWERTYUIOP
    // We return the values stored inside a char array and find the key scan -> ASCII map inside an enum table. The table works in ranges
    // see the include/localization/ folder
    if(SFLAG && CAPSF) //If both shift and CAPS are on
    {
        //Get the modified (shift) character
        if(key == DASH)        return _mdash[0];
        else if(key == EQUALS) return _mequals[0];
        else if(key == SCH)    return _mforward[key];
        else if(key >= TC_S && key <= TC_E) return _mtop_chars[key - TC_S];
        else if(key >= MC_S && key <= MC_E) return _mmiddle_chars[key - MC_S];
        else if(key >= BC_S && key <= BC_E) return _mbottom_chars[key - BC_S];
        else if(key >= FIRST_NUMBER && key <= LAST_NUMBER)                 return _mnum[key - FIRST_NUMBER];
        //However we don't want the caps characters
        else if(key >= FIRST_KEY_TOP_ROW && key <= LAST_KEY_TOP_ROW)       return _top_row[key - FIRST_KEY_TOP_ROW];
        else if(key >= FIRST_KEY_MIDDLE_ROW && key <= LAST_KEY_MIDDLE_ROW) return _middle_row[key - FIRST_KEY_MIDDLE_ROW];
        else if(key >= FIRST_KEY_BOTTOM_ROW && key <= LAST_KEY_BOTTOM_ROW) return _bottom_row[key - FIRST_KEY_BOTTOM_ROW];
    }
    else if(SFLAG) //If only shift is on
    {
        //Get the modified (shift) characters
        if(key == DASH)        return _mdash[0];
        else if(key == EQUALS) return _mequals[0];
        else if(key == SCH)    return _mforward[key];
        else if(key >= TC_S && key <= TC_E) return _mtop_chars[key - TC_S];
        else if(key >= MC_S && key <= MC_E) return _mmiddle_chars[key - MC_S];
        else if(key >= BC_S && key <= BC_E) return _mbottom_chars[key - BC_S];
        else if(key >= FIRST_NUMBER && key <= LAST_NUMBER)                 return _mnum[key - FIRST_NUMBER];
        else if(key >= FIRST_KEY_TOP_ROW && key <= LAST_KEY_TOP_ROW)       return _mtop_row[key - FIRST_KEY_TOP_ROW];
        else if(key >= FIRST_KEY_MIDDLE_ROW && key <= LAST_KEY_MIDDLE_ROW) return _mmiddle_row[key - FIRST_KEY_MIDDLE_ROW];
        else if(key >= FIRST_KEY_BOTTOM_ROW && key <= LAST_KEY_BOTTOM_ROW) return _mbottom_row[key - FIRST_KEY_BOTTOM_ROW];
    }
    else if(CAPSF) //If only CAPS is on
    {
        //Get the unmodified (shift) character
        if(key == DASH)        return _dash[0];
        else if(key == EQUALS) return _equals[0];
        else if(key == SCH)    return _forward[key];
        else if(key >= TC_S && key <= TC_E) return _top_chars[key - TC_S];
        else if(key >= MC_S && key <= MC_E) return _middle_chars[key - MC_S];
        else if(key >= BC_S && key <= BC_E) return _bottom_chars[key - BC_S];
        else if(key >= FIRST_NUMBER && key <= LAST_NUMBER)                 return _num[key - FIRST_NUMBER];
        //But we want caps characters
        else if(key >= FIRST_KEY_TOP_ROW && key <= LAST_KEY_TOP_ROW)       return _mtop_row[key - FIRST_KEY_TOP_ROW];
        else if(key >= FIRST_KEY_MIDDLE_ROW && key <= LAST_KEY_MIDDLE_ROW) return _mmiddle_row[key - FIRST_KEY_MIDDLE_ROW];
        else if(key >= FIRST_KEY_BOTTOM_ROW && key <= LAST_KEY_BOTTOM_ROW) return _mbottom_row[key - FIRST_KEY_BOTTOM_ROW];
    }
    else
    {
        //Get the unmodified (shift) character
        if(key == DASH)        return _dash[0];
        else if(key == EQUALS) return _equals[0];
        else if(key == SCH)    return _forward[key];
        else if(key >= TC_S && key <= TC_E) return _top_chars[key - TC_S];
        else if(key >= MC_S && key <= MC_E) return _middle_chars[key - MC_S];
        else if(key >= BC_S && key <= BC_E) return _bottom_chars[key - BC_S];
        else if(key >= FIRST_NUMBER && key <= LAST_NUMBER)                 return _num[key - FIRST_NUMBER];
        else if(key >= FIRST_KEY_TOP_ROW && key <= LAST_KEY_TOP_ROW)       return _top_row[key - FIRST_KEY_TOP_ROW];
        else if(key >= FIRST_KEY_MIDDLE_ROW && key <= LAST_KEY_MIDDLE_ROW) return _middle_row[key - FIRST_KEY_MIDDLE_ROW];
        else if(key >= FIRST_KEY_BOTTOM_ROW && key <= LAST_KEY_BOTTOM_ROW) return _bottom_row[key - FIRST_KEY_BOTTOM_ROW];
    }
    
    //Get the special keys, i.e., enter, backspace, and keys that cannot be *modified*
    if(key == ENTER) return '\n';
    else if(key == SPACE) return ' ';
    else if(key == BCKSPACE) { console_write("\b \b"); return 0; } //In the case of backspace, we want to write something then return nothing
    else if(key == CAPS){ if(CAPSF){ CAPSF = false; } else { CAPSF = true; } return 0; } //Toggle our CAPS flag
    
    return 0;
}

/* Handles the keyboard interrupt */
void keyboard_handler(regs_t *r)
{
    uint8_t scancode;
    // Get key scan code
    scancode = inb(0x60);
    if(scancode == (LSHIFT + 0x80)) //Shift key is released, reset the caps
        SFLAG = false;
    if(scancode == LSHIFT) //See if shift is pressed and held
        SFLAG = true;
    if(scan_to_ascii(scancode) != 0)
        console_putc(scan_to_ascii(scancode));
}

void register_keyboard()
{
    install_handler(33, &keyboard_handler);
}