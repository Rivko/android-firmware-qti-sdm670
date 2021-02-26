/*! \file pm_target_information.c
*  
*  \brief This file contains target specific PMIC settings common across processors.
*  \n   
*  &copy; Copyright 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.qdsp6/2.1/pmic/pm/framework/src/pm_target_information.c#6 $ 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_target_information.h"
#include "DALSys.h"
#include "DDIChipInfo.h"
#include "DDIChipInfo.h"
#include "DDIPlatformInfo.h"
#include "pm_err_flags.h"
#include "pm_ulog.h"
#include "CoreVerify.h"


/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

static DALSYS_PROPERTY_HANDLE_DECLARE(hProp_common);
static DALSYS_PROPERTY_HANDLE_DECLARE(hProp_target);
static DALSYSPropertyVar prop_common;                /* Union to retreive property values */
static DALSYSPropertyVar prop_target;                /* Union to retreive property values */


void pm_target_information_init(void)
{
    /*
    * Runtime detection to support multiple PMIC configuration on any target.
    * We're only concerned with the primary PMIC here so we use index 0.
    */
    pm_err_flag_type                  err_flag             = PM_ERR_FLAG__SUCCESS;
    DALResult                         result               = DAL_SUCCESS;
    char                             *pmic_props           = NULL;
    DalDeviceHandle                  *phPlatformInfoDevice = NULL;
    DalChipInfoFamilyType             ChipInfoFamily       = CHIPINFO_FAMILY_UNKNOWN;
    DalPlatformInfoPlatformInfoType   platformInfo         = {DALPLATFORMINFO_TYPE_UNKNOWN};
    
    /* Get the chip family */
    ChipInfoFamily = DalChipInfo_ChipFamily();
    /* Get the platform info */
    result = DAL_PlatformInfoDeviceAttach(DALDEVICEID_PLATFORMINFO, &phPlatformInfoDevice);
    if (result != DAL_SUCCESS)
    {
        err_flag = PM_ERR_FLAG__PMIC_NOT_SUPPORTED; 
        PM_LOG_MSG_ERR_FATAL("Failed to get the platform handle info - %d", err_flag);
    }
    result = DalPlatformInfo_GetPlatformInfo(phPlatformInfoDevice, &platformInfo);
    if (result != DAL_SUCCESS)
    {
        err_flag = PM_ERR_FLAG__PMIC_NOT_SUPPORTED; 
        PM_LOG_MSG_ERR_FATAL("Failed to get the platform info - %d", err_flag);    
    }

    if(CHIPINFO_FAMILY_SDM845 == ChipInfoFamily)
    {
        pmic_props = "/sdm845/pmic";
    }
    else if (CHIPINFO_FAMILY_SDM670 == ChipInfoFamily)
    {
        pmic_props = "/sdm670/pmic";
    }
    else if ((CHIPINFO_FAMILY_QCS605 == ChipInfoFamily) || (CHIPINFO_FAMILY_SXR1130 == ChipInfoFamily))
    {
        if (((DALPLATFORMINFO_TYPE_MTP == platformInfo.platform) && (0x04 == platformInfo.subtype)) ||
            ((DALPLATFORMINFO_TYPE_MTP == platformInfo.platform) && (0x06 == platformInfo.subtype)) ||
            ((DALPLATFORMINFO_TYPE_MTP == platformInfo.platform) && (0x08 == platformInfo.subtype)) ||            
            ((DALPLATFORMINFO_TYPE_MTP == platformInfo.platform) && (0x09 == platformInfo.subtype)))
        {
            pmic_props = "/qcs605/pmic";           // For RedDragon LC platform
        }
        else
        {
            pmic_props = "/sdm670/pmic";           // for default RedDragon HP platform - same as warlock.
        }
    }
    else
    {
       pmic_props = NULL; 
    }

    
    if(NULL == pmic_props)
    {
        /* 
        * ERR Fatal here as we were unable to discover the PMIC.  No device ID for PMIC -
        * just use 0.
        */
        DALSYS_LogEvent (0, DALSYS_LOGEVENT_FATAL_ERROR,
        "PMIC:  Initialization failed to identify PMIC device! ");
        return;
    }
    
    if(DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr((const char*)pmic_props, hProp_target))
    {
        DALSYS_LogEvent (0, DALSYS_LOGEVENT_FATAL_ERROR,
        "PMIC:  Initialization failed hProp_target handle retrieval!");
    }
    
}


void* pm_target_information_get_common_info(const char* prop_id)
{
  void *info = NULL;

  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hProp_common, prop_id, NULL, &prop_common))
  {
      info = (void*)prop_common.Val.pStruct;
  }

  return info;
}

void* pm_target_information_get_specific_info(const char* prop_id)
{
  void *info = NULL;

  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hProp_target, prop_id, NULL, &prop_target))
  {
      info = (void*)prop_target.Val.pStruct;
  }

  return info;
}

uint32 pm_target_information_get_periph_count_info(const char* prop_id , uint8 pmic_index)
{
  uint32 *peripheral_count = NULL;

  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hProp_target, prop_id, NULL, &prop_target))
  {
      peripheral_count = (uint32*)prop_target.Val.pStruct;
      return peripheral_count[pmic_index];
  }

  return 0;
}
