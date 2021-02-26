#ifndef VSENSE_H
#define VSENSE_H

/*=============================================================================
                              VSENSE 
                            Header File
GENERAL DESCRIPTION
This is the target header file for Voltage Sensor Driver.

Copyright 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/20/15   aks      Initial revision.
=============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/

#include "com_dtypes.h"

/*----------------------------------------------------------------------------
 * Function : vsense_init
 * -------------------------------------------------------------------------*/
/*!
    Description: Initialize the vsense driver :Allocate Shared Memory
    @param
      void
    @return
    TRUE/FALSE
    
-------------------------------------------------------------------------*/
boolean vsense_init(void);


/*----------------------------------------------------------------------------
 * Function : vsense_start_calibration
 * -------------------------------------------------------------------------*/
/*!
    Description: Calibrate all the supported rails 
    @param
      void
    @return
    TRUE/FALSE
    
-------------------------------------------------------------------------*/

boolean vsense_start_calibration(void);

/*----------------------------------------------------------------------------
 * Function : vsense_copy_to_smem
 * -------------------------------------------------------------------------*/
/*!
    Description: Store the calibration information into shared memory
    @param
      void
    @return
    TRUE/FALSE
    
-------------------------------------------------------------------------*/
boolean vsense_copy_to_smem(void);

#endif
