#ifndef __GPIOIntDirConn_H
#define __GPIOIntDirConn_H
/*===========================================================================

  G P I O   I N T E R R U P T   C O N T R O L L E R   D I R  C O N N

DESCRIPTION
  This modules contains the Direct connect-interrupt specific code for GPIOInt.

REFERENCES

       Copyright (c) 2015 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/GPIOInt/inc/GPIOInt_DirConn.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/18/2015  cpaulo  First draft created. 
===========================================================================*/
#include "DDIGPIOInt.h"
#include "DDIInterruptController.h"
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
GPIOIntrDirConn_Init(DalDeviceHandle* pInterruptHandle);
DALResult
GPIOIntrDirConn_DeInit(void);
DALResult
GPIOIntrDirConn_TriggerInterrupt( uint32 gpio, 
                                  uint32 interrupt_id,
                                  uint8  uImage );
DALResult
GPIOIntrDirConn_ClearInterrupt( uint32 interrupt_id );
DALResult
GPIOIntrDirConn_EnableInterrupt( uint32 gpio, 
                                 uint32 interrupt_id,
                                 HAL_gpioint_InterruptType intr_type  );
DALResult
GPIOIntrDirConn_DisableInterrupt( uint32 gpio, 
                                  uint32 interrupt_id,
                                  HAL_gpioint_InterruptType intr_type );
DALResult
GPIOIntrDirConn_IsInterruptSet( uint32* state, uint32 irq );
DALResult
GPIOIntrDirConn_IsInterruptPending( uint32* state, uint32 irq );
DALResult
GPIOIntrDirConn_IsInterruptEnabled( uint32* state, uint32 irq );
DALResult
GPIOIntrDirConn_ConfigureGPIOInterrupt( uint32 gpio, 
                                        uint32 interrupt_id,
                                        uint32 direct_connect_trigger,
                                        HAL_gpioint_TriggerType trigger,
                                        GPIOIntDataType* state,
                                        boolean bIsDirectConfig,
                                        uint8 uImage );
DALResult
GPIOIntrDirConn_UnConfigureGPIOInterrupt( uint32 gpio, 
                                          uint32 interrupt_id,
                                          uint8  uimage );
DALResult
GPIOIntrDirConn_SetTrigger( uint32 gpio, 
                            uint32 interrupt_id,
                            GPIOIntTriggerType trigger,
                            uint32 direct_connect_trigger,
                            boolean bIsDirectInt );

#endif /*__GPIOIntDirConn_H*/
