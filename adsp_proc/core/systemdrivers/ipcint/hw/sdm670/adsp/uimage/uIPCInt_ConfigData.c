/*==============================================================================

  U I M A G E   I P C   I N T E R R U P T S

DESCRIPTION
  This file has the configuration of the uImage IPC Interrupts.

REFERENCES

     Copyright © 2017 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
         Qualcomm Technologies Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/ipcint/hw/sdm670/adsp/uimage/uIPCInt_ConfigData.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/2017 cpaulo  First draft created. 
===========================================================================*/

#include "uIPCIntInternal.h"

#define IPC_BASE            0x62480000
#define IPC_BASE_OFFSET         0x8000
#define IPC_BASE_OFFSET_R1      0x8004

/*
 * Interrupt Config Map
 */

uIPCIntConfigType uIPCIntConfigMap[] =
{
  { IPC_BASE,      IPC_BASE_OFFSET,      8,  4, DALIPCINT_PROC_LPASS }, 
  { IPC_BASE,      IPC_BASE_OFFSET_R1,   24, 4, DALIPCINT_PROC_SPS },
};

void uIPCInt_GetConfigMap( uIPCIntConfigType **ppMap, uint32 *pnEndPoints )
{
  if ( ppMap != NULL )
  {
    *ppMap = uIPCIntConfigMap;
  }
  if ( pnEndPoints != NULL )
  {
    *pnEndPoints = sizeof(uIPCIntConfigMap)/sizeof(uIPCIntConfigMap[0]);
  }
}


