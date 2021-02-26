#ifndef BOOT_EXTERN_SUBSYS_INTERFACE_H
#define BOOT_EXTERN_SUBSYS_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN CLK DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external clock drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2012, 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/14   ck       Removed feature flag as API is always enabled in XBL
08/20/12   kedara   Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "ClockPIL.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/* ============================================================================
**  Function : boot_clock_setup_processor
** ============================================================================
*/
/*!
    This function will setup the clocks for desired processor

    @param
      None
    @return
      True if success, False otherwise

    @dependencies
    None.

    @sa None
*/
boolean boot_clock_setup_processor(ClockProcessorType eProcessorType);


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
boolean boot_clock_enable_processor(ClockProcessorType eProcessorType);

#endif /* BOOT_EXTERN_SUBSYS_INTERFACE_H */
