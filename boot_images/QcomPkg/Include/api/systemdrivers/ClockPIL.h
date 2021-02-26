#ifndef CLOCKPIL_H
#define CLOCKPIL_H
/*
===========================================================================
*/
/**
  @file ClockPIL.h

  Public interface include file for accessing clock setup APIs during
  processor image loader (PIL) stage.

  The ClockPIL.h file is the public API interface to control core processor
  clocks.
*/
/*
  ====================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================


  when       who     what, where, why
  --------   ---     -----------------------------------------------------------
  01/12/12   vs      Created.

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


/*=========================================================================
      Type Definitions
==========================================================================*/

/**
 * Processor type.
 *
 * This type lists core processors running on different subsystems.
 */
typedef enum
{
  CLOCK_PROCESSOR_LPASS     = 0,  /**< LPASS Processor.  */
  CLOCK_PROCESSOR_MODEM     = 1,  /**< Modem Processor   */
  CLOCK_PROCESSOR_WCN       = 2   /**< WCN Processor.    */
} ClockProcessorType;



/*=========================================================================
      Interface Definitions
==========================================================================*/


/*=========================================================================
      Interface Declarations
==========================================================================*/

/**
  @addtogroup clock_functions
  @{
*/

/* =========================================================================
**  Function : Clock_SetupProcessor
** =========================================================================*/
/**
  Sets up processor clocks.

  This function enables required clocks on a given processor.  It initializes
  required PLL (if necessary) and runs the processor core and bus clocks from
  that PLL.  In addition, the function takes HW block(s) (but NOT core) out
  of reset to setup appropriate clocks.

  @param eProcessorType [in] -- Processor to enable clocks for.

  @return
  TRUE  -- Operation is successful.\n
  FALSE -- Operation is unsuccessful.

  @dependencies
  All required global clock control registers are accessible.
*/

boolean Clock_SetupProcessor(ClockProcessorType eProcessorType);


/* =========================================================================
**  Function : Clock_EnableProcessor
** =========================================================================*/
/**
  Brings processor out of reset.

  This function brings processor out of reset.  A client is expected to have
  called Clock_SetupProcessor before calling this function.

  @param eProcessorType [in] -- Processor to bring out of reset.

  @return
  TRUE  -- Operation is successful.\n
  FALSE -- Operation is unsuccessful.

  @dependencies
  - All required global clock control registers are accessible.
  - Processor core clocks are enabled.
  - Boot address and core memory are configured.
  - Processor core reset register is accessible.

  @sa
  Clock_SetupProcessor
*/

boolean Clock_EnableProcessor(ClockProcessorType eProcessorType);


/* =========================================================================
**  Function : Clock_ResetProcessor
** =========================================================================*/
/**
  Puts processor into reset.

  This function disables clocks (setup during Clock_EnableProcessor
  function call) and then puts processor into reset.

  @param eProcessorType [in] -- Processor to put into reset.

  @return
  TRUE  -- Operation is successful.\n
  FALSE -- Operation is unsuccessful.

  @dependencies
  All required global clock control registers are accessible.

  @sa
  Clock_EnableProcessor
*/

boolean Clock_ResetProcessor(ClockProcessorType eProcessorType);


/* =========================================================================
**  Function : Clock_InitBusTimeout
** =========================================================================*/
/**
  Enables the necessary clocks for the bus timeout slaves.

  This function enables the clocks which are necessary to use the bus
  timeout slaves.

  @dependencies
  None.
*/

void Clock_InitBusTimeout (void);

#endif /* !CLOCKPIL_H */

