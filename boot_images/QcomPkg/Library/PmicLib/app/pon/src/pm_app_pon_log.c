/*! \file pm_app_pon_log.c
*  
*  \brief Implementation file for PON APP level logger APIs.
*    
*  &copy; Copyright 2016-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/*===================================================================
EDIT HISTORY FOR MODULE
 This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Date: 2017/09/18 $ 
$Change: 14402555 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/10/17   aab     Updated PON logging Text
09/13/16   aab     Updated PON logging
08/03/16   akm     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pon.h"
#include "pm_app_pon.h"
#include "pmio_pon.h"
#include "pm_err_flags.h"
#include "device_info.h"
#include "hw_module_type.h"
#include "pm_config_target.h"
#include "pm_sbl_boot_target.h"
#include "pm_pon_target.h"
#include "pm_target_information.h"
#include "CoreVerify.h"
#include "pm_log_utils.h"
#include <string.h> 


/*===========================================================================

                     Global Variables

===========================================================================*/
typedef struct
{
    uint8 pon_reason1;
    uint8 pon_reason2;
    uint8 poff_reason1;
    uint8 poff_reason2;
    uint8 warm_reset_reason1;
    uint8 warm_reset_reason2;
    uint8 soft_reset_reason1;
    uint8 soft_reset_reason2;
}pm_reason_status_type;

typedef struct
{
    uint8 pon_reason1;
    uint8 reserved1;
    uint8 warm_reset_reason1;
    uint8 reserved2;
    uint8 on_reason1;
    uint8 poff_reason1;
    uint8 reserved3;
    uint8 off_reason1;
}pm_pon_gen2_reason_status_type;
 

 
typedef struct
{
    uint8 fault_reason1;
    uint8 fault_reason2;
    uint8 s3_reset_reason;
    uint8 soft_reset_reason1;
}pm_pon_gen2_fault_status_type;
 
 static char temp_str[PM_STATUS_MSG_LEN] = "";

/*===========================================================================

                   Function Prototypes

===========================================================================*/
static pm_err_flag_type pm_app_log_verbose_pon_reason_status(uint8 pmic_index, uint64 pon_reasons);
static pm_err_flag_type pm_app_log_verbose_pon_gen2_reason_status(uint8 pmic_index, uint64 pon_reasons, uint64 fault_reasons);



/*===========================================================================

                   API Implementation

===========================================================================*/
pm_err_flag_type 
pm_log_pon_reasons(boolean pon_verbose_log_mode)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   pm_model_type pmic_model = PMIC_IS_INVALID;
   uint8 pmic_index = 0;
   uint8 sid_index      = 0;
   uint64 pon_reasons   = 0x00;
   uint64 fault_reasons = 0x00;
   static char temp_str1[PM_STATUS_MSG_LEN] = "";

   uint32  pon_peripheral_index = 8;
   peripheral_info_type  pon_peripheral_info;
   memset(&pon_peripheral_info, 0 , sizeof(peripheral_info_type));
   pon_peripheral_info.base_address = pon_peripheral_index * 0x0100;

   strlcat(temp_str,"PON REASON: ", sizeof(temp_str));

   //Log PON reasons for all PMICs
   for (pmic_index = 0; pmic_index < PM_MAX_NUM_PMICS; pmic_index++)
   {
      pmic_model = pm_get_pmic_model(pmic_index);
      if ((pmic_model != PMIC_IS_INVALID) && (pmic_model != PMIC_IS_UNKNOWN) )
      {
         pon_reasons = 0x00;
         err_flag = pm_pon_get_all_pon_reasons(pmic_index, &pon_reasons);

        //Get PON perepheral type and perepheral sub type for each valid PMIC on target
         sid_index = pmic_index * 2;
         pm_get_peripheral_info(sid_index, &pon_peripheral_info);

         snprintf(temp_str1, PM_STATUS_MSG_LEN, "PM%d", (uint8)pmic_index);
         strlcat(temp_str,temp_str1, sizeof(temp_str1));

         if ( pon_peripheral_info.peripheral_subtype  >= PM_HW_MODULE_PON_LV_PON_GEN2_PRIMARY)
         {//PON GEN2
             fault_reasons = 0x00;
             err_flag = pm_pon_get_fault_reasons(pmic_index, &fault_reasons);
             if (pon_verbose_log_mode == FALSE)
             {
                snprintf(temp_str1, PM_STATUS_MSG_LEN, "=0x%llx:0x%llx ",(uint64)pon_reasons, (uint64)fault_reasons);
                strlcat(temp_str,temp_str1, sizeof(temp_str1));
             }
             else
             { 
                err_flag |= pm_app_log_verbose_pon_gen2_reason_status(pmic_index, pon_reasons, fault_reasons);
             }
         }
         else
         { //PON GEN1
             if (pon_verbose_log_mode == FALSE)
             {
                snprintf(temp_str1, PM_STATUS_MSG_LEN, "=0x%llx ",(uint64)pon_reasons);
                strlcat(temp_str,temp_str1, sizeof(temp_str1));
             }
             else
             { 
               err_flag |= pm_app_log_verbose_pon_reason_status(pmic_index, pon_reasons);
             }
         }
      }
   }//for

   if (pon_verbose_log_mode == FALSE)
   {
      pm_log_message(temp_str);
   }

   return err_flag;
}




