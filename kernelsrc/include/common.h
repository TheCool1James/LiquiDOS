/*
 * File:   common.h
 * Author: Kevin Dai
 * Email:  kevindai02@outlook.com
 *
 * Created on 29-Jul-2017 05:08:00 PM
 *
 * @ Last modified by:   Kevin Dai
 * @ Last modified time: 2018-04-01T20:31:00-04:00
*/

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <stdarg.h>

#include "config.h"
#include "export.h"
#include "compiler_flags.h"
#include "utils.h"
#include "errno.h"

#define STREAM_OUT 0
#define STREAM_ERR 1
#define STREAM_LOG 2

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h> // C doesn't have booleans by default.

#ifdef __cplusplus
}
#endif
