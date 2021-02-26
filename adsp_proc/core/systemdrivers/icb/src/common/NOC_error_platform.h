#ifndef __NOC_ERROR_PLATFORM_H__
#define __NOC_ERROR_PLATFORM_H__
/*============================================================================

FILE:      NOC_error_platform.h

DESCRIPTION: Platform specific function and data structure declarations 
             for AHB timeout driver Interface

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/core.qdsp6/2.1/systemdrivers/icb/src/common/NOC_error_platform.h#1 $
$DateTime: 2017/07/21 22:10:47 $
$Author: pwbldsvc $
$Change: 13907704 $ 

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2017/02/07  sds    Update for Hoya
2014/12/09  tb     Split OEM modifiable data from internal data
2014/09/03  tb     Created

                Copyright (c) 2014, 2017 Qualcomm Technologies Incorporated.
                             All Rights Reserved.
                          QUALCOMM Proprietary/GTDR
============================================================================*/

#include "NOC_error.h"
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

/*===========================================================================*/
/**
@brief 
      Handler function for NOC error interrupt.
 
@param[in]  void* arg Context data for client.

@return    
      void* arg.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*==============================================================================*/
void* NOC_Error_Handle_Interrupt(void* arg);

//*============================================================================*/
/**
@brief 
      Performs platform-specific interrupt registration for AHB Timeout driver.
 
@param[in]  *PropData Pointer to property data structure.

@return    
      bool indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
bool NOC_Error_Platform_Register_Interrupt( NOCERR_info_type *noc_info );

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
bool NOC_Error_Platform_Register_Interrupt_OS( uint32  nInterruptVector, 
                                               const   DALISR isr,
                                               const   DALISRCtx ctx, 
                                               uint32  nTrigger);

//*============================================================================*/
/**
@brief 
      Gets internal platfrom specific property data structure for configuration.
 
@param[in]  None.

@return    
      NOCERR_propdata_type* Pointer to internal property data structure.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
NOCERR_propdata_type* NOC_Error_Platform_Get_Propdata(void);

//*============================================================================*/
/**
@brief 
      Gets OEM platfrom specific property data structure for configuration.
 
@param[in]  None.

@return    
      NOCERR_propdata_type_oem* Pointer to OEM property data structure.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
NOCERR_propdata_type_oem* NOC_Error_Platform_Get_Propdata_OEM(void);

#endif /* __NOC_ERROR_PLATFORM_H__ */

