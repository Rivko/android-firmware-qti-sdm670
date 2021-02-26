#ifndef __UGPIOInt_H__
#define __UGPIOInt_H__
/*===========================================================================*/

/*    M I C R O   I M A G E   G P I O   I N T E R R U P T   C O N T R O L L E R    */

/**
  @file uGPIOInt.h 
  @brief This file contains the API specification for the Micro Image GPIO 
  interrupt controller device driver.The definitions in this module are 
  used externally by the clients of the GPIO interrupt controller 
  when they are executing inside the micro image. They are also called from 
  the DAL GPIO Interrupt Controller. 
*/

/* NOTE: For the output PDF generated using Doxygen and Latex, all file and 
         group descriptions are maintained in the mainpage file. To change 
         any of the the file/group text for the PDF, edit this 
         file, or contact Tech Pubs.
         The above description for this file is part of the "gpioint_api" group 
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

$Header: //components/rel/core.qdsp6/2.1/api/systemdrivers/uGPIOInt.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/
#include "com_dtypes.h"
/*
  This macro defines an invalid value that tells the different processors how to treat the GPIO configuration.
 */
#define UGPIOINT_NONE  (uint8)0xFF

/*
  This macro defines the default class of GPIO interrupts.
  Summary GPIO interrupts are supported on all Qualcomm chipsets. 
  These are leaf interrupts that each general purpose IO pin can be configured to.
  These interrupts have a single interrupt line feeding into the main interrupt controller of each processor subsystem.
  Direct connect GPIOs is a class of interrupts that was added on the new Qualcomm chipsets. Essentially any GPIO
  can be connected as a direct connect interrupt. This means that the interrupt now has a dedicated interrupt port on the Main
  Interrupt controller on the processor. Since The Direct connect Interrupts have a costly overhead on hardware there are only a few
  dedicated lines and these are hardwired to each processor in the subsystem. \n
 */
#define UGPIOINT_SUMMARY 0x7fffffff

/*
 * The error codes are to be kept in syun with the dal error codes.
 */
#define UGPIOINT_SUCCESS 0
#define UGPIOINT_ERROR -1


typedef uint32 uGPIOINTISRCtx; /**< This is the parameter to be passed to client callback when the gpio interrupt fires. */
typedef void * (*uGPIOINTISR)(uGPIOINTISRCtx); /**< This is the void function pointer client callback to be registered with the uGPIOInt driver. */


/*
  This enum is used to define the trigger condition of the GPIO interrupt.
  please note this is exactly the same as the DAL GPIOInt. It will always be in sync with the dal code.                                                                                                     .
 */
typedef enum{
  UGPIOINT_TRIGGER_HIGH,   /**< The GPIO interrupt will trigger only if the input signal is high */
  UGPIOINT_TRIGGER_LOW,  /**< The GPIO interrupt will trigger only if the input signal is low */
  UGPIOINT_TRIGGER_RISING, /**< The GPIO interrupt will trigger only if the input signal level transitions from low to high */
  UGPIOINT_TRIGGER_FALLING, /**< The GPIO interrupt will trigger only if the input signal level transitions from high to low */
  UGPIOINT_TRIGGER_DUAL_EDGE, /**< The GPIO interrupt will trigger only if the input signal level transitions from high to low or from low to high.*/
  
  PLACEHOLDER_uGPIOIntTriggerType = 0x7fffffff 
  
}uGPIOIntTriggerType;

/*
 * This enum is for return value for status
 */
typedef enum {
  UGPIOINT_TSS_INVALID          = 0x0,  /**< Timestamp is not valid */
  UGPIOINT_TSS_VALID            = 0x1,  /**< Timestamp is valid */
  UGPIOINT_TSS_INVALID_OVERFLOW = 0x10, /**< Timestamp is invalid, and there has been an overflow */
  UGPIOINT_TSS_VALID_OVERFLOW   = 0x11, /**< Timestamp is valid, and there has been an overflow */
  UGPIOINT_TSS_MAX              = 0x12  /**< Max enumeration */
}uGPIOIntTimestampStatus;