pm_err_flag_type 
pm_app_log_verbose_pon_reason_status(uint8 pmic_index, uint64 pon_reasons)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   pm_reason_status_type *reasons_ptr = (pm_reason_status_type*)(&pon_reasons);
   static char temp_str1[PM_STATUS_MSG_LEN] = "";

   snprintf(temp_str, PM_STATUS_MSG_LEN, "%d", (uint8)pmic_index);

   //PON RESEN1
   if ((uintnt)(reasons_ptr->pon_reason1) != 0x00) 
   {
       snprintf(temp_str1, PM_STATUS_MSG_LEN, ": PON RESEN1=0x%lx", (uintnt)reasons_ptr->pon_reason1);
       strlcat(temp_str,temp_str1, sizeof(temp_str1));

       if ((uintnt)(reasons_ptr->pon_reason1) & PMIO_PON_PON_REASON1_KPDPWR_N_BMSK) 
       {
           strlcat(temp_str,":KPDPWR_N", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->pon_reason1) & PMIO_PON_PON_REASON1_CBLPWR_N_BMSK)
       {
           strlcat(temp_str,":CBLPWR_N", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->pon_reason1) & PMIO_PON_PON_REASON1_PON1_BMSK)
       {
           strlcat(temp_str,":PON1", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->pon_reason1) & PMIO_PON_PON_REASON1_USB_CHG_BMSK)
       {
           strlcat(temp_str,":USB_CHG", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->pon_reason1) & PMIO_PON_PON_REASON1_DC_CHG_BMSK)
       {
           strlcat(temp_str,":DC_CHG", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->pon_reason1) & PMIO_PON_PON_REASON1_RTC_BMSK)
       {
           strlcat(temp_str,":RTC", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->pon_reason1) & PMIO_PON_PON_REASON1_SMPL_BMSK)
       {
           strlcat(temp_str,":SMPL", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->pon_reason1) & PMIO_PON_PON_REASON1_HARD_RESET_BMSK)
       {
           strlcat(temp_str,":HARD_RESET", sizeof(temp_str));
       }
   }


   //PON RESEN2 
   if ((uintnt)(reasons_ptr->pon_reason2) != 0x00) 
   {
      snprintf(temp_str1, PM_STATUS_MSG_LEN, ": PON RESEN2=0x%lx", (uintnt)reasons_ptr->pon_reason2);
      strlcat(temp_str,temp_str1, sizeof(temp_str1));
   }


   //POFF RESEN1
   if ((uintnt)(reasons_ptr->poff_reason1) != 0x00) 
   {
       snprintf(temp_str1, PM_STATUS_MSG_LEN, ": POFF RESEN1=0x%lx", (uintnt)reasons_ptr->poff_reason1);
       strlcat(temp_str,temp_str1, sizeof(temp_str1));

       if ((uintnt)(reasons_ptr->poff_reason1) & PMIO_PON_POFF_REASON1_KPDPWR_N_BMSK) 
       {
           strlcat(temp_str,":KPDPWR_N", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason1) & PMIO_PON_POFF_REASON1_RESIN_N_BMSK)
       {
           strlcat(temp_str,":RESIN_N", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason1) & PMIO_PON_POFF_REASON1_KPDPWR_AND_RESIN_BMSK)
       {
           strlcat(temp_str,":KPDPWR_AND_RESIN", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason1) & PMIO_PON_POFF_REASON1_GP2_BMSK)
       {
           strlcat(temp_str,":GP2", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason1) & PMIO_PON_POFF_REASON1_GP1_BMSK)
       {
           strlcat(temp_str,":GP1", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason1) & PMIO_PON_POFF_REASON1_PMIC_WD_BMSK)
       {
           strlcat(temp_str,":PMIC_WD", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason1) & PMIO_PON_POFF_REASON1_PS_HOLD_BMSK)
       {
           strlcat(temp_str,":PS_HOLD", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason1) & PMIO_PON_POFF_REASON1_SOFT_BMSK)
       {
           strlcat(temp_str,":SOFT", sizeof(temp_str));
       }
   }


   //POFF RESEN2
   if ((uintnt)(reasons_ptr->poff_reason2) != 0x00) 
   {
       snprintf(temp_str1, PM_STATUS_MSG_LEN, ": POFF RESEN2=0x%lx", (uintnt)reasons_ptr->poff_reason2);
       strlcat(temp_str,temp_str1, sizeof(temp_str1));

       if ((uintnt)(reasons_ptr->poff_reason2) & PMIO_PON_POFF_REASON2_STAGE3_BMSK) 
       {
           strlcat(temp_str,":STAGE3", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason2) & PMIO_PON_POFF_REASON2_OTST3_BMSK)
       {
           strlcat(temp_str,":OTST3", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason2) & PMIO_PON_POFF_REASON2_STAGE3_BMSK)
       {
           strlcat(temp_str,":STAGE3", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason2) & PMIO_PON_POFF_REASON2_UVLO_BMSK)
       {
           strlcat(temp_str,":UVLO", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason2) & PMIO_PON_POFF_REASON2_TFT_BMSK)
       {
           strlcat(temp_str,":TFT", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason2) & PMIO_PON_POFF_REASON2_CHARGER_BMSK)
       {
           strlcat(temp_str,":CHARGER", sizeof(temp_str));
       }
   }


   //WARM RESET RESEN1
   if ((uintnt)(reasons_ptr->warm_reset_reason1) != 0x00) 
   {
       snprintf(temp_str1, PM_STATUS_MSG_LEN, ": WARM RST RESEN1=0x%lx", (uintnt)reasons_ptr->warm_reset_reason1);
       strlcat(temp_str,temp_str1, sizeof(temp_str1));

       if ((uintnt)(reasons_ptr->warm_reset_reason1) & PMIO_PON_WARM_RESET_REASON1_KPDPWR_N_BMSK) 
       {
           strlcat(temp_str,":KPDPWR_N", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->warm_reset_reason1) & PMIO_PON_WARM_RESET_REASON1_RESIN_N_BMSK)
       {
           strlcat(temp_str,":RESIN_N", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->warm_reset_reason1) & PMIO_PON_WARM_RESET_REASON1_KPDPWR_AND_RESIN_BMSK)
       {
           strlcat(temp_str,":KPDPWR_AND_RESIN", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->warm_reset_reason1) & PMIO_PON_WARM_RESET_REASON1_GP2_BMSK)
       {
           strlcat(temp_str,":GP2", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->warm_reset_reason1) & PMIO_PON_WARM_RESET_REASON1_GP1_BMSK)
       {
           strlcat(temp_str,":GP1", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->warm_reset_reason1) & PMIO_PON_WARM_RESET_REASON1_PMIC_WD_BMSK)
       {
           strlcat(temp_str,":PMIC_WD", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->warm_reset_reason1) & PMIO_PON_WARM_RESET_REASON1_PS_HOLD_BMSK)
       {
           strlcat(temp_str,":PS_HOLD", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->warm_reset_reason1) & PMIO_PON_WARM_RESET_REASON1_SOFT_BMSK)
       {
           strlcat(temp_str,":SOFT", sizeof(temp_str));
       }
   }


   //WARM RESET RESEN2
   if((uintnt)(reasons_ptr->warm_reset_reason2) & PMIO_PON_WARM_RESET_REASON2_TFT_BMSK)
   {
      snprintf(temp_str1, PM_STATUS_MSG_LEN, ": WARM RST RESEN2=0x%lx", (uintnt)reasons_ptr->warm_reset_reason2);
      strlcat(temp_str,temp_str1, sizeof(temp_str1));
      strlcat(temp_str,":TFT", sizeof(temp_str));
   }


   //SOFT RESET RESEN1
   if ((uintnt)(reasons_ptr->soft_reset_reason1) != 0x00) 
   {
       snprintf(temp_str1, PM_STATUS_MSG_LEN, " SOFT RST RESEN1=0x%lx", (uintnt)reasons_ptr->soft_reset_reason1);
       strlcat(temp_str,temp_str1, sizeof(temp_str1));

       if ((uintnt)(reasons_ptr->soft_reset_reason1) & PMIO_PON_SOFT_RESET_REASON1_KPDPWR_N_BMSK) 
       {
           strlcat(temp_str,":KPDPWR_N", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->soft_reset_reason1) & PMIO_PON_SOFT_RESET_REASON1_RESIN_N_BMSK)
       {
           strlcat(temp_str,":RESIN_N", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->soft_reset_reason1) & PMIO_PON_SOFT_RESET_REASON1_KPDPWR_AND_RESIN_BMSK)
       {
           strlcat(temp_str,":KPDPWR_AND_RESIN", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->soft_reset_reason1) & PMIO_PON_SOFT_RESET_REASON1_GP2_BMSK)
       {
           strlcat(temp_str,":GP2", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->soft_reset_reason1) & PMIO_PON_SOFT_RESET_REASON1_GP1_BMSK)
       {
           strlcat(temp_str,":GP1", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->soft_reset_reason1) & PMIO_PON_SOFT_RESET_REASON1_PMIC_WD_BMSK)
       {
           strlcat(temp_str,":PMIC_WD", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->soft_reset_reason1) & PMIO_PON_SOFT_RESET_REASON1_PS_HOLD_BMSK)
       {
           strlcat(temp_str,":PS_HOLD", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->soft_reset_reason1) & PMIO_PON_SOFT_RESET_REASON1_SOFT_BMSK)
       {
           strlcat(temp_str,":SOFT", sizeof(temp_str));
       }
   }


   //SOFT RESET RESEN2
   if((uintnt)(reasons_ptr->soft_reset_reason1) & PMIO_PON_SOFT_RESET_REASON2_TFT_BMSK)
   {
       snprintf(temp_str1, PM_STATUS_MSG_LEN, " SOFT RST RESEN2=0x%lx", (uintnt)reasons_ptr->soft_reset_reason1);
       strlcat(temp_str,temp_str1, sizeof(temp_str1));
       strlcat(temp_str,":TFT", sizeof(temp_str));
   }
   pm_log_message(temp_str);

   return err_flag;
}



