#ifndef UCLOCK_H
#define UCLOCK_H
/*
===========================================================================
*/
/**
  @file uClock.h
  @brief Public definitions include file for accessing the micro image clock
         device driver.
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      This file is the public header file for the sensors micro image (uImage)
      clock driver implementation.  It is meant to be used by a small subset
      of drivers supporting the sensors uImage feature, which cannot access
      DDR.

===========================================================================*/
/*
  ===========================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1.c4/api/systemdrivers/uClock.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------- 
  12/17/13   dcf     Created.

  ===========================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "com_dtypes.h"


/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * Enumeration of available clocks for the uImage clock controller.  Add additional
 * clocks to the bottom of this list as they will be used to index clock tables.
 */
typedef enum
{
  CLOCK_SCC_QUPV3_SE0_CLK,
  CLOCK_SCC_QUPV3_SE1_CLK,
  CLOCK_SCC_QUPV3_SE2_CLK,
  CLOCK_SCC_QUPV3_SE3_CLK,
  CLOCK_SCC_QUPV3_SE4_CLK,
  CLOCK_SCC_QUPV3_SE5_CLK,
  CLOCK_SCC_QUPV3_S_HCLK_CLK,
  CLOCK_SCC_QUPV3_M_HCLK_CLK,
  CLOCK_SCC_QUPV3_CORE_CLK,
  CLOCK_SCC_QUPV3_2XCORE_CLK,

  CLOCK_TOTAL_CLOCK_ENUMS,

}uClockIdType;


/*=========================================================================
      Function Definitions.
==========================================================================*/


/* ============================================================================
**  Function : uClock_EnableClock
** ============================================================================
*/
/**
  Enables a clock.
    
  @param[in]  eClockId  - The ID of the clock to enable.

  @return
  TRUE  - The clock was enabled.
  FALSE - Otherwise.
  
  @dependencies
  None.

*/

boolean uClock_EnableClock(uClockIdType eClockId);


/* ============================================================================
**  Function : uClock_DisableClock
** ============================================================================
*/
/**
  Disables a clock.
    
  @param[in]  eClockId  - The ID of the clock to disable.

  @return
  TRUE  - The clock was disabled.
  FALSE - Otherwise.
  
  @dependencies
  None.

*/

boolean uClock_DisableClock(uClockIdType eClockId);


/* ============================================================================
**  Function : uClock_IsClockEnabled
** ============================================================================
*/
/**
  Returns whether a clock is enabled or not.
    
  @param[in]  eClockId  - The ID of the clock to check status on.

  @return
  TRUE  - The clock is on.
  FALSE - The clock is off.
  
  @dependencies
  None.

*/

boolean uClock_IsClockOn(uClockIdType eClockId);

#endif /* UCLOCK_H */
