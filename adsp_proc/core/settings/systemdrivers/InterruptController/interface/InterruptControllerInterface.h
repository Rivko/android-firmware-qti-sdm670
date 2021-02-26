#ifndef _INTERRUPTCONTROLLER_INTERFACE_H_
#define _INTERRUPTCONTROLLER_INTERFACE_H_
/*==============================================================================

FILE:      InterruptControllerInterface.h

DESCRIPTION: This file defines the interrupt controller data that is used
             in device configuration of the interrupt controller driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2012-2017 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/settings/systemdrivers/InterruptController/interface/InterruptControllerInterface.h#2 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------
02/13/17   dcf     Initial version.

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*=========================================================================

                           CONSTANTS

===========================================================================*/


/*=========================================================================

                           TYPE DEFINITIONS

===========================================================================*/

#define MAX_QURT_IRQ 310
#ifndef DALINTERRUPT_SUPPORTED_INTERRUPTS
  #define DALINTERRUPT_SUPPORTED_INTERRUPTS MAX_QURT_IRQ
#else
  #define DALINTERRUPT_MEMORY_SAVE_MODE
#endif


/*
 * IST THREAD PRIORITY  LIMITS
 *
 * Base IST priority level. This is the lowest priority of an interrupt service thread.
 * Highest priority level. These two limits are checks on the range of the Interrupt 
 * Thread priorities that can be set.
 */
#define IST_THREAD_PRIORITY_BASE    8
#define IST_THREAD_HIGHEST_PRIORITY 1


 /*
  *  Invalid interrupt value for error checking
  */
#define INVALID_INTERRUPT        0x7FFF
#define IST_TASKNAME_SIZE 16
/* 
 * For each interrupt handler thread a Default IST stack size is declared.
*/
#define DAL_IST_STACK_SIZE       2048
#define DAL_IST_BASE_STACK_SIZE  1024
#define DAL_IST_STACK_SIZE_4k    4096

/* Type of IST configuration */
#define DAL_IST_CONFIG_DYNAMIC      1

/*
 * Interrupt Controller internal interrupt state flags.
 */
#define INTERRUPTF_DISABLED      0x00000002
#define INTERRUPTF_CLEARAFTER    0x00000004
#define INTERRUPTF_REGISTERED    0x00000008
#define INTERRUPTF_RUNNING       0x00000010
#define INTERRUPTF_SUSPENDED     0x00000020
#define INTERRUPTF_FAKETRIGGER   0x00000040
#define INTERRUPTF_UINTERRUPT    0x00000080

/*
 * Return signals for registering interrupts
 */
#define INTERRUPT_REGISTER_SUCCESS  0x1
#define INTERRUPT_REGISTER_FAIL     0x2

/*=========================================================================

                           TYPE DEFINITIONS

===========================================================================*/

/*
 * InterruptConfigType
 */
typedef struct
{
  uint16                      nInterruptVector;
  uint16                      nTrigger;
  char                       *pInterruptName;
  
} InterruptConfigType;

 /*
  * InterruptPlatformDataType
  *
  * pIRQConfigs            - Pointer to an array of interrupt definitions in the BSP.
  * nMaxIRQ                - Maximum number of supported interrupts.
  */
typedef struct
{
  InterruptConfigType       *pIRQConfigs;
  uint32                    nMaxIRQ;
} InterruptPlatformDataType;


#endif /* _INTERRUPTCONTROLLER_INTERFACE_H_ */

