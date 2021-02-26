/*! \file pm_app_pon.c
*  
*  \brief Implementation file for PON APP level APIs.
*    
*  &copy; Copyright 2014-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/*===================================================================
EDIT HISTORY FOR MODULE
 This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Date: 2017/09/18 $ 
$Change: 14402555 $
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/22/17   czq     Updated pm_app_pon_pshold_cfg() to support DVDD hard reset type.
05/05/16   aab     Updated pm_app_pon_reset_cfg() to support GP1 Configuation 
04/29/16   aab     Added pm_app_pon_reset_init(), used for making any needed pon related init
03/28/16   aab     Updated pm_app_pon_pshold_cfg() to support MSM8998 target
01/22/16   aab     Added pm_log_pon_gen2_reasons()
12/04/15   aab     Updated pm_log_pon_reasons() 
11/11/15   aab     Added back pm_check_pbs_ram_warm_reset_seq_presence() 
10/05/15   aab     Removed Updated pmapp_ps_hold_cfg(). Added pm_app_ps_hold_cfg(), pm_app_pon_reset_cfg() and pm_app_pon_s3_reset_timer_cfg()
09/25/15   aab     Added pm_check_pbs_ram_warm_reset_seq_presence()
08/08/15   aab     Updated pmapp_ps_hold_cfg() to support PON APIs with out doing pmic init
05/31/15   aab     Adding pm_app_log_pon_reasons() and pm_app_log_verbose_pon_reason_status()
04/14/15   al      Adding Hawker support
04/02/15   aab     Updated pmapp_ps_hold_cfg(), 
                    - configure PM8994/PMI8994 individualy instead of using broadcasting command to all 3 PMICs 
01/06/15   kt      Added pmapp_ps_hold_cfg API for multi-pmic config
07/10/14   akm     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pon.h"
#include "pm_app_pon.h"
#include "pmio_pon.h"
#include "pm_err_flags.h"
#include "device_info.h"
#include "pm_pon_target.h"
#include "pm_target_information.h"
#include "CoreVerify.h"
#include "pm_device.h"


/*===========================================================================

                     Global Variables

===========================================================================*/
#define PM_MIN_SEC_PMIC_S3_TIMER 64
#define PON_SPARE_BIT_7 0x80

/*===========================================================================

                   API Implementation

===========================================================================*/
pm_err_flag_type
pm_app_pon_pshold_cfg(pm_app_pon_reset_cfg_type app_pshold_cfg) 
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   pm_model_type pmic_model  = PMIC_IS_INVALID;
   uint32  pmic_index        = 0;
   pm_pon_reset_cfg_type    pshold_cfg = PM_PON_RESET_CFG_INVALID;


   if (app_pshold_cfg >= PM_APP_PON_CFG_MAX) 
   {
      return PM_ERR_FLAG_INVALID_PARAMETER;
   }

   if (app_pshold_cfg == PM_APP_PON_CFG_DVDD_SHUTDOWN) 
   {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;//SHUTDOWN configs are not supported at this time
   }

   err_flag |= pm_device_setup();
   if (err_flag != PM_ERR_FLAG_SUCCESS) 
   {
      return PM_ERR_FLAG_INVALID_PERIPHERAL;
   }


   while (pmic_index < PM_MAX_NUM_PMICS) 
   {
      pmic_model = pm_get_pmic_model(pmic_index);

      if ((pmic_model != PMIC_IS_INVALID) && (pmic_model != PMIC_IS_UNKNOWN)) 
      {
         //Get PMIC specific PSHOLD configuration
         err_flag |= pm_tgt_get_pshold_reset_cfg(pmic_index, app_pshold_cfg, &pshold_cfg); 
         if (err_flag != PM_ERR_FLAG_SUCCESS) 
         {
            break;
         }

         //PSHOLD Reset Config
         if (pshold_cfg != PON_NO_CFG_NEEDED) 
         { 
            err_flag |= pm_pon_ps_hold_cfg(pmic_index, pshold_cfg);
         }

         //Target specific reset config 
         err_flag |= pm_pon_tgt_specific_pshold_cfg(pmic_index, app_pshold_cfg);

      }
      pmic_index++;
   }

   return err_flag;
} 




