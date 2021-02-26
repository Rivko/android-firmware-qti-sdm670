/*============================================================================

FILE:      DalIPCInt.c

DESCRIPTION:
  This file implements the portion of the ChipInfo DAL that reads information
  from the hardware.

PUBLIC CLASSES:
  IPCInt_DriverInit
  IPCInt_DriverDeInit
  IPCInt_Trigger
  IPCInt_IsSupported
  IPCInt_TriggerBySource
  IPCInt_IsSupportedBySource

============================================================================
        Copyright (c) 2009-2016 QUALCOMM Technologies, Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================
  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/ipcint/src/DalIPCInt.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $
============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DalIPCInt.h"
#include "DALStdDef.h"
#include "DDIHWIO.h"
#include "DDIInterruptController.h"

#ifdef IPCINT_RECEIVE_TEST
uint8 InterruptsToRegister[32];
uint32 Interruptlog[200];
 DalDeviceHandle  *pIPCHandle;
 
#endif /* IPCINT_RECEIVE_TEST */
/*=========================================================================
      Implementation
==========================================================================*/

DalDeviceHandle *pHWIOHandler;
/*==========================================================================

  FUNCTION      IPCInt_DriverInit

  DESCRIPTION   See DDIIPCInt.h

==========================================================================*/

DALResult IPCInt_DriverInit
(
  IPCIntDrvCtxt *pCtxt
)
{
  return DAL_SUCCESS;

} /* IPCInt_DriverInit */


/*==========================================================================

  FUNCTION      IPCInt_DriverDeInit

  DESCRIPTION   See DDIIPCInt.h

==========================================================================*/

DALResult IPCInt_DriverDeInit
(
  IPCIntDrvCtxt *pCtxt
)
{
	return DAL_SUCCESS;

} /* IPCInt_DriverDeInit */


/*==========================================================================

  FUNCTION      IPCInt_Trigger

  DESCRIPTION   See DDIIPCInt.h

==========================================================================*/

DALResult IPCInt_Trigger
(
  IPCIntClientCtxt       *pCtxt,
  DalIPCIntProcessorType  eTarget,
  DalIPCIntInterruptType  eInterrupt
) 
{
  return IPCInt_TriggerBySource(
    pCtxt, pCtxt->pIPCIntDevCtxt->pIPCIntDrvCtxt->eSourceProcessor,
    eTarget, eInterrupt);

} /* IPCInt_Trigger */


/*==========================================================================

  FUNCTION      IPCInt_TriggerBySource

  DESCRIPTION   See DDIIPCInt.h

==========================================================================*/

DALResult IPCInt_TriggerBySource
(
  IPCIntClientCtxt       *pCtxt,
  DalIPCIntProcessorType  eSource,
  DalIPCIntProcessorType  eTarget,
  DalIPCIntInterruptType  eInterrupt
) 
{
  uint32 int_mask = 1;
  uint32 reg_addr = 0;

  /*
   * Check if the target is acceptable, that we have a valid address and 
   * that the interrupt bitmask is supported in our implementation. 
   */
  if ( eSource  != pCtxt->pIPCIntDevCtxt->pIPCIntDrvCtxt->eSourceProcessor ||
       eTarget >= DALIPCINT_NUM_PROCS ||
       pCtxt->pIPCIntDevCtxt->ipcint_cntrl.nMap[eTarget].nBaseAddress == IPC_NULL_BASE ||
       pCtxt->pIPCIntDevCtxt->ipcint_cntrl.nMap[eTarget].nNumBits <= (uint16) eInterrupt ) 
  {
    return DAL_ERROR;
  }

  /*
   * Get the IPC register address for the target
   */
  reg_addr = (uint32) pCtxt->pIPCIntDevCtxt->ipcint_cntrl.nMap[eTarget].nBaseAddress;

  /*
   * Offset the bitmask by the base offset for the target subsystem
   */
  int_mask <<= pCtxt->pIPCIntDevCtxt->ipcint_cntrl.nMap[eTarget].nBitOffset;

  /* 
   * Offset the bitmask additionally by the bitmask requested 
   */ 
  int_mask <<= (uint32) eInterrupt;

  /*
   * Write the bitmask to the IPC out register
   */
  outpdw(reg_addr, int_mask);     
      
  return DAL_SUCCESS;

} /* IPCInt_TriggerBySource */


/*==========================================================================

  FUNCTION      IPCInt_IsSupported

  DESCRIPTION   See DDIIPCInt.h

==========================================================================*/

DALResult IPCInt_IsSupported
(
  IPCIntClientCtxt       *pCtxt,
  DalIPCIntProcessorType  eTarget,
  DalIPCIntInterruptType  eInterrupt
) 
{
  return IPCInt_IsSupportedBySource(
    pCtxt, pCtxt->pIPCIntDevCtxt->pIPCIntDrvCtxt->eSourceProcessor,
    eTarget, eInterrupt);

} /* IPCInt_IsSupported */


