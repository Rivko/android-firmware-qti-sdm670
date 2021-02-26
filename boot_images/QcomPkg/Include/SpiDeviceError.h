#ifndef _SPIDEVICEERROR_H_
#define _SPIDEVICEERROR_H_
/*==================================================================================================

FILE: SpiDeviceError.h

DESCRIPTION:
   This file contains the result definitions.

====================================================================================================

   Edit History

When      Who  What, where, why
--------  ---  -----------------------------------------------------------
06/11/12  DK   Added review comments.
09/26/11  AG   Created

====================================================================================================
                     Copyright (c) 2011-2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     Qualcomm Proprietary/GTDR
==================================================================================================*/

typedef enum
{
   SPIDEVICE_RES_SUCCESS            = 0,

   /* Error Classes */
   SPIDEVICE_RES_ERROR_CLS_DEVICE   = 0x03000000,
   SPIDEVICE_RES_ERROR_CLS_PLATFORM = 0x0E000000,
   SPIDEVICE_RES_ERROR_CLS_OS       = 0x0F000000,
} SPIDEVICE_RESULT;

#endif  // _SPIDEVICEERROR_H_
