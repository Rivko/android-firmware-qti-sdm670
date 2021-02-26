/*! \file
*  
*  \brief  pm_utils_loader.c 
*  &copy; Copyright 2016-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/15/17   aab      Added pm_sbl_set_batt_id_forced_mode()
02/02/17   cs       SBL needs to do a fresh reading of batt id
08/03/16   akm      Created

===========================================================================*/

/*===========================================================================

INCLUDE FILES 

===========================================================================*/

#include "DALSys.h"
#include "DDIAdc.h"
#include "busywait.h"
#include "boothw_target.h"

#include "pm_app_pon.h"
#include "pm_utils.h"
#include "pm_log_utils.h"

pm_err_flag_type
pm_busy_wait ( uint32 uS )
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  if ( uS > 0 )
  {  
     (void) DALSYS_BusyWait(uS);
  }
  else
  {
     err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }

  return err_flag;
}



pm_err_flag_type 
pm_sbl_get_batt_id(uint32 *batt_id_ohms)
{
   pm_err_flag_type err_flag         = PM_ERR_FLAG_SUCCESS;
   DALResult result                  = DAL_SUCCESS;
   static DALBOOL bInit              = FALSE;
   static DalDeviceHandle *phAdcDev  = NULL;
   static AdcInputPropertiesType adcInputProps;
   AdcResultType adcResult;

   if (!bInit)
   {
      /* Attach to ADC */
      result = DAL_AdcDeviceAttach(DALDEVICEID_ADC, &phAdcDev);
      if (result != DAL_SUCCESS)
      {
         pm_log_message("ERROR: DAL_AdcDeviceAttach()");
         return err_flag = PM_ERR_FLAG_PERIPHERAL_ERR;
      }
 
      /* Get the channel's input properties */
      result = DalAdc_GetAdcInputProperties(phAdcDev,
                                            ADC_INPUT_BATT_ID_OHMS,
                                            sizeof(ADC_INPUT_BATT_ID_OHMS),
                                            &adcInputProps);
      if (result != DAL_SUCCESS)
      {
         pm_log_message("ERROR: DalAdc_GetAdcInputProperties()");
         return err_flag = PM_ERR_FLAG_PERIPHERAL_ERR;
      }
 
      bInit = TRUE;
   }
 
   /* Read the ADC channel */
   result = DalAdc_Read(phAdcDev, &adcInputProps, &adcResult);
   if (result != DAL_SUCCESS)
   {
      pm_log_message("ERROR: DalAdc_Read()");
      return err_flag = PM_ERR_FLAG_PERIPHERAL_ERR;
   }
 
   if ((adcResult.eStatus == ADC_RESULT_VALID) || (adcResult.eStatus == ADC_RESULT_STALE) )
   {
      *batt_id_ohms = adcResult.nPhysical;      
   }
   else
   {
      pm_log_message("ERROR: Invalid adcResult.eStatus"); //ADC Reading will NOT be valid if BATT_ID is not connected(S4-2)
      *batt_id_ohms = 0;
   }
   
   return err_flag;
}

pm_err_flag_type 
pm_device_shutdown(void)
{
  boot_hw_powerdown();   
  
  /*should not return*/
  return PM_ERR_FLAG_SUCCESS;
}

  
pm_err_flag_type  
pm_sbl_daladc_fg_set_temp_threshold(uint8 pmic_index,  AdcFGThresholdType adc_fg_threshold, int temp_value) 
{ 
   pm_err_flag_type err_flag         = PM_ERR_FLAG_SUCCESS; 
   DALResult result                  = DAL_SUCCESS; 
   static DALBOOL bInit              = FALSE; 
   static DalDeviceHandle *phAdcDev  = NULL; 
  
   if (!bInit) 
   { 
      /* Attach to ADC */ 
      result = DAL_AdcDeviceAttach(DALDEVICEID_ADC, &phAdcDev); 
      if (result != DAL_SUCCESS) 
      { 
         pm_log_message("ERROR: DAL_AdcDeviceAttach()"); 
         return err_flag = PM_ERR_FLAG_PERIPHERAL_ERR; 
      } 
  
      bInit = TRUE; 
   } 
  
   //Configure charger / skin temp threshold 
   result = DalAdc_FGSetThreshold(phAdcDev, adc_fg_threshold, temp_value); 
   if (result != DAL_SUCCESS) 
   { 
      pm_log_message("ERROR: DalAdc_FGSetThreshold()"); 
      err_flag = PM_ERR_FLAG_PERIPHERAL_ERR; 
   } 
  
   return err_flag; 
}




pm_err_flag_type 
pm_sbl_set_batt_id_forced_mode(boolean enable)
{
   pm_err_flag_type err_flag         = PM_ERR_FLAG_SUCCESS;
   DALResult result                  = DAL_SUCCESS;
   static DALBOOL bInit              = FALSE;
   static DalDeviceHandle *phAdcDev  = NULL;

   if (!bInit)
   {
      /* Attach to ADC */
      result = DAL_AdcDeviceAttach(DALDEVICEID_ADC, &phAdcDev);
      if (result != DAL_SUCCESS)
      {
         pm_log_message("ERROR: DAL_AdcDeviceAttach()");
         return err_flag = PM_ERR_FLAG_PERIPHERAL_ERR;
      }
	  bInit = TRUE;
   }
   
   /* Configure bat_id forced mode */
   result = DalAdc_FGSetBattIdForced(phAdcDev, enable);
   if (result != DAL_SUCCESS)
   {
      pm_log_message("ERROR: DalAdc_FGSetBattIdForced()");
      return err_flag = PM_ERR_FLAG_PERIPHERAL_ERR;
   }
 
   return err_flag;
}