/*==========================================================================

  FUNCTION      IPCInt_IsSupportedBySource

  DESCRIPTION   See DDIIPCInt.h

==========================================================================*/

DALResult IPCInt_IsSupportedBySource
(
  IPCIntClientCtxt       *pCtxt,
  DalIPCIntProcessorType  eSource,
  DalIPCIntProcessorType  eTarget,
  DalIPCIntInterruptType  eInterrupt
) 
{
  /*
   * Check if the target is acceptable, that we have a valid address and 
   * that the interrupt bitmask is supported in our implementation. 
   */
  if ( eSource  != pCtxt->pIPCIntDevCtxt->pIPCIntDrvCtxt->eSourceProcessor ||
       eTarget >= DALIPCINT_NUM_PROCS ||
       pCtxt->pIPCIntDevCtxt->ipcint_cntrl.nMap[eTarget].nBaseAddress == IPC_NULL_BASE ||
       pCtxt->pIPCIntDevCtxt->ipcint_cntrl.nMap[eTarget].nNumBits <= (uint16) eInterrupt ) 
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;

} /* IPCInt_IsSupported */

DALResult IPCInt_GetTargetData(IPCIntClientCtxt *pCtxt)
{
  IPCIntDevCtxt *pDevice;
  DALSYS_PROPERTY_HANDLE_DECLARE(IPCInt_property);
  DALSYSPropertyVar system_property;
  uint32 nTableEntry;

  pDevice = pCtxt->pIPCIntDevCtxt; // point to the associated device context.  
  
  if( DAL_SUCCESS != DALSYS_GetDALPropertyHandle(DALDEVICEID_IPCINT, IPCInt_property))
  {
    return DAL_ERROR;
  }

  if( DAL_SUCCESS != DALSYS_GetPropertyValue(IPCInt_property, "SourceProc", 0, &system_property))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
                      "IPCInt_GetTargetData: Device could not get SourceProc");
    return DAL_ERROR;
    
  }
    
  pCtxt->pIPCIntDevCtxt->pIPCIntDrvCtxt->eSourceProcessor = (DalIPCIntProcessorType)system_property.Val.dwVal;

  // Get the configuration table
  IPCInt_GetConfigMap( &pDevice->ipcint_cntrl.nMap );

  if(DAL_DeviceAttach(DALDEVICEID_HWIO,&pHWIOHandler) == DAL_SUCCESS)
  {
    for ( nTableEntry = 0; nTableEntry < DALIPCINT_NUM_PROCS; nTableEntry++ ) 
    {
      if ( pDevice->ipcint_cntrl.nMap[nTableEntry].nBaseAddress != IPC_NULL_BASE ) 
      {
        if (DalHWIO_MapRegionByAddress(pHWIOHandler, (uint8 *)pDevice->ipcint_cntrl.nMap[nTableEntry].nBaseAddress, 
                                      (uint8**)&(pDevice->ipcint_cntrl.nMap[nTableEntry].nBaseAddress) )!= DAL_SUCCESS)
        {
          DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
                          "IPCInt_GetTargetData: could not get the virtual address");
          return DAL_ERROR; 
        }    
        pDevice->ipcint_cntrl.nMap[nTableEntry].nBaseAddress += pDevice->ipcint_cntrl.nMap[nTableEntry].nBaseAddressOffset;
      }
    }
  }
  else
  {
    for ( nTableEntry = 0; nTableEntry < DALIPCINT_NUM_PROCS; nTableEntry++ ) 
    {
      if ( pDevice->ipcint_cntrl.nMap[nTableEntry].nBaseAddress != IPC_NULL_BASE ) 
      {
        pDevice->ipcint_cntrl.nMap[nTableEntry].nBaseAddress += pDevice->ipcint_cntrl.nMap[nTableEntry].nBaseAddressOffset;
      }
    }
  }

  IPCInt_InitConfig();

  return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are declared in DalDevice Interface. 
------------------------------------------------------------------------------*/

DALResult 
IPCInt_DeviceInit(IPCIntClientCtxt *pCtxt)
{

  IPCIntDevCtxt *pDevice;
  pDevice = pCtxt->pIPCIntDevCtxt; // point to the associated device context

  if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                                      &(pDevice->IPCInt_synchronization),
                                      &(pDevice->IPCIntSyncObject)))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
                      "IPCInt_DeviceInit: Device could not create a sync object");
    return DAL_ERROR;
  }

  if(IPCInt_GetTargetData(pCtxt) != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Success.                                                              */
  /*-----------------------------------------------------------------------*/
  return DAL_SUCCESS;
}