pm_err_flag_type 
pm_app_pon_reset_cfg( pm_app_pon_reset_source_type  app_pon_reset_source, 
                      pm_app_pon_reset_cfg_type     app_pon_reset_cfg, 
                      uint32 s1_timer, 
                      uint32 s2_timer )
{
    pm_err_flag_type err_flag  = PM_ERR_FLAG_SUCCESS;
    pm_model_type pmic_model   = PMIC_IS_INVALID;
    uint32 pmic_index          = 0;
    pm_pon_reset_source_type    pon_reset_source  = PM_PON_RESET_SOURCE_INVALID;
    pm_pon_reset_cfg_type       kpdpwr_reset_cfg  = PM_PON_RESET_CFG_INVALID;
     

    if(app_pon_reset_source >= PM_APP_PON_RESET_SOURCE_MAX) 
    {
       return PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else if (app_pon_reset_source == PM_APP_PON_RESET_SOURCE_RESIN_OR_KPDPWR) 
    {
       return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }


    if(app_pon_reset_cfg >= PM_APP_PON_CFG_MAX)
    {
       return PM_ERR_FLAG_INVALID_PARAMETER;
    } 
    else if( (app_pon_reset_cfg == PM_APP_PON_CFG_HARD_RESET) ||  
             (app_pon_reset_cfg == PM_APP_PON_CFG_NORMAL_SHUTDOWN) )
    {
       return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;//HARD_RESET / SHUTDOWN configs are not supported at this time
    }


    err_flag |= pm_device_setup();
    if (err_flag != PM_ERR_FLAG_SUCCESS) 
    {
       return PM_ERR_FLAG_INVALID_PERIPHERAL;
    }

    //Map App level allowed PON reset source enum to PON source enum
    switch(app_pon_reset_source)
    {
       case PM_APP_PON_RESET_SOURCE_KPDPWR:
       {
          pon_reset_source = PM_PON_RESET_SOURCE_KPDPWR;
       }
       break;
       case PM_APP_PON_RESET_SOURCE_RESIN:
       {
          pon_reset_source = PM_PON_RESET_SOURCE_RESIN;
       }
       break;
       case PM_APP_PON_RESET_SOURCE_RESIN_AND_KPDPWR:
       {
          pon_reset_source = PM_PON_RESET_SOURCE_RESIN_AND_KPDPWR;
       }
       break;
       default:
          return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    while (pmic_index < PM_MAX_NUM_PMICS)
    {
       pmic_model = pm_get_pmic_model(pmic_index);

       if ((pmic_model != PMIC_IS_INVALID) && (pmic_model != PMIC_IS_UNKNOWN))
       {
          //Get individual PMIC specific configuration
          err_flag |= pm_tgt_get_kpdpwr_reset_cfg(pmic_index, app_pon_reset_cfg, &kpdpwr_reset_cfg);
          if (err_flag != PM_ERR_FLAG_SUCCESS) 
          {
             break;
          }

         //KpadPower/Resin/Key Combo:  Reset Config
         if (kpdpwr_reset_cfg != PON_NO_CFG_NEEDED) 
         { 
            err_flag |= pm_pon_reset_source_cfg(  pmic_index, 
                                                   pon_reset_source,
                                                   s1_timer,   
                                                   s2_timer,   
                                                   kpdpwr_reset_cfg   
                                                 );
         }

         //Target specific reset config 
         err_flag |= pm_pon_tgt_specific_kpd_reset_cfg(pmic_index, app_pon_reset_cfg);
       }
       pmic_index++;
    }

    return err_flag;
} 





pm_err_flag_type 
pm_app_pon_s3_reset_timer_cfg( pm_app_pon_reset_source_type  app_pon_s3_reset_source, uint32 s3_timer )
{
    pm_err_flag_type err_flag  = PM_ERR_FLAG_SUCCESS;
    boolean skip_reset_config  = FALSE;
    pm_model_type pmic_model   = PMIC_IS_INVALID;
    uint32 pmic_index          = 0;
    pm_pon_reset_source_type    pon_s3_reset_source = PM_PON_RESET_SOURCE_INVALID;
    uint32                      pm_s3_reset_timer_multiplier;
    uint32                      pm_s3_timer;

    if(app_pon_s3_reset_source >= PM_APP_PON_RESET_SOURCE_MAX)
    {
       return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    err_flag |= pm_device_setup();
    if (err_flag != PM_ERR_FLAG_SUCCESS) 
    {
       return PM_ERR_FLAG_INVALID_PERIPHERAL;
    }

    //Map App level allowed PON reset source enum
    switch(app_pon_s3_reset_source)
    {
       case PM_APP_PON_RESET_SOURCE_KPDPWR:
       {
          pon_s3_reset_source = PM_PON_RESET_SOURCE_KPDPWR;
       }
       break;
       case PM_APP_PON_RESET_SOURCE_RESIN:
       {
          pon_s3_reset_source = PM_PON_RESET_SOURCE_RESIN;
       }
       break;
       case PM_APP_PON_RESET_SOURCE_RESIN_AND_KPDPWR:
       {
          pon_s3_reset_source = PM_PON_RESET_SOURCE_RESIN_AND_KPDPWR;
       }
       break;
       case PM_APP_PON_RESET_SOURCE_RESIN_OR_KPDPWR:
       {
          pon_s3_reset_source = PM_PON_RESET_SOURCE_RESIN_OR_KPDPWR;
       }
       break;
       default:
          return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }


    while (pmic_index < PM_MAX_NUM_PMICS)
    {
       pmic_model = pm_get_pmic_model(pmic_index);
       skip_reset_config = FALSE;

       if ((pmic_model != PMIC_IS_INVALID) && (pmic_model != PMIC_IS_UNKNOWN))
       {
          //Get individual PMIC specific configuration
          err_flag |= pm_tgt_get_pon_s3_reset_timer_multiplier( pmic_index, app_pon_s3_reset_source, &pm_s3_reset_timer_multiplier);
          if (err_flag != PM_ERR_FLAG_SUCCESS) 
          {
             break;
          }

          if (pm_s3_reset_timer_multiplier != 0)
          {//Configure S3 reset timer 
             pm_s3_timer = s3_timer * pm_s3_reset_timer_multiplier;

              //S3 timer value for all Secondary PMICs should be at least 64 sec
              if( (pmic_index > 0) && (pm_s3_timer < PM_MIN_SEC_PMIC_S3_TIMER))
              {
                  pm_s3_timer = PM_MIN_SEC_PMIC_S3_TIMER;
              }
              err_flag |= pm_pon_stage3_reset_source_cfg(pmic_index, pon_s3_reset_source, pm_s3_timer);
          }
       }
       pmic_index++;
    }

    return err_flag;
}




pm_err_flag_type 
pm_app_pon_reset_init (void)
{
   pm_err_flag_type err_flag   = PM_ERR_FLAG_SUCCESS;
   static pm_sbl_pon_specific_data_type *pon_param_ptr = NULL;
   pm_app_pon_reset_source_type app_reset_source = PM_APP_PON_RESET_SOURCE_MAX;
   uint32 s3_timer_value = 16; //16sec is HW default
   
   //S3 Timer Configuration
   pon_param_ptr = (pm_sbl_pon_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_SBL_PON_SPECIFIC_DATA);
   CORE_VERIFY_PTR(pon_param_ptr);

   s3_timer_value   = pon_param_ptr->s3_reset_timer_cfg.timer_value;
   app_reset_source = pon_param_ptr->s3_reset_timer_cfg.reset_source_type;
   if (app_reset_source >= PM_APP_PON_RESET_SOURCE_MAX )
   {
        return PM_ERR_FLAG_INVALID_PARAMETER;
   }

   // If configuration enabled, set S3 Timer to desired value for each reset source
   if (pon_param_ptr->s3_reset_timer_cfg.enable_config == PM_ENABLE_CONFIG) 
   {
      err_flag |= pm_app_pon_s3_reset_timer_cfg( app_reset_source, s3_timer_value );
   }

   return err_flag;
}


pm_err_flag_type 
pm_pon_uvlo_reset_status (uint32 pmic_device_index,boolean * status)
{
    pm_err_flag_type err_flag   = PM_ERR_FLAG_SUCCESS;
    uint8 pmic_spare_bit        = 0;
    uint8 warm_reset_reason_bit = 0;
    pm_pon_poff_reason_type poff_reason;
    pm_pon_warm_reset_reason_type warm_reset_reason;
    uint8 reg_data;
	
    pm_pon_get_poff_reason(pmic_device_index, &poff_reason);
    pm_pon_get_warm_reset_reason(pmic_device_index,&warm_reset_reason);
    pm_pon_get_spare_reg (pmic_device_index,PM_PON_XVDD_SPARE ,&reg_data); 


    /*'pmic_spare_bit' bit<7> of XVDD_RB_SPARE reg is used as a flag for UVLO reset*/
	
    if( PON_SPARE_BIT_7 == reg_data)
    {
       pmic_spare_bit = 1;
    }

    /* Check if there is any warm reset reason bit is set*/
    warm_reset_reason_bit = warm_reset_reason.gp1 || warm_reset_reason.gp2 || 
                            warm_reset_reason.kpdpwr || warm_reset_reason.kpdpwr_and_resin || 
                            warm_reset_reason.pmic_wd || warm_reset_reason.ps_hold || 
                            warm_reset_reason.reserved || warm_reset_reason.reserved1 || 
                            warm_reset_reason.resin || warm_reset_reason.soft || 
                            warm_reset_reason.tft ;  

    /* If poff reason is UVLO and no warm_reset_reason and XVDD_SPARE_REG bit<7> is set, then return 
           UVLO reset status as TRUE */
    if (((poff_reason.gp1 ==1 ) || (poff_reason.uvlo == 1)) && (warm_reset_reason_bit == 0) && (pmic_spare_bit == 1))
    {
     *status = TRUE;
    }
    else
    {
     *status = FALSE;
    }

    return err_flag;
}


