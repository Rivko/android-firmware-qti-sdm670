/*===========================================================================

                    BOOT EXTERN CLK DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external api to bring subsytem/procesors
  out of reset.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2012, 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   ck      Corrected copyright
08/20/12   kedara  Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_extern_subsys_interface.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 


/* ============================================================================
**  Function : boot_clock_setup_processor
** ============================================================================
*/
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
boolean boot_clock_setup_processor(ClockProcessorType eProcessorType)
{
  return Clock_SetupProcessor(eProcessorType);
}

/* =========================================================================
**  Function : boot_clock_enable_processor
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
boolean boot_clock_enable_processor(ClockProcessorType eProcessorType)
{
  return Clock_EnableProcessor(eProcessorType);
}
