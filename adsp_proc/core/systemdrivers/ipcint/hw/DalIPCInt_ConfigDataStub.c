/*==============================================================================

  I P C   I N T E R R U P T S

DESCRIPTION
  This file has the configuration of the IPC Interrupts.

REFERENCES

     Copyright © 2015 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/ipcint/hw/DalIPCInt_ConfigDataStub.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/2015 cpaulo  First draft created. 
===========================================================================*/

#include "DalIPCInt.h"
#include "DDIHWIO.h"

#define IPC_BASE            0x0
#define IPC_BASE_OFFSET     0x0
#define IPC_BASE_OFFSET_R1  0x0

/*
 * Modem Interrupt Config Map
 */

IPCIntConfigType IPCIntConfigMap[DALIPCINT_NUM_PROCS] =
{
  { IPC_NULL_BASE, IPC_NULL_BASE_OFFSET, 0, 0 }, //DALIPCINT_PROC_NULL
  { IPC_NULL_BASE, IPC_NULL_BASE_OFFSET, 0, 0 }, //DALIPCINT_PROC_MCPU
  { IPC_NULL_BASE, IPC_NULL_BASE_OFFSET, 0, 0 }, //DALIPCINT_PROC_MDSPSW
  { IPC_NULL_BASE, IPC_NULL_BASE_OFFSET, 0, 0 }, //DALIPCINT_PROC_MDSPFW
  { IPC_NULL_BASE, IPC_NULL_BASE_OFFSET, 0, 0 }, //DALIPCINT_PROC_ACPU
  { IPC_NULL_BASE, IPC_NULL_BASE_OFFSET, 0, 0 }, //DALIPCINT_PROC_ACPU1
  { IPC_NULL_BASE, IPC_NULL_BASE_OFFSET, 0, 0 }, //DALIPCINT_PROC_ADSP
  { IPC_NULL_BASE, IPC_NULL_BASE_OFFSET, 0, 0 }, //DALIPCINT_PROC_SPS
  { IPC_NULL_BASE, IPC_NULL_BASE_OFFSET, 0, 0 }, //DALIPCINT_PROC_RPM
  { IPC_NULL_BASE, IPC_NULL_BASE_OFFSET, 0, 0 }, //DALIPCINT_PROC_WCN/DALIPCINT_PROC_RIVA
  { IPC_NULL_BASE, IPC_NULL_BASE_OFFSET, 0, 0 }, //DALIPCINT_PROC_GSS
  { IPC_NULL_BASE, IPC_NULL_BASE_OFFSET, 0, 0 }, //DALIPCINT_PROC_LPASS
  { IPC_NULL_BASE, IPC_NULL_BASE_OFFSET, 0, 0 }, //DALIPCINT_PROC_TZ
  { IPC_NULL_BASE, IPC_NULL_BASE_OFFSET, 0, 0 }, //DALIPCINT_PROC_SPSS
  { IPC_NULL_BASE, IPC_NULL_BASE_OFFSET, 0, 0 }, //DALIPCINT_PROC_CDSP
  { IPC_NULL_BASE, IPC_NULL_BASE_OFFSET, 0, 0 }, //DALIPCINT_PROC_GFX
  { IPC_NULL_BASE, IPC_NULL_BASE_OFFSET, 0, 0 }, //DALIPCINT_PROC_TITAN
  { IPC_NULL_BASE, IPC_NULL_BASE_OFFSET, 0, 0 }  //DALIPCINT_PROC_SDC
};

void IPCInt_GetConfigMap( IPCIntConfigType **ppMap )
{
  if ( ppMap != NULL )
  {
    *ppMap = IPCIntConfigMap;
  }
}

void IPCInt_InitConfig( void )
{
  /*
   *  No longer need to do anything here
   */
  return;
}







  