DALResult 
IPCInt_DeviceDeInit(IPCIntClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult 
IPCInt_PowerEvent(IPCIntClientCtxt *pCtxt, DalPowerCmd PowerCmd, 
                    DalPowerDomain PowerDomain)
{
    return DAL_SUCCESS;
}

DALResult 
IPCInt_Open(IPCIntClientCtxt *pCtxt, uint32 dwaccessMode )
{
    return DAL_SUCCESS;
}

DALResult 
IPCInt_Close(IPCIntClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult 
IPCInt_Info(IPCIntClientCtxt *pCtxt,DalDeviceInfo *pdeviceInfo, uint32 dwSize)
{
    return DAL_SUCCESS;
}

#ifdef IPCINT_RECEIVE_TEST
void IPCInt_TestIsr
(
  void *param
)
{
 
  uint8 Interrupt;
  uint32 interrupt_count, mycount;

  Interrupt = *(uint32 *)param;
  interrupt_count = Interruptlog[Interrupt];
  Interruptlog[Interrupt] = interrupt_count +1;
  mycount = 1;
  while(mycount)
  {
    if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_MDSPSW, DALIPCINT_GP_0))
    {
      return;
    }
    if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_MDSPSW, DALIPCINT_GP_1))
    {
      return;    
    }
    if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_MDSPSW, DALIPCINT_GP_2))
    {
      return;    
    }
    if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_MDSPSW, DALIPCINT_GP_3))
    {
      return;    
    }

    if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_ACPU, DALIPCINT_GP_0))
    {
      return;    
    }
    if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_ACPU, DALIPCINT_GP_1))
    {
      return;    
    }
    if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_ACPU, DALIPCINT_GP_2))
    {
      return;    
    }
    if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_ACPU, DALIPCINT_GP_3))
    {
      return;    
    }
    if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_SPSS, DALIPCINT_GP_0))
    {
      return;    
    }
    if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_SPSS, DALIPCINT_GP_1))
    {
      return;    
    }
    if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_SPSS, DALIPCINT_GP_2))
    {
      return;    
    }
    if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_SPSS, DALIPCINT_GP_3))
    {
      return;    
    }
    mycount --;
  }
}
#endif /* IPCINT_RECEIVE_TEST*/
void IPCInt_Test(void)
{

  #ifdef IPCINT_RECEIVE_TEST
  DalDeviceHandle * intr_handle;
  uint32 mycount,index;
  mycount= 10;
  if(DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_IPCINT, &pIPCHandle))
  {
    return;
  }
  /* attach to the Main Interrupt Controller dal*/
  if(DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, 
                                     &intr_handle))
  {
    return;
  }

  /* Incoming interrupts from MPSS */
  InterruptsToRegister[0] = 92;
  InterruptsToRegister[1] = 93;
  InterruptsToRegister[2] = 94;
  InterruptsToRegister[3] = 95;

  /* Incoming interrupts from APPS */
  InterruptsToRegister[4] = 13;
  InterruptsToRegister[5] = 14;
  InterruptsToRegister[6] = 15;
  InterruptsToRegister[7] = 16;
  
  /* Incoming interrupts from Pronto */
  InterruptsToRegister[8]  = 15;
  InterruptsToRegister[9]  = 52;
  InterruptsToRegister[10] = 21;
  InterruptsToRegister[11] = 35;
  
  for(index=0;index< 12;index++)
  {
    if (DAL_SUCCESS != DalInterruptController_RegisterISR(intr_handle, 
                     InterruptsToRegister[index] ,(DALISR)IPCInt_TestIsr, (DALISRCtx)&InterruptsToRegister[index], 
                     DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER))
    {
      return;
    }
  }
#endif
#ifdef IPCINT_SEND_TEST
  while(mycount)
{
  if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_MDSPSW, DALIPCINT_GP_0))
  {
    return;
  }
  if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_MDSPSW, DALIPCINT_GP_1))
  {
    return;    
  }
  if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_MDSPSW, DALIPCINT_GP_2))
  {
    return;    
  }
  if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_MDSPSW, DALIPCINT_GP_3))
  {
    return;    
  }

  if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_ACPU, DALIPCINT_GP_0))
  {
    return;    
  }
  if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_ACPU, DALIPCINT_GP_1))
  {
    return;    
  }
  if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_ACPU, DALIPCINT_GP_2))
  {
    return;    
  }
  if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_ACPU, DALIPCINT_GP_3))
  {
    return;    
  }
  if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_SPSS, DALIPCINT_GP_0))
  {
    return;    
  }
  if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_SPSS, DALIPCINT_GP_1))
  {
    return;    
  }
  if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_SPSS, DALIPCINT_GP_2))
  {
    return;    
  }
  if (DAL_SUCCESS != DalIPCInt_Trigger(pIPCHandle, DALIPCINT_PROC_SPSS, DALIPCINT_GP_3))
  {
    return;    
  }
  mycount --;
}
  #endif /* IPCINT_SEND_TEST */
}

