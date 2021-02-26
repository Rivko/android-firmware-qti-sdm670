#ifndef UIPCINTINTERNAL_H
#define UIPCINTINTERNAL_H
/*==============================================================================

FILE:      uIPCIntIntenral.h

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

  Copyright (c) 2017 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "uIPCInt.h"

typedef struct
{
  uint32 nBaseAddress;
  uint32 nBaseAddressOffset;
  uint16 nBitOffset;
  uint8 nNumBits;
  uint8 nEndpoints;
}
uIPCIntConfigType;

typedef struct
{
  uint32             nEndpoints;
  uIPCIntConfigType *pMap;
}
uIPCIntState;

/*------------------------------------------------------------------------------
Functions required for platform configuration
------------------------------------------------------------------------------*/
void uIPCInt_GetConfigMap( uIPCIntConfigType **ppMap, uint32 *pnNumEndpoints );
void uIPCInt_Init ( void );

#endif /*UIPCINTINTERNAL_H*/

