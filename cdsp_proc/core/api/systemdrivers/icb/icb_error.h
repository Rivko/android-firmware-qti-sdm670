#ifndef __ICBERROR_H__
#define __ICBERROR_H__
/*============================================================================

FILE:      icb_error.h

DESCRIPTION: Initialization for ICB bus error handlers

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/api/systemdrivers/icb/icb_error.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Author: pwbldsvc $
$Change: 16759227 $ 

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2017/06/01  sds    Add deferred NoC error programming support
2017/02/08  sds    Created

                Copyright (c) 2017 Qualcomm Technologies Incorporated.
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
      Performs initialization for NOC error handler.
      It enables interrupts required to handle NOC errors.
 
@param[in]  None.

@return    
      None.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
void NOC_Error_Init(void);

//*============================================================================*/
/**
@brief 
      Performs deferred initialization for NOC error handler.
      It enables interrupts required to handle NOC errors.
 
@param[in]  id  - the power domain being enabled

@return    
      None.

@dependencies
      NOC_Error_Init() must have been called first.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
void NOC_Error_InitDeferred(ClockPowerDomainIdType id);

#endif /* __ICBERROR_H__ */
