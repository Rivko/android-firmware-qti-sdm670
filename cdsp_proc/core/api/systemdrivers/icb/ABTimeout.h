#ifndef __ABTIMEOUT_H__
#define __ABTIMEOUT_H__
/*============================================================================

FILE:      ABTtimeout.h

DESCRIPTION: Function and data structure declarations for AHB timeout driver Interface

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/core.qdsp6/2.1.c4/api/systemdrivers/icb/ABTimeout.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Author: pwbldsvc $
$Change: 16759227 $ 

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2018/04/03  ak     Add deferred ABT programming support
2014/03/14  ddk    Added support for additional clock voting.
2014/03/10  ddk    Added support for hwio mapping.
2013/11/14  tb     Added support for multiple enable/status registers
2013/04/16  pm     Added interrupt priority
2012/10/04  av     Support for disabling ABT
2012/05/31  av     Created

                Copyright (c) 2012-2017 QUALCOMM Technologies, Incorporated.
                             All Rights Reserved.
                          QUALCOMM Proprietary/GTDR
============================================================================*/
#include "com_dtypes.h"
#include "ClockDefs.h"
/*============================================================================
                          DEFINEs/MACROs
============================================================================*/

/*============================================================================
                          TYPE DEFINITION
============================================================================*/


/*============================================================================
                                 FUNCTIONS
============================================================================*/

//*============================================================================*/
/**
@brief 
      Initializes AHB Timeout driver. It configures interrupt and timeout values
      for AHB timeout slaves
 
@param[in]  None.

@return    
      None.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
void ABT_Init(void);

//*============================================================================*/
/**
@brief 
      Performs deferred initialization for AHB Timeout driver. It configures timeout values
      for AHB timeout slaves
 
@param[in]  None.

@return    
      None.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
void ABT_InitDeferred(ClockPowerDomainIdType id);

#endif /* __ABTIMEOUT_H__ */
