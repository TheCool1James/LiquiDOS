/**
 * @file   vsprintf.c
 * @author Kevin Dai \<kevindai02@outlook.com\>
 * 
 * @date Created on Saturday, October 13th 2018, 9:12:17 pm
 * 
 * @date Last modified by:   Kevin Dai
 * @date Last modified time: 2018-10-14T16:17:34-04:00
 */

#include <stdio.h>
#include "string.h"

EXPORT_SYMBOL(sprintf);
EXPORT_SYMBOL(vsprintf);

#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

#define SIGNED   0b1       // Is signed
#define ZEROPAD  0b10      // Pad with zeros instead of spaces
#define FPLUS    0b100     // Force plus sign
#define SPLUS    0b1000    // Show space instead of '+'
#define PREFIX   0b10000   // Prefixes 0x and 0
#define SMALL    0b100000  // Upper case for hex printout
#define LEFT     0b1000000 // Left justify within field width

#define is_digit(c) ((c) >= '0' && (c) <= '9')

// Reads a number from the format string
static int parse_num(const char** c)
{
    int num = 0;
    while (is_digit(**c))
        num = num * 10 + *((*c)++) - '0';
    return num;
}

static int f(void (*g)(char, char**), char c, char** buf) { g(c, buf); return 1; }

static int num(void (*g)(char, char**), char** _buf, long long num, char base, uint8_t flags, int width, int percision)
{
    char str[23]; memset(str, 0, 23); // 22 digits of uint64_t in oct + 1 EOL
    char* base_str = flags & SMALL ? "0123456789abcdef" : "0123456789ABCDEF";
    int written = 0;
    char sign = 0;
    int w = 0, p = percision == -1 ? 0 : percision;
    unsigned long long divide = (num < 0) && (flags & SIGNED) ? -num : (unsigned long long) num;
    // Base conversion
    for(int i = 0; divide > 0; i++)
    {
        str[i] = base_str[divide % base];
        divide /= base;
    }
    if(num == 0) str[0] = '0';
    // Prepare some values for padding
    if((flags & PREFIX) && (base == 16)) w += 2;
    else if((flags & PREFIX) && (base == 8)) w++;
    if(((flags & SIGNED) && (num < 0)) || ((flags & SIGNED) && (flags & FPLUS)) || ((flags & SIGNED) && (flags & SPLUS))) w++;
    w = strlen(str) + w;
    w = width - w > 0 ? width - w : 0;
    // Deal with signage
    sign = (num < 0) && (flags & SIGNED) ? '-' : sign;
    sign = (num >= 0) && (flags & FPLUS) ? '+' : sign;
    sign = (num >= 0) && (flags & SPLUS) ? ' ' : sign;
    // Deal with paddings
    if(!(flags & ZEROPAD) && !(flags & LEFT))
        while(w-- > 0)
            written += f(g, ' ', _buf);
    written = sign != 0 ? written + f(g, sign, _buf) : written; // Sign and prefixes go before the zero padding and after the space padding
    if((flags & PREFIX) && (base == 16)) { written += f(g, '0', _buf); written += flags & SMALL ? f(g, 'x', _buf) : f(g, 'X', _buf); }
    else if((flags & PREFIX) && (base == 8)) written += f(g, '0', _buf);
    while(p-- > 0) written += f(g, '0', _buf);
    if(flags & ZEROPAD)
        while(w-- > 0)
            written += f(g, '0', _buf);
    for(int i = strlen(str); i > 0; i--) written += f(g, str[i - 1], _buf);
    if(!(flags & ZEROPAD) && (flags & LEFT))
        while(w-- > 0)
            written += f(g, ' ', _buf);
    return written;
}

