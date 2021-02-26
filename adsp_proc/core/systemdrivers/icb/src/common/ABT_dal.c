/*==============================================================================

FILE:      ABT_dal.c

DESCRIPTION: This file implements DAL platform specific ABT driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/core.qdsp6/2.1/systemdrivers/icb/src/common/ABT_dal.c#3 $ 
$DateTime: 2018/04/06 07:36:35 $
$Author: pwbldsvc $
$Change: 15866471 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2018/04/03  ak     Add deferred ABT programming support
2015/03/27  tb     Added clock frequency call
2013/08/30  pm     Split out interrupt reg from ABT_init
2013/04/16  pm     Added interrupt priority and split out interrupt registration
2012/05/31  av     Created
 
        Copyright (c) 2012 - 2015 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#include "ABTimeout.h"
#include "ABT_internal.h"
#include "ABT_platform.h"
#include "DALSys.h"
#include "DALStdErr.h"
#include "DALDeviceId.h"
#include "DDIInterruptController.h"
#include "DDIClock.h"
#include "err.h"
#include "ClockDefs.h"

/*============================================================================
                          DEFINEs/MACROs
============================================================================*/
DalDeviceHandle *pHandle = NULL;
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
ABT_error_type ABT_Platform_Init( ABT_propdata_type * PropData, ClockPowerDomainIdType id , uint32_t slave_id)
{
  uint32_t clock_num;

  // Enable Slave Clocks
  if(pHandle == NULL && DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_CLOCK, &pHandle))
  {
      ERR_FATAL("ABTimeout: Device attach to Clock failed.", 0, 0, 0);
      return ABT_ERROR_INIT_FAILURE;
  }  

     /* Initialize power domain, if present . This is done only  once during init time */
  if( NULL != PropData->slave_info[slave_id].power_domain && PropData->slave_info[slave_id].power_domain_id == 0)
  {
    //Get power domain to enable GDS
        if(DAL_SUCCESS != DalClock_GetPowerDomainId(pHandle,
                                   PropData->slave_info[slave_id].power_domain,
                                   &PropData->slave_info[slave_id].power_domain_id))
    {
        ERR_FATAL("ABTimeout: Get power domain ID failed.", 0, 0, 0);
        return ABT_ERROR_INIT_FAILURE;
    }
   
    //Enable GDS
    if(PropData->slave_info[slave_id].deferral_pref == ABT_DEFER_NONE && DAL_SUCCESS != DalClock_EnablePowerDomain(pHandle, PropData->slave_info[slave_id].power_domain_id))
    {
        ERR_FATAL("ABTimeout: Enable power domain failed.", 0, 0, 0);
        return ABT_ERROR_INIT_FAILURE;
    }
  }
      if (PropData->slave_info[slave_id].power_domain_id && id && PropData->slave_info[slave_id].power_domain_id != id )
      {
          return ABT_ERROR_INVALID_PARAM;
      }
  
      for(clock_num = 0; 
            clock_num < PropData->slave_info[slave_id].num_of_clk ; 
            clock_num++)
      {   
        if (PropData->slave_info[slave_id].clk_id_array[clock_num] && PropData->slave_info[slave_id].clk_disable_array[clock_num] == FALSE)
        {
          continue ;
        }
        if(PropData->slave_info[slave_id].clk_id_array[clock_num] == 0 && DAL_SUCCESS != 
              DalClock_GetClockId(pHandle, 
                               PropData->slave_info[slave_id].clk_name_array[clock_num],
                               &PropData->slave_info[slave_id].clk_id_array[clock_num]))
        {
          ERR_FATAL("ABTimeout: Could not get ClockId.", 0, 0, 0);
          return ABT_ERROR_INIT_FAILURE;
        }

        if(DAL_SUCCESS != DalClock_EnableClock(pHandle, PropData->slave_info[slave_id].clk_id_array[clock_num]))
        {
          ERR_FATAL("ABTimeout: Could not enable clock.", 0, 0, 0);
          return ABT_ERROR_INIT_FAILURE;
        }
      }

  return ABT_SUCCESS;
}

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
ABT_error_type ABT_Platform_Deinit( ABT_propdata_type * PropData )
{
  uint32_t slave_id,clock_num;

  for(slave_id = 0; slave_id < PropData->len; slave_id++)
  {     
    for(clock_num = 0; 
          clock_num < PropData->slave_info[slave_id].num_of_clk ; 
          clock_num++)
    {   
      if(PropData->slave_info[slave_id].clk_disable_array[clock_num])
      {
        if(DAL_SUCCESS != DalClock_DisableClock(pHandle, PropData->slave_info[slave_id].clk_id_array[clock_num]))
        {
          ERR_FATAL("ABTimeout: Could not disable clock.", 0, 0, 0);
          return ABT_ERROR_INIT_FAILURE;
        }
      }
    }

    //Disable GDS
    if(PropData->slave_info[slave_id].deferral_pref == ABT_DEFER_NONE && PropData->slave_info[slave_id].power_domain_id && DAL_SUCCESS != DalClock_DisablePowerDomain(pHandle, PropData->slave_info[slave_id].power_domain_id))
    {
        ERR_FATAL("ABTimeout: Disable power domain failed.", 0, 0, 0);
        return ABT_ERROR_INIT_FAILURE;
    }
  }


  return ABT_SUCCESS;
}

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
ABT_error_type ABT_Platform_Register_Interrupt( ABT_propdata_type * PropData )
{
  // Register IRQs
  return ABT_Platform_Register_Interrupt_OS(
     PropData->platform_info->intr_vector, 
     ABT_Handle_Interrupt,
     NULL, 
     DALINTRCTRL_ENABLE_CONFIG(DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, 
                               PropData->platform_info->intr_priority)
     );
}

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
ABT_error_type ABT_Platform_Log_Syndrome(ABT_syndrome_info_type *syndrome)
{
    ERR("ABTimeout: Bus %s hang; Syndrome ID: 0x%x", 
                  syndrome->slave_name, syndrome->id, 0);
    ERR("ABTimeout: Bus %s hang; Syndrome ADDR0: 0x%x", 
                  syndrome->slave_name, syndrome->addr0, 0);
    ERR("ABTimeout: Bus %s hang; Syndrome ADDR1: 0x%x", 
                  syndrome->slave_name, syndrome->addr1, 0);
    ERR("ABTimeout: Bus %s hang; Syndrome HREADY: 0x%x", 
                  syndrome->slave_name, syndrome->hready, 0);
    ERR("ABTimeout: Bus %s hang; Number of Slaves: %d", 
                  syndrome->slave_name, syndrome->num_slaves, 0);
    return ABT_SUCCESS;
}

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
ABT_error_type ABT_Platform_Report_Hang(uint32 hung_slaves,ABT_syndrome_info_type *syndrome)
{
    if(syndrome)
    {  
        ERR_FATAL("ABTimeout: AHB Bus hang is detected, Number of bus hang detected := %d , addr0 = 0x%x , addr1 = 0x%x!!!",
                 hung_slaves, syndrome->addr0, syndrome->addr1);
    }
    else
    {
      ERR_FATAL("ABTimeout: AHB Bus hang is detected, Number of bus hang detected := %d !!!",
                 hung_slaves, 0, 0);
    }
      return ABT_SUCCESS;
}

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
ABT_propdata_type* ABT_Platform_Get_Propdata(void)
{
   DALSYSPropertyVar ABTPropVar;
   DALSYS_PROPERTY_HANDLE_DECLARE(hABTProp);

   /* Get default configuration from config data */
   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr( "/dev/ABTimeout",
                                                      hABTProp ))
   {
     // Can't get property handle
     ERR("ABTimeout: Can't get DAL property handle.", 0, 0, 0);
     return NULL;
   }

   if ((DAL_SUCCESS == DALSYS_GetPropertyValue(hABTProp,
                                              "ABT_Propdata", 0, &ABTPropVar)))
   {
       if((0 >= ABTPropVar.dwLen))
       {
           ERR("ABTimeout: Invalid property data length", 0, 0, 0);
           return NULL;
       }
       return (ABT_propdata_type*) ABTPropVar.Val.pStruct;
   }

   ERR("ABTimeout: Can't get DAL property value", 0, 0, 0);
   return NULL;
}


