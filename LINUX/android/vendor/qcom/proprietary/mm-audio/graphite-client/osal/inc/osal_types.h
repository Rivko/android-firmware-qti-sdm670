/*
 * osal_types.h
 *
 * This header file defines different variable types
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __OSAL_TYPES_H__
#define __OSAL_TYPES_H__

#if defined(_ANDROID_)

#include <stdint.h>

#elif defined(_QCET)

#include <stdint.h>

#elif defined(_WIN_OS)

#include <basetsd.h>

typedef UINT64             uint64_t;  /* Unsigned 64 bit value */
typedef UINT32             uint32_t;  /* Unsigned 32 bit value */
typedef UINT16             uint16_t;  /* Unsigned 16 bit value */
typedef UINT8              uint8_t;   /* Unsigned 8  bit value */

typedef INT64              int64_t;   /* Signed 64 bit value */
typedef INT32              int32_t;   /* Signed 32 bit value */
typedef INT16              int16_t;   /* Signed 16 bit value */
typedef INT8               int8_t;    /* Signed 8  bit value */

#endif /* _ANDROID_ */

#ifdef size_t
#undef size_t
typedef uint32_t size_t;
#endif /* size_t */

#ifdef true
#undef true
#endif /* true */

#ifdef false
#undef false
#endif /* false */

#ifdef bool
#undef bool
#endif /* bool */

typedef enum {
    false = 0,
    true,
} bool;

#endif /* __OSAL_TYPES_H__ */
