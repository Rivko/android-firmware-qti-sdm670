#ifndef __UGPIOIntUser_H
#define __UGPIOIntUser_H
/*===========================================================================

  D A L   G P I O   I N T E R R U P T   C O N T R O L L E R    

DESCRIPTION
  This modules contains enumeration definitions to support the Dal GPIO 
  interrupt controller. The definitions in this module are used internally 
  in the GPIO interrupt controller and are not to be exposed to the clients.

REFERENCES

       Copyright (c) 2012-2015 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/GPIOInt/src/user/uimage/uGPIOIntUser.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/

#include "qurt.h"
#include "uGPIOInt.h"
#include "uGPIOIntInternal.h"
#include "DDIGPIOMgr.h"
#include "qurt_island.h"
/*------------------------------------------------------------------------------
Declaring uGPIOInt Driver data
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 GPIO interrupt controller constant declarations.
------------------------------------------------------------------------------*/

#define UGPIOIST_TASKNAME_SIZE 16
/*
 * As an Upper limit we keep the maximum number of GPIOs as 150.
 * This number needs to be updated if the GPIO number exceeds 150.
 * Also the number of GPIOs supported in Uimage is 9 is a direct 1 to 1 relation to the number of direct connects currently available in Sensors.
 */
#define MAX_NUMBER_OF_UGPIOS        18
#define MAX_NUMBER_OF_GPIOS         150
#define UGPIOINTF_MPMINT            0x02
#define UGPIOINTF_WAKEUP            0x04
#define UGPIOINTF_REGISTERED        0x08
#define UGPIOINTF_DIRECTINT_CONFIG  0x10 /** Direct line to IntC */ 
//#define UGPIOINTF_DSR             0x20 /** Defined in uGPIOInt.h */
//#define UGPIOINTF_ISLAND          0x40 /** Defined in uGPIOInt.h */
#define UGPIOINT_IST_STACK_SIZE     1792

/*
 * Task signals are going to be the offset of which point they exist in the state table                        .
 * state[0] will be 0x1 << 0                                                                                                            .
 * state[1] will be 0x1 << 1                                                                                                                                     .
 * One signal is reserved: 
 * SIG_INT_ABORT == 0x80000000 
 *  
 */

#define UGPIOINT_TASK_INTMASK   0x7FFFFFFF

#define UGPIOINT_INT_NONE 0xFFFF

/* #define SIG_INT_ABORT 0x80000000 */


/*=========================================================================

                           DATA TYPE DEFINITIONS

===========================================================================*/

/*
 * GPIOIntdata_type
 * Structure storing the details for a SUMMARY group interrupt
 */
typedef struct
{
  uGPIOINTISR                    isr;
  uGPIOINTISRCtx                 isr_param;
  uint32                         nInterruptMask;
  qurt_anysignal_t*              pAckSig;
  uint16                         qurt_intr_id;
  uint8                          intr_trigger;
  uint8                          gpio_intr_flags;
  uint8                          nGPIO;
  uint8                          nAltIdx;
} uGPIOIntDataType;
 

/*
 * uGPIOIntConfigMapType
 * Structure storing the interrupt data map accross all processors.
 * it matches with the DAL GPIOInt driver.
 * uint32          gpio;         GPIO pin number
 * uint32          interrupt_id; l1 main interrupt line for direct connects.
 * uint32          polarity;     polarity of the interrupt.
 */
typedef struct
{
  uint32                                 gpio;
  uint32                                 DirConnIntId;
  uint32                                 qurt_interrupt_id;
  uint32                                 flags;
} uGPIOIntConfigMapType;

/*
 * GPIOIntCntrlType
 *
 * Container for all local data.
 *
 * initialized: Indicates if the driver has been started or not.  Needed
 *              mostly because some compilers complain about empty structs.
 * table:       Table of registered GPIO_INT handler functions.
 * wakeup_isr:  ISR to invoke when a monitored GPIO interrupt triggers.
 */
typedef struct
{
  /* GPIOInt Dev state can be added by developers here */

  /* interrupt_state Table of registered GPIO_INT handler functions */
  DalDeviceHandle *              hGPIOMgr;
  uGPIOIntDataType               state[MAX_NUMBER_OF_UGPIOS];
  uint8                          aGPIOLUT[MAX_NUMBER_OF_GPIOS];
  qurt_thread_t                  nThreadID;
  uint8                          aISTStack[UGPIOINT_IST_STACK_SIZE];
  char                           aISTName[UGPIOIST_TASKNAME_SIZE];
  qurt_anysignal_t               ISTSignal;
  qurt_thread_attr_t             ThreadAttr;
  qurt_island_t                  gsSpec;
  uint32                         nPriority;
  uint32                         nGPIOIntMask;
  uint32                         nGPIOIntRegistrationMask;
  uint32                         nISTStackSize;
  qurt_mutex_t                   uGPIOIntLock;
  
  /* Configuration map for direct connect interrupts.*/
  uGPIOIntConfigMapType          *puGPIOIntConfigMap;

  /* Flag to Initialize GPIOInt_Init is called first 
   * before anything else can attach 
   */
  uint8                          uGPIOInt_Init;
  int                            gpioint_qdi;

} uGPIOIntCntrlType;


/*=========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

#endif /*__UGPIOIntUser_H */
