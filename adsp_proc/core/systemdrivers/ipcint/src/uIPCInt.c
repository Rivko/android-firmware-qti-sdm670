/*============================================================================

FILE:      uIPCInt.c

DESCRIPTION:
  This file implements the portion of the uImage IPC Interrupt Driver
  that triggers interrupts from uImage.

PUBLIC CLASSES:
  uIPCInt_Trigger
  uIPCInt_IsSupported

============================================================================
     Copyright (c) 2017 QUALCOMM Technologies, Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================
  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/ipcint/src/uIPCInt.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $
============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "uIPCInt.h"
#include "uIPCIntInternal.h"
#include "DDIHWIO.h"
#include "DALDeviceId.h"
#include "DalDevice.h"

/*=========================================================================
      Internal Data
==========================================================================*/

static uIPCIntState uIPCIntCtxt;

/*=========================================================================
      Implementation
==========================================================================*/

/*==========================================================================

  FUNCTION      uIPCInt_Init

  DESCRIPTION   See uIPCIntIntenral.h

==========================================================================*/
void uIPCInt_Init ( void )
{
  DalDeviceHandle *pHWIOHandler;
  uint32 nTableEntry;

  if ( uIPCIntCtxt.pMap == NULL )
  {
    uIPCInt_GetConfigMap( &uIPCIntCtxt.pMap, &uIPCIntCtxt.nEndpoints );

    if(DAL_DeviceAttach(DALDEVICEID_HWIO,&pHWIOHandler) == DAL_SUCCESS)
    {
      for ( nTableEntry = 0; nTableEntry < uIPCIntCtxt.nEndpoints; nTableEntry++ ) 
      {
        if ( uIPCIntCtxt.pMap[nTableEntry].nBaseAddress != UIPC_NULL_BASE ) 
        {
          (void)DalHWIO_MapRegionByAddress(pHWIOHandler, (uint8 *)uIPCIntCtxt.pMap[nTableEntry].nBaseAddress, 
                                        (uint8**)&(uIPCIntCtxt.pMap[nTableEntry].nBaseAddress) );
        }
      }
    }

    for ( nTableEntry = 0; nTableEntry < uIPCIntCtxt.nEndpoints; nTableEntry++ ) 
    {
      if ( uIPCIntCtxt.pMap[nTableEntry].nBaseAddress != UIPC_NULL_BASE ) 
      {
        uIPCIntCtxt.pMap[nTableEntry].nBaseAddress += uIPCIntCtxt.pMap[nTableEntry].nBaseAddressOffset;
      }
    }
  }
}

/*==========================================================================

  FUNCTION      uIPCInt_Trigger

  DESCRIPTION   See uIPCInt.h

==========================================================================*/

int32 uIPCInt_Trigger
(
  DalIPCIntProcessorType  eTarget,
  DalIPCIntInterruptType  eInterrupt
) 
{
  uint32 int_mask = 1;
  uint32 reg_addr = 0;
  uint32 nTableEntry;

  for ( nTableEntry = 0; nTableEntry < uIPCIntCtxt.nEndpoints; nTableEntry++ ) 
  {
    if ( uIPCIntCtxt.pMap[nTableEntry].nEndpoints == eTarget && 
         uIPCIntCtxt.pMap[nTableEntry].nNumBits > eInterrupt )
    {
      /*
       * Get the IPC register address for the target
       */
      reg_addr = (uint32) uIPCIntCtxt.pMap[nTableEntry].nBaseAddress;

      /*
       * Offset the bitmask by the base offset for the target subsystem
       */
      int_mask <<= uIPCIntCtxt.pMap[nTableEntry].nBitOffset;

      /* 
       * Offset the bitmask additionally by the bitmask requested 
       */ 
      int_mask <<= (uint32) eInterrupt;

      /*
       * Write the bitmask to the IPC out register
       */
      outpdw(reg_addr, int_mask);     

      return UIPCINT_SUCCESS;
    }
  }
  return UIPCINT_ERROR;
} /* uIPCInt_Trigger */


/*==========================================================================

  FUNCTION      IPCInt_IsSupported

  DESCRIPTION   See DDIIPCInt.h

==========================================================================*/

int32 uIPCInt_IsSupported
(
  DalIPCIntProcessorType  eTarget,
  DalIPCIntInterruptType  eInterrupt
) 
{
  uint32 nTableEntry;

  for ( nTableEntry = 0; nTableEntry < uIPCIntCtxt.nEndpoints; nTableEntry++ ) 
  {
    if ( uIPCIntCtxt.pMap[nTableEntry].nEndpoints == eTarget && 
         uIPCIntCtxt.pMap[nTableEntry].nNumBits > eInterrupt )
    {
      return UIPCINT_SUCCESS;
    }
  }
  return UIPCINT_ERROR;
} /* uIPCInt_IsSupported */



