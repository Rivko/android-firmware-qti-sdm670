#ifndef CLOCK_INTERNAL_H
#define CLOCK_INTERNAL_H
/*
===========================================================================
*/
/**
  @file ClockSDCInternal.h 
  
  Internal header file for the micro clock settings.
*/
/*  
  ====================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/clock/image/slpi/hw/sdm855/uClockInternal.h#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  12/03/16   dcf     Created.

  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/


/*=========================================================================
      Macro Definitions
==========================================================================*/

#define UCLOCK_QDI_CLOCK_CMD_ROUTER               (1011+QDI_PRIVATE)
#define UCLOCK_QDI_SET_ENABLE_CLOCK               (1011+QDI_PRIVATE)
#define UCLOCK_QDI_SET_DISABLE_CLOCK              (1012+QDI_PRIVATE)

/*=========================================================================
      Type Definitions
==========================================================================*/

typedef enum
{
  UCLOCK_CMD_ENABLE_CLOCK,
  UCLOCK_CMD_DISABLE_CLOCK,
  UCLOCK_CMD_IS_CLOCK_ON,
  
}uClockCmdType;


/*=========================================================================
      Type Definitions
==========================================================================*/

typedef struct
{
  uint32 nCbcAddr;
  uint32 nVoteAddr;
  uint32 nVoteBmsk;
  
}uClockRegType;

/*=========================================================================
      Function Definitions
==========================================================================*/

/*
 * Micro uClock QDI layer driver name.
 */
#define uClockQDIName                            "/dev/uClockQDI"


#endif /* !CLOCK_INTERNAL_H */

