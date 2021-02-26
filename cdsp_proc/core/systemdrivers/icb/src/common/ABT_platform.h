#ifndef __ABT_PLATFORM_H__
#define __ABT_PLATFORM_H__
/*============================================================================

FILE:      ABT_platform.h

DESCRIPTION: Platform specific function and data structure declarations 
             for AHB timeout driver Interface

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/icb/src/common/ABT_platform.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Author: pwbldsvc $
$Change: 16759227 $ 

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2013/08/30  pm     Renamed ABT_Register_Interrupt() and moved it from ABT_internal.h
2012/05/31  av     Created

                Copyright (c) 2013 Qualcomm Technologies Incorporated.
                             All Rights Reserved.
                          QUALCOMM Proprietary/GTDR
============================================================================*/

#include "ABTimeout.h"
#include "ABT_internal.h"
#include "DDIInterruptController.h"

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
      Performs platform specific initialization for AHB Timeout driver.
      It enables clock/interrupts required for AHB timeout slave.
 
@param[in]  *PropData Pointer to property data structure.

@return    
      ABT_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
ABT_error_type ABT_Platform_Init( ABT_propdata_type *PropData ,  ClockPowerDomainIdType id, uint32_t slave_id);

//*============================================================================*/
/**
@brief 
      Performs platform specific de-initialization for AHB Timeout driver.
      It disables clocks required for AHB timeout slave.
 
@param[in]  *PropData Pointer to property data structure.

@return    
      ABT_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
ABT_error_type ABT_Platform_Deinit( ABT_propdata_type *PropData );

//*============================================================================*/
/**
@brief 
      Performs platform-specific interrupt registration for AHB Timeout driver.
 
@param[in]  *PropData Pointer to property data structure.

@return    
      ABT_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
ABT_error_type ABT_Platform_Register_Interrupt( ABT_propdata_type *PropData );

/*==========================================================================*/
/**
@brief 
      This function registers the interrupt service routine.  A different
      method is used depending on the OS (e.g. DAL vs. QuRT)
 
@param[in] nInterruptVector    ID of the interrupt vector with which the client
                               wants to register a callback. 
@param[in] isr      Callback function to be registered.
@param[in] ctx      Parameter associated with the callback function.
@param[in] nTrigger Client must use one of the interrupt flags, which
                       contain the interrupt triggers. The trigger values are: \n
                       - DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER   -- 0x00000002
                       - DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER    -- 0x00000003
                       - DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER  -- 0x00000004
                       - DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER -- 0x00000005
                         @tablebulletend
                    Optionally, bits 31-24 contain the interrupt priority desired
  
@return
  ABT_SUCCESS -- Interrupt was enabled successfully and
                 client notification was registered. \n
  ABT_ERROR_INIT_FAILURE -- Otherwise
 
@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*==============================================================================*/
ABT_error_type ABT_Platform_Register_Interrupt_OS( uint32  nInterruptVector, 
                                                   const   DALISR isr,
                                                   const   DALISRCtx ctx, 
                                                   uint32  nTrigger);

//*============================================================================*/
/**
@brief 
      Logs syndrome register infomration using platform specific APIs.
 
@param[in]  *syndrome Pointer to syndrome info structure

@return    
      ABT_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
ABT_error_type ABT_Platform_Log_Syndrome(ABT_syndrome_info_type *syndrome);

//*============================================================================*/
/**
@brief 
      Logs syndrome register infomration using platform specific APIs.
 
@param[in]  hung_slaves Number of hung slaves.

@return    
      ABT_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
ABT_error_type ABT_Platform_Report_Hang(uint32 hung_slaves,ABT_syndrome_info_type *syndrome);

//*============================================================================*/
/**
@brief 
      Gets platfrom specific property data structure for configuration.
 
@param[in]  None.

@return    
      ABT_propdata_type* Pointer to property data structure.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
ABT_propdata_type* ABT_Platform_Get_Propdata(void);

#endif /* __ABT_PLATFORM_H__ */

