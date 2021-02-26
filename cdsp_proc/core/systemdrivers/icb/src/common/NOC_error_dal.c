/*==============================================================================

FILE:      ABT_tz.c

DESCRIPTION: This file implements TZ platform specific ABT driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/icb/src/common/NOC_error_dal.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Author: pwbldsvc $
$Change: 16759227 $

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2017/02/07  sds    Update for Hoya
2014/12/09  tb     Split OEM modifiable data from internal data
2014/09/03  tb     Created

        Copyright (c) 2014, 2017 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#include "NOC_error.h"
#include "NOC_error_platform.h"
#include "DALSys.h"
#include "DALStdErr.h"
#include "DALDeviceId.h"
#include "err.h"

/*============================================================================
                          DEFINEs/MACROs
============================================================================*/

/*============================================================================
                                 FUNCTIONS
============================================================================*/
//
//*============================================================================*/
/**
@brief 
      Performs platform specific initialization for AHB Timeout driver.
      It enables clock/interrupts required for AHB timeout slave.
 
@param[in]  *PropData Pointer to property data structure.

@return    
      bool indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
bool NOC_Error_Platform_Register_Interrupt( NOCERR_info_type *noc_info )
{
  // Register IRQs
  return NOC_Error_Platform_Register_Interrupt_OS(
     noc_info->intr_vector, 
     NOC_Error_Handle_Interrupt,
     (void*)( noc_info->intr_vector), 
     DALINTRCTRL_ENABLE_CONFIG(DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, 
                               noc_info->intr_priority)
     );
}


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
NOCERR_propdata_type* NOC_Error_Platform_Get_Propdata(void)
{
   DALSYSPropertyVar NOCErrorPropVar;
   DALSYS_PROPERTY_HANDLE_DECLARE(hNOCErrorProp);

   /* Get default configuration from config data */
   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr( "/dev/NOCError",
                                                      hNOCErrorProp ))
   {
     // Can't get property handle
     ERR("NOCError: Can't get DAL property handle.", 0, 0, 0);
     return NULL;
   }

   if ((DAL_SUCCESS == DALSYS_GetPropertyValue(hNOCErrorProp,
                                              "NOCError_Propdata", 0, &NOCErrorPropVar)))
   {
       if((0 >= NOCErrorPropVar.dwLen))
       {
           ERR("NOCError: Invalid property data length", 0, 0, 0);
           return NULL;
       }
       return (NOCERR_propdata_type*) NOCErrorPropVar.Val.pStruct;
   }

   ERR("NOCError: Can't get DAL property value", 0, 0, 0);
   return NULL;
}

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
NOCERR_propdata_type_oem* NOC_Error_Platform_Get_Propdata_OEM(void)
{
   DALSYSPropertyVar NOCErrorOEMPropVar;
   DALSYS_PROPERTY_HANDLE_DECLARE(hNOCErrorOEMProp);

   /* Get default configuration from config data */
   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr( "/dev/NOCErrorOEM",
                                                       hNOCErrorOEMProp ))
   {
     // Can't get property handle
     ERR("NOCError: Can't get DAL property handle.", 0, 0, 0);
     return NULL;
   }

   if ((DAL_SUCCESS == DALSYS_GetPropertyValue(hNOCErrorOEMProp,
                                              "NOCError_Propdata_oem", 0, &NOCErrorOEMPropVar)))
   {
       if((0 >= NOCErrorOEMPropVar.dwLen))
       {
           ERR("NOCError: Invalid property data length", 0, 0, 0);
           return NULL;
       }
       return (NOCERR_propdata_type_oem*) NOCErrorOEMPropVar.Val.pStruct;
   }

   ERR("NOCError: Can't get DAL property value", 0, 0, 0);
   return NULL;
}