/**< Interrupt flags -- To be used with Register API to indicate special requirements */
#define UGPIOINTF_DSR           0x20 /**< Delayed Service Routine, Must call uGPIOInt_InterruptDone when the interrupt is ready to be re-enabled/acknowledged */
#define UGPIOINTF_ISLAND        0x40 /**< Island mode interrupt, this will allow for the interrupt to be handled in island mode */
#define UGPIOINTF_TIMESTAMP_EN  0x80 /**< Enable timestamping unit, this will allow the timestamp to be captured when interrupts are detected */
  
/* =========================================================================
**  Function : uGPIOInt_RegisterInterrupt
** =========================================================================*/
/*
  Registers a client callback notification for a GPIO interrupt.

  This function registers the client callback isr routine to a given GPIO 
  interrupt.Whenever an interrupt is fired the isr routine registered by the 
  user is called. \n
  
  @param[in] gpio      The GPIO pin number.
  @param[in] trigger   The interrupt trigger condition for which the client callback is being registered.
  @param[in] isr       The client ISR callback.
  @param[in] param     The Client specified parameter to be given to the client callback when the interrupt fires.
  @param[in] nFlags    The Client specified flags to be given to the interrupt controller so that internally it
                       can decide whether the interrupt is to be used in Uimage or big image. 
  
  DEPENDENCIES  None.
  @return
  UGPIOINT_SUCCESS -- Is returned if the ISR registration was a success. \n
  UGPIOINT_ERROR -- An Error if the uGPIOInt driver was not able to register the interrupt
  service routine (ISR). \n
  @dependencies
  None.
*/

int32
uGPIOInt_RegisterInterrupt( uint32 gpio,uGPIOIntTriggerType trigger,
uGPIOINTISR isr,uGPIOINTISRCtx param, uint32 nFlags);


/*=============================================================================
  FUNCTION      uGPIOInt_DeregisterInterrupt
=============================================================================*/
 /*
  Deregisters the client callback routine to a given GPIO interrupt.

  This function deregisters the client callback ISR function to a given GPIO 
  interrupt.As a result of which the interrupt is disabled. \n
   
  @param[in] gpio       The GPIO pin number.

  DEPENDENCIES  None.
  @return
  UGPIOINT_SUCCESS -- Is returned if the ISR deregistration was a success. \n
  UGPIOINT_ERROR -- Is returned if the uGPIOInt driver was not able to deregister 
  the client interrupt service routine. \n
  @dependencies
  None.
*/

int32
uGPIOInt_DeregisterInterrupt( uint32  gpio);


/*=============================================================================
  FUNCTION      uGPIOInt_IsInterruptPending
=============================================================================*/
/*
  Queries the GPIO interrupt pending state.

  This function tells the user if the given gpio interrupt is pending or not. \n
  
  @param[in] gpio       The GPIO pin number.
  @param[in] state      The pointer to a client allocated uint32 
                           variable which is populated by the uGPIOInt driver 
                           to indicate whether the interrupt is pending or not.

  DEPENDENCIES  None.
  @return
  UGPIOINT_SUCCESS -- Is returned if the gpio state query was successful. \n
  UGPIOINT_ERROR --  Is returned if the GPIO is unsupported. \n
  @dependencies
  None.
*/

int32
uGPIOInt_IsInterruptPending( uint32  gpio, uint32 *  state);


/*=============================================================================
  FUNCTION      uGPIOInt_TriggerInterrupt
=============================================================================*/
/*
  Artificially force triggers an interrupt
   
  This function is used to trigger an interrupt without the GPIO interrupt 
  being actually set in hardware. This used mainly by the vmpm driver.
   
  @param[in] gpio           The GPIO pin number.

  DEPENDENCIES  None.
  @return
  UGPIOINT_SUCCESS -- Is returned if the gpio interrupt was 
  triggered successfully. \n 
  UGPIOINT_ERROR --  Is returned if the GPIO number is invalid. 
  \n 
  @dependencies
  None.
*/

int32
uGPIOInt_TriggerInterrupt(uint32 gpio);

/*=============================================================================
  FUNCTION      uGPIOInt_MapMPMInterrupt
=============================================================================*/
/*
  This function maps the GPIO to wakeup MPM driver id.
  Its generally called directly from the MPM driver or internally
  from the Kernel GPIOInt DAL. \n

  DEPENDENCIES  None.
  @return
  UGPIOINT_SUCCESS -- Is returned if the GPIO and MPM is correctly mapped. \n
  UGPIOINT_ERROR --  Is returned if the GPIO and MPM is not set up due to the gpio being invalid. \n
  @dependencies
  None.
*/
int32 
uGPIOInt_MapMPMInterrupt
(
  
  uint32 gpio,
  uint32 mpm_id
);