pm_err_flag_type 
pm_app_log_verbose_pon_gen2_reason_status(uint8 pmic_index, uint64 pon_reasons, uint64 fault_reasons)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   pm_pon_gen2_reason_status_type *reasons_ptr       = (pm_pon_gen2_reason_status_type*)(&pon_reasons);
   pm_pon_gen2_fault_status_type  *fault_reasons_ptr = (pm_pon_gen2_fault_status_type*) (&fault_reasons);
   static char temp_str1[PM_STATUS_MSG_LEN] = "";

   snprintf(temp_str, PM_STATUS_MSG_LEN, "%d", (uint8)pmic_index);

   //PON Reason1       
   if ((uintnt)(reasons_ptr->pon_reason1) != 0x00) 
   {
       snprintf(temp_str1, PM_STATUS_MSG_LEN, ": PON=0x%lx", (uintnt)reasons_ptr->pon_reason1);
       strlcat(temp_str,temp_str1, sizeof(temp_str1));

       if ((uintnt)(reasons_ptr->pon_reason1) & PMIO_PON_PON_REASON1_KPDPWR_N_BMSK) 
       {
           strlcat(temp_str,":KPDPWR_N", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->pon_reason1) & PMIO_PON_PON_REASON1_CBLPWR_N_BMSK)
       {
           strlcat(temp_str,":CBLPWR_N", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->pon_reason1) & PMIO_PON_PON_REASON1_PON1_BMSK)
       {
           strlcat(temp_str,":PON1", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->pon_reason1) & PMIO_PON_PON_REASON1_USB_CHG_BMSK)
       {
          strlcat(temp_str,":USB_CHG", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->pon_reason1) & PMIO_PON_PON_REASON1_DC_CHG_BMSK)
       {
           strlcat(temp_str,":DC_CHG", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->pon_reason1) & PMIO_PON_PON_REASON1_RTC_BMSK)
       {
           strlcat(temp_str,":RTC", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->pon_reason1) & PMIO_PON_PON_REASON1_SMPL_BMSK)
       {
           strlcat(temp_str,":SMPL", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->pon_reason1) & PMIO_PON_PON_REASON1_HARD_RESET_BMSK)
       {
           strlcat(temp_str,":HARD_RESET", sizeof(temp_str));
       }
   }


   //Warm Reset Reason1
   if ((uintnt)(reasons_ptr->warm_reset_reason1) != 0x00) 
   {
       snprintf(temp_str1, PM_STATUS_MSG_LEN, ": WARM RST=0x%lx", (uintnt)reasons_ptr->warm_reset_reason1);
       strlcat(temp_str,temp_str1, sizeof(temp_str1));

       if((uintnt)(reasons_ptr->warm_reset_reason1) & PMIO_PON_WARM_RESET_REASON1_KPDPWR_N_BMSK)
       {
           strlcat(temp_str,":KPDPWR_N", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->warm_reset_reason1) & PMIO_PON_WARM_RESET_REASON1_RESIN_N_BMSK)
       {
           strlcat(temp_str,":RESIN_N", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->warm_reset_reason1) & PMIO_PON_WARM_RESET_REASON1_KPDPWR_AND_RESIN_BMSK)
       {
           strlcat(temp_str,":KPDPWR_AND_RESIN", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->warm_reset_reason1) & PMIO_PON_WARM_RESET_REASON1_GP2_BMSK)
       {
           strlcat(temp_str,":GP2", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->warm_reset_reason1) & PMIO_PON_WARM_RESET_REASON1_GP1_BMSK)
       {
           strlcat(temp_str,":GP1", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->warm_reset_reason1) & PMIO_PON_WARM_RESET_REASON1_PMIC_WD_BMSK)
       {
           strlcat(temp_str,":PMIC_WD", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->warm_reset_reason1) & PMIO_PON_WARM_RESET_REASON1_PS_HOLD_BMSK)
       {
           strlcat(temp_str,":PS_HOLD", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->warm_reset_reason1) & PMIO_PON_WARM_RESET_REASON1_SOFT_BMSK)
       {
           strlcat(temp_str,":SOFT", sizeof(temp_str));
       }
   }

   //PON ON Reason1    
   if ((uintnt)(reasons_ptr->on_reason1) != 0x00) 
   {
       snprintf(temp_str1, PM_STATUS_MSG_LEN, ": ON=0x%lx", (uintnt)reasons_ptr->on_reason1);
       strlcat(temp_str,temp_str1, sizeof(temp_str1));
       if ((uintnt)(reasons_ptr->on_reason1) & PMIO_PON_GEN2_ON_REASON_PON_SEQ_BMSK)
       {
           strlcat(temp_str,":PON_SEQ", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->on_reason1) & PMIO_PON_GEN2_ON_REASON_WARM_SEQ_BMSK)
       {
           strlcat(temp_str,":WARM_SEQ", sizeof(temp_str));
       }
   }

   //PON POFF Reason1  
   if ((uintnt)(reasons_ptr->poff_reason1) != 0x00) 
   {
       snprintf(temp_str1, PM_STATUS_MSG_LEN, ": POFF=0x%lx", (uintnt)reasons_ptr->poff_reason1);
       strlcat(temp_str,temp_str1, sizeof(temp_str1));

       if ((uintnt)(reasons_ptr->poff_reason1) & PMIO_PON_GEN2_POFF_REASON1_KPDPWR_N_BMSK) 
       {
           strlcat(temp_str,":KPDPWR_N", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason1) & PMIO_PON_GEN2_POFF_REASON1_RESIN_N_BMSK)
       {
           strlcat(temp_str,":RESIN_N", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason1) & PMIO_PON_GEN2_POFF_REASON1_KPDPWR_AND_RESIN_BMSK)
       {
           strlcat(temp_str,":KPDPWR_AND_RESIN", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason1) & PMIO_PON_GEN2_POFF_REASON1_GP2_BMSK)
       {
           strlcat(temp_str,":GP2", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason1) & PMIO_PON_GEN2_POFF_REASON1_GP1_BMSK)
       {
           strlcat(temp_str,":GP1", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason1) & PMIO_PON_GEN2_POFF_REASON1_PMIC_WD_BMSK)
       {
           strlcat(temp_str,":PMIC_WD", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason1) & PMIO_PON_GEN2_POFF_REASON1_PS_HOLD_BMSK)
       {
           strlcat(temp_str,":PS_HOLD", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->poff_reason1) & PMIO_PON_GEN2_POFF_REASON1_SOFT_BMSK)
       {
           strlcat(temp_str,":SOFT", sizeof(temp_str));
       }
   }

   //PON OFF Reason1   
   if ((uintnt)(reasons_ptr->off_reason1) != 0x00) 
   {
       snprintf(temp_str1, PM_STATUS_MSG_LEN, ": OFF=0x%lx", (uintnt)reasons_ptr->off_reason1);
       strlcat(temp_str,temp_str1, sizeof(temp_str1));

       if ((uintnt)(reasons_ptr->off_reason1) & PMIO_PON_GEN2_OFF_REASON_POFF_SEQ_BMSK) 
       {
           strlcat(temp_str,":POFF_SEQ", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->off_reason1) & PMIO_PON_GEN2_OFF_REASON_FAULT_SEQ_BMSK)
       {
           strlcat(temp_str,":FAULT_SEQ", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->off_reason1) & PMIO_PON_GEN2_OFF_REASON_S3_RESET_BMSK)
       {
           strlcat(temp_str,":S3_RESET", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->off_reason1) & PMIO_PON_GEN2_OFF_REASON_IMM_XVDD_SD_BMSK)
       {
           strlcat(temp_str,":IMM_XVDD_SD", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->off_reason1) & PMIO_PON_GEN2_OFF_REASON_RAW_DVDD_SHD_BMSK)
       {
           strlcat(temp_str,":RAW_DVDD_SHD", sizeof(temp_str));
       }
       if((uintnt)(reasons_ptr->off_reason1) & PMIO_PON_GEN2_OFF_REASON_RAW_XVDD_SHD_BMSK)
       {
           strlcat(temp_str,":RAW_XVDD_SHD", sizeof(temp_str));
       }
   }

   //FALT Reasons
   //PON_FAULT_REASON1
   if ((uintnt)(fault_reasons_ptr->fault_reason1) != 0x00) 
   {
       snprintf(temp_str1, PM_STATUS_MSG_LEN, ": FAULT1=0x%lx", (uintnt)fault_reasons_ptr->fault_reason1);
       strlcat(temp_str,temp_str1, sizeof(temp_str1));

       if ((uintnt)(fault_reasons_ptr->fault_reason1) & PMIO_PON_GEN2_FAULT_REASON1_AVDD_RB_BMSK) 
       {
           strlcat(temp_str,":AVDD_RB", sizeof(temp_str));
       }
       if((uintnt)(fault_reasons_ptr->fault_reason1) & PMIO_PON_GEN2_FAULT_REASON1_UVLO_BMSK)
       {
           strlcat(temp_str,":UVLO", sizeof(temp_str));
       }
       if((uintnt)(fault_reasons_ptr->fault_reason1) & PMIO_PON_GEN2_FAULT_REASON1_OVLO_BMSK)
       {
           strlcat(temp_str,":OVLO", sizeof(temp_str));
       }
       if((uintnt)(fault_reasons_ptr->fault_reason1) & PMIO_PON_GEN2_FAULT_REASON1_MBG_FAULT_BMSK)
       {
           strlcat(temp_str,":MBG_FAULT", sizeof(temp_str));
       }
       if((uintnt)(fault_reasons_ptr->fault_reason1) & PMIO_PON_GEN2_FAULT_REASON1_GP_FAULT3_BMSK)
       {
           strlcat(temp_str,":GP_FAULT3", sizeof(temp_str));
       }
       if((uintnt)(fault_reasons_ptr->fault_reason1) & PMIO_PON_GEN2_FAULT_REASON1_GP_FAULT2_BMSK)
       {
           strlcat(temp_str,":GP_FAULT2", sizeof(temp_str));
       }
       if((uintnt)(fault_reasons_ptr->fault_reason1) & PMIO_PON_GEN2_FAULT_REASON1_GP_FAULT1_BMSK)
       {
           strlcat(temp_str,":GP_FAULT1", sizeof(temp_str));
       }
       if((uintnt)(fault_reasons_ptr->fault_reason1) & PMIO_PON_GEN2_FAULT_REASON1_GP_FAULT0_BMSK)
       {
           strlcat(temp_str,":GP_FAULT0", sizeof(temp_str));
       }
   }


   //FAULT_REASON2
   if ((uintnt)(fault_reasons_ptr->fault_reason2) != 0x00) 
   {
       snprintf(temp_str1, PM_STATUS_MSG_LEN, ": FAULT2=0x%lx", (uintnt)fault_reasons_ptr->fault_reason2);
       strlcat(temp_str,temp_str1, sizeof(temp_str1));

       if ((uintnt)(fault_reasons_ptr->fault_reason2) & PMIO_PON_GEN2_FAULT_REASON2_OTST3_BMSK) 
       {
           strlcat(temp_str,":OTST3", sizeof(temp_str));
       }
       if((uintnt)(fault_reasons_ptr->fault_reason2) & PMIO_PON_GEN2_FAULT_REASON2_RESTART_PON_BMSK)
       {
           strlcat(temp_str,":RESTART_PON", sizeof(temp_str));
       }
       if((uintnt)(fault_reasons_ptr->fault_reason2) & PMIO_PON_GEN2_FAULT_REASON2_PBS_NACK_BMSK)
       {
           strlcat(temp_str,":PBS_NACK", sizeof(temp_str));
       }
       if((uintnt)(fault_reasons_ptr->fault_reason2) & PMIO_PON_GEN2_FAULT_REASON2_PBS_WD_TO_BMSK)
       {
           strlcat(temp_str,":PBS_WD_TO", sizeof(temp_str));
       }
       if((uintnt)(fault_reasons_ptr->fault_reason2) & PMIO_PON_GEN2_FAULT_REASON2_FAULT_N_BMSK)
       {
           strlcat(temp_str,":FAULT_N", sizeof(temp_str));
       }
   }

   //S3_RESET_REASON
   if ((uintnt)(fault_reasons_ptr->s3_reset_reason) != 0x00) 
   {
       snprintf(temp_str1, PM_STATUS_MSG_LEN, ": S3 RST=0x%lx", (uintnt)fault_reasons_ptr->s3_reset_reason);
       strlcat(temp_str,temp_str1, sizeof(temp_str1));

       if ((uintnt)(fault_reasons_ptr->s3_reset_reason) & PMIO_PON_GEN2_S3_RESET_REASON_KPDPWR_ANDOR_RESIN_BMSK) 
       {
           strlcat(temp_str,":KPDPWR_ANDOR_RESIN", sizeof(temp_str));
       }
       if((uintnt)(fault_reasons_ptr->s3_reset_reason) & PMIO_PON_GEN2_S3_RESET_REASON_PBS_NACK_BMSK)
       {
           strlcat(temp_str,":PBS_NACK", sizeof(temp_str));
       }
       if((uintnt)(fault_reasons_ptr->s3_reset_reason) & PMIO_PON_GEN2_S3_RESET_REASON_PBS_WD_TO_BMSK)
       {
           strlcat(temp_str,":PBS_WATCHDOG_TO", sizeof(temp_str));
       }
       if((uintnt)(fault_reasons_ptr->s3_reset_reason) & PMIO_PON_GEN2_S3_RESET_REASON_FAULT_N_BMSK)
       {
           strlcat(temp_str,":FAULT_N", sizeof(temp_str));
       }
   }

   //SOFT_RESET_REASON1
   if ((uintnt)(fault_reasons_ptr->soft_reset_reason1) != 0x00) 
   {
       snprintf(temp_str1, PM_STATUS_MSG_LEN, ": SOFT RST=0x%lx", (uintnt)fault_reasons_ptr->soft_reset_reason1);
       strlcat(temp_str,temp_str1, sizeof(temp_str1));

       if ((uintnt)(fault_reasons_ptr->soft_reset_reason1) & PMIO_PON_GEN2_SOFT_RESET_REASON1_SOFT_BMSK) 
       {
           strlcat(temp_str,":SOFT", sizeof(temp_str));
       }
   }
   pm_log_message(temp_str); 
   return err_flag;
}


