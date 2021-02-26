#ifndef __GPIOIntSummary_H
#define __GPIOIntSummary_H
/*===========================================================================

  G P I O   I N T E R R U P T   C O N T R O L L E R   S U M M A R Y

DESCRIPTION
  This modules contains the Summary-interrupt specific code for GPIOInt.

REFERENCES

       Copyright (c) 2015 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/GPIOInt/inc/GPIOInt_Summary.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/18/2015  cpaulo  First draft created. 
===========================================================================*/
#include "DDIGPIOInt.h"
#include "GPIOInt.h"
#include "HALgpioint.h"
#include "uGPIOInt.h"
#include "busywait.h"

/*=========================================================================

                           DATA DEFINITIONS

===========================================================================*/

/*=========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/

/*
 * Need to add documentation if required
 */

DALResult
GPIOIntrSummary_Init(GPIOIntDevCtxt *device, DalDeviceHandle* pInterruptHandle);
DALResult
GPIOIntrSummary_TriggerInterrupt( uint32 gpio );
DALResult
GPIOIntrSummary_ClearInterrupt( uint32 gpio );
DALResult
GPIOIntrSummary_EnableInterrupt( uint32 gpio, 
                                 HAL_gpioint_ProcessorType processor,
                                 HAL_gpioint_InterruptType intr_type  );
DALResult
GPIOIntrSummary_DisableInterrupt( uint32 gpio );
DALResult
GPIOIntrSummary_IsInterruptSet( uint32 gpio, uint32* state );
DALResult
GPIOIntrSummary_IsInterruptPending( uint32 gpio, uint32* state );
DALResult
GPIOIntrSummary_IsInterruptEnabled( uint32 gpio, uint32* state );
DALResult
GPIOIntrSummary_ConfigureGPIOInterrupt( GPIOIntDevCtxt * device,
                                        uint32 gpio, 
                                        HAL_gpioint_TriggerType trigger,
                                        HAL_gpioint_ProcessorType processor,
                                        HAL_gpioint_InterruptType intr_type );
DALResult
GPIOIntrSummary_UnConfigureGPIOInterrupt( GPIOIntDevCtxt * device,
                                          uint32 gpio, 
                                          uint32 interrupt_id,
                                          uint8  uimage );
DALResult
GPIOIntrSummary_SetTrigger( uint32  gpio, GPIOIntTriggerType  trigger );

/* PD specific functions */
DALResult
GPIOIntrSummaryInternal_Init(GPIOIntDevCtxt *device, DalDeviceHandle* pInterruptHandle);
DALResult
GPIOIntrSummaryInternal_UnConfigureGPIOInterrupt( GPIOIntDevCtxt * device, uint32 gpio, uint8 uimage );
DALResult
GPIOIntrSummaryInternal_ConfigureGPIOInterrupt( GPIOIntDevCtxt * device, uint32 gpio );



#endif /*__GPIOIntSummary_H*/
