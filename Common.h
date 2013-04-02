/* ============================================================================
 *  Common.h: Common definitions, macros, etc.
 *
 *  BUSSIM: Reality Co-Processor BUS SIMulator.
 *  Copyright (C) 2013, Tyler J. Stachecki.
 *  All rights reserved.
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE', which is part of this source code package.
 * ========================================================================= */
#ifndef __COMMON_H__
#define __COMMON_H__

/* ============================================================================
 *  Attempt to define standard integer and boolean types.
 * ========================================================================= */
#if (__STDC_VERSION__ >= 199901L)
#include <stdbool.h>
#include <stdint.h>

#elif (__cplusplus > 201100L)
#include <cstdint>

#elif defined (_MSC_VER)
#if (_MSC_VER < 1300)
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#else
typedef signed __int8 int8_t;
typedef signed __int16 int16_t;
typedef signed __int32 int32_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
#endif

typedef signed __int64 int64_t;
typedef unsigned __int64 uint64_t;
typedef char bool;
#define false 0
#define true 1

#else
#error "Couldn't determine standard integer types."
#error "Use C99/C11/C++0x/C++11, or add typedefs to 'Common.h'"
#endif

/* ============================================================================
 *  debug(x): Prints messages only when DNDEBUG is not defined.
 * ========================================================================= */
#ifndef NDEBUG
#ifdef __cplusplus
#include <cstdio>
#else
#include <stdio.h>
#endif
#define debug(msg) fprintf(stderr, "[BUS]: "#msg"\n")
#define debugarg(msg, arg) fprintf(stderr, "[BUS]: "#msg"\n", arg)
#else
#define debug(msg)
#define debugarg(msg, arg)
#endif

/* ============================================================================
 *  likely(x) and unlikely(x): Specifies branch weights.
 * ========================================================================= */
#ifdef __GNUC__
#define likely(expr) __builtin_expect(!!(expr), !0)
#define unlikely(expr) __builtin_expect(!!(expr), 0)

#else
#define likely(expr)
#define unlikely(expr)
#endif

/* ============================================================================
 *  unused(x): Marks unused variables.
 * ========================================================================= */
#ifdef __GNUC__
#define uninit(var) var = var
#define unused(var) UNUSED__##var __attribute__((unused))
#else
#define uninit(var) var
#define unused(var)
#endif

#endif

