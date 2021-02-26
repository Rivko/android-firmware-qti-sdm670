#ifndef CUSTSDCC_H
#define CUSTSDCC_H
/*===========================================================================

            " C u s t - S D C C "   H E A D E R   F I L E

DESCRIPTION
  Configuration for SDCC Feature

  Copyright (c) 2007,2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/build.tz/2.0.2/cust/custsdcc.h#1 $ $DateTime: 2018/02/06 03:02:41 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ----------------------------------------------------------
2007-11-29   sc      Updated with sdcc code review comments
2007-11-26   sc      Initial creation
===========================================================================*/

/* for M7600/M7200A KxMemory.h location */
#define KXMEMORY_H_FILE   "quartz/KxMemory.h"

/* configure the number of SD/SDIO/MMC card slots */
#if defined(FEATURE_THIN_UI)
    #define SDCC_MAX_NUM_CONTROLLER 1
#else
    #if defined(T_SURF)
        #define SDCC_MAX_NUM_CONTROLLER 4
    #else /* FFA */
        #if defined(T_FFA)
           #define SDCC_MAX_NUM_CONTROLLER 2
           #define SDCC_WLAN_SLOT 1
        #endif
    #endif
#endif

/* configure the number of MUX support */
#define SDCC_MAX_MUX_SUPPORT    1

/* for M7600 or targets that supports 8-bit bus width mode */
#define FEATURE_SDCC_MMC_8BIT_BUS_WIDTH_SUPPORT

/* for targets that have data mover restrictions such as */
/* 32-byte alignment and 1-to-1 mapping */
/* translate virtual address to physical address is required and */
/* temporary SDCC data buffer is required if the memory address */
/* is not data mover compliance */
#if !defined(FEATURE_WINCE) && !defined(BUILD_BOOT_CHAIN)
#define FEATURE_SDCC_HANDLE_DMOV_NONCOMPLIANT_MEMORY
#endif

#define SDCC_WLAN_SLOT 1

#endif /* CUSTSDCC_H */

