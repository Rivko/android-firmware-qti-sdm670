#ifndef __UINTERRUPT_H__
#define __UINTERRUPT_H__
/*===========================================================================*/

/*    M I C R O   I M A G E   I N T E R R U P T   C O N T R O L L E R    */

/**
  @file uInterruptController.h 
  @brief This file contains the API specification for the Micro Image
  interrupt controller device driver.The definitions in this module are 
  used externally by the clients of the interrupt controller 
  when they are executing inside the micro image.
*/

/* NOTE: For the output PDF generated using Doxygen and Latex, all file and 
         group descriptions are maintained in the mainpage file. To change 
         any of the the file/group text for the PDF, edit this 
         file, or contact Tech Pubs.
         The above description for this file is part of the "interruptcontroller_api" group 
         description in the mainpage file. 
*/

/*===========================================================================
  Copyright (c) 2014-2017 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/api/systemdrivers/uInterruptController.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/
#include "com_dtypes.h"

/*
  This macro defines the error flags returned by the uInterruptController.
 */

#define UINTERRUPT_SUCCESS 0
#define UINTERRUPT_ERROR -1
#define UINTERRUPT_ERROR_EVAL -2
#define UINTERRUPT_ERROR_EINT -3
#define UINT_NONE  (uint8)0xFF

typedef void * uIRQCtx;            /* Client callback parameter. */
typedef void * (*uIRQ)(uIRQCtx);   /* Client callback. */

/** Default interrupt trigger value.
*/
#define UINTERRUPTCONTROLLER_DEFAULT_SETTINGS        0x00000001 

/** Interrupt is triggered only if the input signal at the source is high.
*/
#define UINTERRUPTCONTROLLER_LEVEL_HIGH_TRIGGER      0x00000002 

/** Interrupt is triggered only if the input signal at the source is low.
*/
#define UINTERRUPTCONTROLLER_LEVEL_LOW_TRIGGER       0x00000003 

/** Interrupt is triggered only if the input signal at the source transitions
from low to high.
*/
#define UINTERRUPTCONTROLLER_RISING_EDGE_TRIGGER     0x00000004 

/** Interrupt is triggered only if the input signal at the source transitions
from high  to low.
*/
#define UINTERRUPTCONTROLLER_FALLING_EDGE_TRIGGER    0x00000005 

/*=============================================================================
  FUNCTION      uInterruptController_IsInterruptPending
=============================================================================*/
/*
  Queries the interrupt pending state.

  This function tells the user if the given interrupt is pending or not. \n
  
  @param[in] nInterruptID  The Interrupt vector number.
  @param[in] state         The pointer to a client allocated uint32 
                           variable which is populated by the uInterruptController driver 
                           to indicate whether the interrupt is pending or not.
 
  
  DEPENDENCIES  None.
  @return
  UINTERRUPT_SUCCESS -- Is returned if the Interrupt state query was successful. \n
  UINTERRUPT_ERROR --  Is returned if the interrupt is unsupported. \n
  @dependencies
  None.
*/

int32
uInterruptController_IsInterruptPending( uint32 nInterruptID, uint32 *  state);


/*=============================================================================
  FUNCTION      uInterruptController_TriggerInterrupt
=============================================================================*/
/*
  Artificially force triggers an interrupt
   
  This function is used to trigger an interrupt without the interrupt 
  being actually set in hardware. This used mainly by the vmpm driver.
   
  @param[in] nInterruptID      The Interrupt vector number.

  DEPENDENCIES  None.
  @return
  UINTERRUPT_SUCCESS -- Is returned if the interrupt was 
  triggered successfully. \n 
  UINTERRUPT_ERROR --  Is returned if the interrupt vector number is invalid. 
  \n 
  @dependencies
  None.
*/

int32
uInterruptController_TriggerInterrupt(uint32 nInterruptID);

/*=============================================================================
  FUNCTION      uInterruptController_Enable
=============================================================================*/
/*
  This function is used to enable or disable an island interrupt from within
  island mode.
   
  @param[in] nInterruptID      The Interrupt vector number.
  @param[in] bEnable           Enable or disable the interrupt.

  DEPENDENCIES  None.
  @return
  UINTERRUPT_SUCCESS -- Is returned if the interrupt was 
                        enabled/disabled successfully. \n 
  UINTERRUPT_ERROR --  Is returned if the interrupt vector number is invalid
                        in this driver, or unspecified error.
  UINTERRUPT_ERROR_EVAL -- Is returned if the interrupt is not registered
                            at the kernel.
  UINTERRUPT_ERROR_EINT -- Is returned if the interrupt number passed in
                            is invalid at the kernel.
 
  @dependencies
  Interrupt has to be successfully registered in the IST which can take an
    unspecified amount of time.
*/

int32
uInterruptController_Enable(uint32 nInterruptID, boolean bEnable);

#endif  //__UINTERRUPT_H__