int ssprintf(void (*g)(char, char**), char* buf, const char* fmt, va_list args)
{
    int written = 0;
    uint8_t flags;
    int width;
    int percision;
    char length;

    while(*fmt != '\0')
    {
        long long val = 0;
        char base = 0;

        if(*fmt == '%')
        {
            // Collect and parse the flags
            flags = 0;
repeat:
            fmt++;
            switch(*fmt)
            {
                case '0': flags |= ZEROPAD; goto repeat;
                case ' ': flags |= SPLUS;   goto repeat;
                case '+': flags |= FPLUS;   goto repeat;
                case '#': flags |= PREFIX;  goto repeat;
                case '-': flags |= LEFT;    goto repeat;
            }

            // Parse the width
            width = -1;
            if(*fmt == '*')
            {
                fmt++;
                width = va_arg(args, int);
                if(width < 0)
                {
                    width = -width;
                    flags |= LEFT;
                }
            }
            else if(is_digit(*fmt))
                width = parse_num(&fmt);

            // Parse percision
            percision = -1;
            if(*fmt == '.')
            {
                fmt++;
                if(*fmt == '*')
                {
                    percision = va_arg(args, int);
                    fmt++;
                }
                else if(is_digit(*fmt))
                    percision = parse_num(&fmt);
                percision = percision < 0 ? 0 : percision;
            }

            // Parse length
            length = 0;
            if((*fmt == 'h') || (*fmt == 'l') || (*fmt == 'L'))
            {
                length = *fmt;
                fmt++;
            }

            switch(*fmt)
            {
                case 's': ; // Don't mind me, I'm just a semicolon because of some C standard shit
                    char* str = va_arg(args, char*);
                    int w = (width - strlen(str) > 0) ? width - strlen(str) : 0;
                    if(!(flags & LEFT)) // Deal with padding before the text
                        while(w-- > 0)
                            written += f(g, ' ', &buf);
                    for(int i = 0; str[i] != '\0'; i++) written += f(g, str[i], &buf); // Print the text
                    while(w-- > 0) // Deal with padding after the text
                        written += f(g, ' ', &buf);
                    break;
                case 'c':
                    if(!(flags & LEFT)) // Deal with padding before the character (do you see a trend?)
                        while(--width > 0)
                            written += f(g, ' ', &buf);
                    written += f(g, (uint8_t) va_arg(args, int), &buf); // Print
                    while(--width > 0) // I'm just a comment don't mind me
                        written += f(g, ' ', &buf);
                    break;
                case 'x': flags |= SMALL;
                case 'X':
                    base = 16;
                case 'o':
                    base = base == 0 ? 8 : base;
                case 'u':
                    base = base == 0 ? 10 : base;
                    if(length == 'l')       val = va_arg(args, uint64_t);
                    else if(length == 'h')  val = (unsigned short) va_arg(args, uint32_t);
                    else val = va_arg(args, uint32_t);
                    written += num(g, &buf, val, base, flags, width, percision);
                    break;
                case 'p':
                    if (width == -1)
                    {
                        width = 8;
                        flags |= ZEROPAD;
                    }
                    written += num(g, &buf, (unsigned long long) va_arg(args, void *), 16, flags, width, percision);
                    break;
                case 'i':
                case 'd': flags |= SIGNED;
                    if(length == 'l')       val = va_arg(args, long long);
                    else if(length == 'h')  val = (short) va_arg(args, long);
                    else val = va_arg(args, long);
                    written += num(g, &buf, val, 10, flags, width, percision);
                    break;
                case 'n': ;
                    int* foo = va_arg(args, int*); // Get the pointer
                    *foo = written; // Store the amount of characters written in an integer pointed to by a pointer
                    break;
                case '%': written += f(g, '%', &buf); break;
                default: return -1;
            }
            fmt++;
        }
        else
        {
            written += f(g, *fmt, &buf);
            fmt++;
        }
    }
    return written;
}

static void __swrite_sbuf__(char c, char** buf)
{
    **buf = c;
    *buf += 1;
}

int sprintf(char* buf, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = vsprintf(buf, fmt, args);
    va_end(args);
    return ret;
}

int vsprintf(char* buf, const char* fmt, va_list args)
{
    char* modbuf = buf;
    int ret = ssprintf(__swrite_sbuf__, modbuf, fmt, args);
    return ret;
}
