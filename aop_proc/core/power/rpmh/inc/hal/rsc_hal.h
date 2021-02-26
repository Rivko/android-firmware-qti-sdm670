/*
==============================================================================

FILE:         rsc_hal.h

DESCRIPTION:  This file contains description about RSC Hal. Rsc HAL is mainly responsible for 
              managing TCSs/AMCs associcated with RSC hw block on each subsystem


==============================================================================

                             Edit History



Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================


#ifndef _HAL_RSC
#define _HAL_RSC

#include <stddef.h>
#include "comdef.h"
#include "rsc_hal_hwio.h"
#include "rpmh_common.h"
#include "rsc_hal_image_config.h"

#define DRV 0
#define MSG_SIZE 8
#define AMC_WRITE 1

#define _HWIO_CONCAT_NAME(prefix, name)         prefix##_##name
#define HWIO_CONCAT_NAME(prefix, name)          _HWIO_CONCAT_NAME(prefix, name)
#define TCS_HWIO_EXPAND_NAME(name)                  HWIO_CONCAT_NAME(TCS_HWIO_PREFIX, name)

/* EXPLICIT HWIO BASE MACROS */
#define TCS_HWIO_INX(base, name)                          HWIO_INX(base,    name)
#define TCS_HWIO_INXI(base, name, index)                  HWIO_INXI(base,   name, index)
#define TCS_HWIO_INXI2(base, name, index1, index2)        HWIO_INXI2(base,  name, index1, index2)

#define TCS_HWIO_INXM(base, name, mask)                   HWIO_INXM(base,   name, mask)
#define TCS_HWIO_INXMI(base, name, index, mask)           HWIO_INXMI(base,  name, index, mask)

#define TCS_HWIO_INXF(base, name, field)                  HWIO_INXF(base,   name, field)
#define TCS_HWIO_INXFI(base, name, index, field)          HWIO_INXFI(base,  name, index, field)
#define TCS_HWIO_INXFI2(base, name, index, index2, field) HWIO_INXFI2(base, name, index1, index2, field)

#define TCS_HWIO_OUTX(base, name, val)                    HWIO_OUTX(base,   name, val)

#define TCS_HWIO_OUTXI(base, name, index, val)            HWIO_OUTXI(base,  name, index, val)
#define TCS_HWIO_OUTXI2(base, name, index1, index2, val)  HWIO_OUTXI2(base, name, index1, index2, val)

#define TCS_HWIO_OUTXF(base, name, field, val)            HWIO_OUTXF(base,  name, field, val)
#define TCS_HWIO_OUTXFI(base, name, index, field, val)    HWIO_OUTXFI(base, name, index, field, val)

#define TCS_HWIO_OUTXM(base, name, mask,  val)            HWIO_OUTXM(base,  name, mask, val)
#define TCS_HWIO_OUTXMI(base, name, index, mask, val)     HWIO_OUTXMI(base, name, index, mask, val)


/* EXPLICIT HWIO BASE MACROS WITH PREFIX */
//#define TCS_HWIO_INX(base, name)                          HWIO_INX(base,    TCS_HWIO_EXPAND_NAME(name))
//#define TCS_HWIO_INXI(base, name, index)                  HWIO_INXI(base,   TCS_HWIO_EXPAND_NAME(name), index)
//#define TCS_HWIO_INXI2(base, name, index1, index2)        HWIO_INXI2(base,  TCS_HWIO_EXPAND_NAME(name), index1, index2)
//
//#define TCS_HWIO_INXM(base, name, mask)                   HWIO_INXM(base,   TCS_HWIO_EXPAND_NAME(name), mask)
//#define TCS_HWIO_INXMI(base, name, index, mask)           HWIO_INXMI(base,  TCS_HWIO_EXPAND_NAME(name), index, mask)
//
//#define TCS_HWIO_INXF(base, name, field)                  HWIO_INXF(base,   TCS_HWIO_EXPAND_NAME(name), field)
//#define TCS_HWIO_INXFI(base, name, index, field)          HWIO_INXFI(base,  TCS_HWIO_EXPAND_NAME(name), index, field)
//#define TCS_HWIO_INXFI2(base, name, index, index2, field) HWIO_INXFI2(base, TCS_HWIO_EXPAND_NAME(name), index1, index2, field)
//
//#define TCS_HWIO_OUTX(base, name, val)                    HWIO_OUTX(base,   TCS_HWIO_EXPAND_NAME(name), val)
//
//#define TCS_HWIO_OUTXI(base, name, index, val)            HWIO_OUTXI(base,  TCS_HWIO_EXPAND_NAME(name), index, val)
//#define TCS_HWIO_OUTXI2(base, name, index1, index2, val)  HWIO_OUTXI2(base, TCS_HWIO_EXPAND_NAME(name), index1, index2, val)
//
//#define TCS_HWIO_OUTXF(base, name, field, val)            HWIO_OUTXF(base,  TCS_HWIO_EXPAND_NAME(name), field, val)
//#define TCS_HWIO_OUTXFI(base, name, index, field, val)    HWIO_OUTXFI(base, TCS_HWIO_EXPAND_NAME(name), index, field, val)
//
//#define TCS_HWIO_OUTXM(base, name, mask,  val)            HWIO_OUTXM(base,  TCS_HWIO_EXPAND_NAME(name), mask, val)
//#define TCS_HWIO_OUTXMI(base, name, index, mask, val)     HWIO_OUTXMI(base, TCS_HWIO_EXPAND_NAME(name), index, mask, val)

typedef enum {
    WriteFirenForget=0,
    WriteCompletion,
    ReadRequest
} RSCRequestType;

#endif
