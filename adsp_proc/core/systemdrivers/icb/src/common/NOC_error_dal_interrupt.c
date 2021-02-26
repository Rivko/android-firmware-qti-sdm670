/*==============================================================================

FILE:      NOC_error_dal_interrupt.c

DESCRIPTION: This file implements registering the NOC interrupts via the DAL 
interface.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2013, 2017 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/icb/src/common/NOC_error_dal_interrupt.c#1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/23/17   sds     Branched for NoC Error
08/30/13   pm      Renamed ABT_Register_Interrupt
04/15/13   pm      New file to allow ABT_qurt_interrupt.c to have QuRT method 

===========================================================================*/


/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "NOC_error_platform.h"
#include "DDIInterruptController.h"
#include "DALDeviceId.h"
#include <DALSys.h>
#include <DALSysCmn.h>
#include "err.h"

/*==========================================================================*/
/**
@brief 
      This function registers the interrupt service routine
 
@param[in] nInterruptVector    ID of the interrupt vector with which the client
                               wants to register a callback. 
@param[in] isr      Callback function to be registered.
@param[in] ctx      Parameter associated with the callback function.
@param[in] nTrigger Client must use one of the interrupt flags, which
                       contain the interrupt triggers. The trigger values are: \n
                       - DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER   -- 0x00000002
                       - DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER    -- 0x00000003
                       - DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER  -- 0x00000004
                       - DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER -- 0x00000005
                         @tablebulletend
                    Optionally, bits 31-24 contain the interrupt priority desired
  
@return
  ABT_SUCCESS -- Interrupt was enabled successfully and
                 client notification was registered. \n
  ABT_ERROR_INIT_FAILURE -- Otherwise
 
@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*==========================================================================*/
bool NOC_Error_Platform_Register_Interrupt_OS
(
  uint32  nInterruptVector, 
  const   DALISR isr,
  const   DALISRCtx ctx, 
  uint32  nTrigger
) 
{
   DalDeviceHandle *hInthandle;

   // Register IRQs
   if(DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &hInthandle))
   {
      ERR_FATAL("NOCError: ISR registration failed !", 0, 0, 0);
      return false;
   }

   if(DAL_SUCCESS != DalInterruptController_RegisterISR(hInthandle,
                                                        nInterruptVector,
                                                        isr,
                                                        ctx,
                                                        nTrigger))
   {
      ERR_FATAL("NOCError: ISR registration failed !", 0, 0, 0);
      return false;
   }

   return true;
}