/*=============================================================================
  FUNCTION      uGPIOInt_GetInterruptID
=============================================================================*/
/*
  This function gets the interrupt ID at the interrupt controller that
  this driver is running on. \n

  DEPENDENCIES  None.
  @return
  UGPIOINT_SUCCESS -- Is returned if the GPIO has a valid interrupt ID. \n
  UGPIOINT_ERROR --  Is returned if the GPIO does not have a valid interrupt ID. \n
  @dependencies
  None.
*/
int32 
uGPIOInt_GetInterruptID
(
  
  uint32 gpio,
  uint32* pnInterruptID
);


/*=============================================================================
  FUNCTION      uGPIOInt_EnableInterrupt
=============================================================================*/
/*
  This function enables the interrupt at the interrupt controller for
  a given GPIO. \n

  DEPENDENCIES  None.
  @return
  UGPIOINT_SUCCESS -- Is returned if the GPIO has successfully been enabled. \n
  UGPIOINT_ERROR --  Is returned if the GPIO was not successfully enabled. \n
  @dependencies
  None.
*/
int32 
uGPIOInt_EnableInterrupt
(
  
  uint32 gpio
);


/*=============================================================================
  FUNCTION      uGPIOInt_DisableInterrupt
=============================================================================*/
/*
  This function disables the interrupt at the interrupt controller for
  a given GPIO. \n

  DEPENDENCIES  None.
  @return
  UGPIOINT_SUCCESS -- Is returned if the GPIO has successfully been disabled. \n
  UGPIOINT_ERROR --  Is returned if the GPIO was not successfully disabled. \n
  @dependencies
  None.
*/
int32 
uGPIOInt_DisableInterrupt
(
  
  uint32 gpio
);


/*=============================================================================
  FUNCTION      uGPIOInt_EnableInterruptEx
=============================================================================*/
/*
  This function enables/disables the interrupt at the interrupt controller for
  a given GPIO, as well as at the PDC. This function is not supported in
  island mode.\n
 
  @param gpio     - Gpio to enable/disable. \n
  @param bEnable  - Boolean on whether or not to enable or disable the interrupt. \n
                    TRUE to enable, FALSE to disable.

  DEPENDENCIES  None.
  @return
  UGPIOINT_SUCCESS -- Is returned if the GPIO has successfully been enabled/disabled. \n
  UGPIOINT_ERROR --  Is returned if the GPIO was not successfully enabled/disabled. \n
  @dependencies
  None.
*/
int32 
uGPIOInt_EnableInterruptEx
(
  
  uint32  gpio,
  boolean bEnable
);


/*=============================================================================
  FUNCTION      uGPIOInt_InterruptDone
=============================================================================*/
/*
  This function tells the uGPIOInt driver that the interrupt has completed
  it's processing, and the interrupt may now be acked / re-enabled. This function
  should only be used if the UGPIOINTF_DSR flag is set at registration time. \n

  DEPENDENCIES  None.
  @return
  UGPIOINT_SUCCESS -- Is returned if the GPIO has successfully been acked. \n
  UGPIOINT_ERROR --  Is returned if the GPIO was not successfully acked. \n
  @dependencies
  None.
*/
int32 
uGPIOInt_InterruptDone
(
  uint32 gpio
);


/*=============================================================================
  FUNCTION      uGPIOInt_TimestampRead
=============================================================================*/
/*
  This function returns the value returned by the timestamping unit in the PDC
  for when the interrupt fired. \n

  DEPENDENCIES  None.
  @return
  UGPIOINT_SUCCESS -- Is returned if the timestamp was successfully retrieved. \n
  UGPIOINT_ERROR --  Is returned if the timestamp unit was either not
                      configured or there is an unknown error. \n
  @dependencies
  None.
*/
int32 
uGPIOInt_TimestampRead
(
  uint32                    gpio,
  uGPIOIntTimestampStatus*  pnStatus,
  uint64*                   pnTimestamp
);



#endif  //__UGPIOInt_H__
