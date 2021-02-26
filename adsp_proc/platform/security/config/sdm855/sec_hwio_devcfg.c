/*=============================================================================

FILE:       sec_hwio.c

SERVICES:   Functions for reading QFPROM fuse-based values

==============================================================================
        Copyright (c) 2016 Qualcomm Technologies Incorporated.
                   All Rights Reserved.
            QUALCOMM Proprietary and Confidential
==============================================================================*/
#ifdef __llvm__
#pragma clang diagnostic ignored "-Wunused-function"
#endif

#include "sec_hwio.h"
#include "HALhwio.h"
#include "msmhwiobase.h"
#include "msmhwioreg.h"

pl_sechwio_type pl_sechwio = {
    HWIO_ADDR(SERIAL_NUM),
    0,
    HWIO_ADDR(OEM_ID),
    0,
    0,
    0
};
