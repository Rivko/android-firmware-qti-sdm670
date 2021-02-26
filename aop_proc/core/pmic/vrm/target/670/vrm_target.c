/*! \file vrm_target.c
*
*  \brief Contains target specific vrm based implementation.
*  \n
*  &copy; Copyright 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/target/670/vrm_target.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/13/17   as      Making MSS voltage request as Fire and Forget ( CR#2158045 )
07/06/17   kt      Initial version
===========================================================================*/

#include "vrm_target.h"
#include "rinit.h"
#include "vrm_hal.h"
#include "pm_comm_inc.h"

#define   S5_CTRL_PFM_CTL                  0x2044
#define   S5_PS_SEC_ACCESS                 0x21D0
#define   S5_PS_PFM_CURRENT_LIM_CTL        0x214B
#define   MSS_SLAVE_ID                     0x1

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

typedef struct 
{
    uint32 rsrc_id ;
    uint32 irq_count;
}vrm_mss_pbs_info_type ;

//******************************************************************************
// Global Data
//******************************************************************************

static vrm_mss_pbs_info_type vrm_mss_pbs_info ={0};
static uint32 g_wrs_data = 0x0;

//******************************************************************************
// Local Helper Functions
//******************************************************************************
vrm_result vrm_target_rsrc_aon(vrm_info* vrm_ptr)
{
   return VRM_SUCCESS;
}

vrm_result vrm_target_specific_rinit(vrm_info* vrm_ptr)
{
   vrm_result result = VRM_SUCCESS;
   int ret_val = RINIT_SUCCESS;

   ret_val = rinit_module_init ("vrm_tcs");

   if (ret_val != RINIT_SUCCESS)
   {
      result |= VRM_RINIT_ERROR;
   }

   return result;
}

vrm_result vrm_target_mss_init(vrm_info* vrm_ptr)
{
    vrm_result result = VRM_SUCCESS;
    uint32 vrm_address = 0x0;
    
    vrm_address = cmd_db_query_addr("vrm.mss");
     if (vrm_address == 0)
     {
      result |= VRM_CMD_DB_ERROR;
     }

    result |= vrm_get_rsrc_id(VRM_RSRC_REGULATOR, vrm_address, &(vrm_mss_pbs_info.rsrc_id));
    CORE_VERIFY(result==VRM_SUCCESS);
    
    if(result==VRM_SUCCESS)
    {
    result |= vrm_hal_interrupt_change_irq_enable(vrm_ptr , vrm_mss_pbs_info.rsrc_id , VRM_VOLTAGE ,TRUE );
    //Enable settling block
    //result |= vrm_hal_rescfg_settling_block_enable(vrm_ptr,vrm_mss_pbs_info.rsrc_id,VRM_VOLTAGE,TRUE);
    }
    return result;
}

pm_err_flag_type pm_pwr_mss_low_volt_comp(uint32 voltage)
{
    pm_err_flag_type err_flag=PM_ERR_FLAG_SUCCESS;
    uint8 data = 0x0;
    uint8 data_lim_ctl = 0x86;
    if (voltage > 750)
    {
        data = 0xC5;
        data_lim_ctl=0x88;
    }
    else if (voltage > 520)
    {
        data = 0xC6;
    }
    else
    {
        data = 0xC7;
    }
    err_flag |= pm_comm_write_byte(MSS_SLAVE_ID, S5_CTRL_PFM_CTL, data, 0);
    err_flag |= pm_comm_write_byte(MSS_SLAVE_ID, S5_PS_SEC_ACCESS, 0xA5, 0);
    err_flag |= pm_comm_write_byte(MSS_SLAVE_ID, S5_PS_PFM_CURRENT_LIM_CTL, data_lim_ctl, 0);

    return err_flag;
}



vrm_result vrm_target_mss_mode_workaround(vrm_info* vrm_ptr, uint64* change_irq_status, uint32 num_settings)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   vrm_result result = VRM_SUCCESS;
   uint64 mss_change_irq_status = 0x0;
   uint64 bit_mask = 0x1;
   uint32 wrs_data = 0x0;
   uint32 crs_data = 0x0;
   if (num_settings > VRM_NUM_SETTINGS)
   {
      return VRM_OUT_OF_BOUND_ERROR;
   }
   mss_change_irq_status = change_irq_status[VRM_VOLTAGE] &(bit_mask<<(vrm_mss_pbs_info.rsrc_id));
   if(mss_change_irq_status!=0)
   {
      result |= vrm_hal_interrupt_change_irq_enable(vrm_ptr,vrm_mss_pbs_info.rsrc_id,VRM_VOLTAGE,FALSE);
      
      result |= vrm_hal_interrupt_change_irq_clear(vrm_ptr,vrm_mss_pbs_info.rsrc_id,VRM_VOLTAGE);
      
      result |= vrm_hal_rescfg_wrs_read(vrm_ptr,vrm_mss_pbs_info.rsrc_id,VRM_VOLTAGE,&wrs_data);

      g_wrs_data = wrs_data;

      err_flag = pm_pwr_mss_low_volt_comp(wrs_data);
      CORE_VERIFY(err_flag==PM_ERR_FLAG_SUCCESS);
    /*
      result |= vrm_hal_rescfg_settling_block_enable(vrm_ptr,vrm_mss_pbs_info.rsrc_id,VRM_VOLTAGE,FALSE);
      while(crs_data!=wrs_data)
      {
      result |= vrm_hal_rescfg_crs_read(vrm_ptr,vrm_mss_pbs_info.rsrc_id,VRM_VOLTAGE,&crs_data);
      }
      
      
      result |= vrm_hal_rescfg_settling_block_enable(vrm_ptr,vrm_mss_pbs_info.rsrc_id,VRM_VOLTAGE,TRUE);
	*/
      
      result |= vrm_hal_interrupt_change_irq_enable(vrm_ptr,vrm_mss_pbs_info.rsrc_id,VRM_VOLTAGE,TRUE);
      
      vrm_mss_pbs_info.irq_count =vrm_mss_pbs_info.irq_count + 1; // irq_count will increase if request will come for mss rail
   }
   

    return result;
}


//******************************************************************************
// API Functions
//******************************************************************************
                            
/**
 * @brief VRM target specific initialization. 
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_target_post_init(vrm_info* vrm_ptr)
{
   vrm_result result = VRM_SUCCESS;

   result |= vrm_target_rsrc_aon(vrm_ptr);

   result |= vrm_target_specific_rinit(vrm_ptr);

   result |= vrm_target_mss_init(vrm_ptr);

   return result;
}
                            
/**
 * @brief Handles VRM related target specific dependency 
 *        interrupts.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info.
 * 
 * @param[in] settled_irq_status settled interrupt status
 * 
 * @param[in] num_settings num of settings 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_target_dep_irq_handler(vrm_info* vrm_ptr, uint64* dep_irq_status, uint32 num_settings)
{
   return VRM_SUCCESS;
}
                            
/**
 * @brief Handles VRM related target specific change interrupts.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info.
 * 
 * @param[in] settled_irq_status settled interrupt status
 * 
 * @param[in] num_settings num of settings 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_target_change_irq_handler(vrm_info* vrm_ptr, uint64* change_irq_status, uint32 num_settings)
{
   vrm_result result = VRM_SUCCESS;
   
   result = vrm_target_mss_mode_workaround(vrm_ptr , change_irq_status ,num_settings );
      
   return result;
}
                            
/**
 * @brief Handles VRM related target specific settled 
 *        interrupts.
 * 
 * @param[in] vrm_ptr Pointer to struct containing all the VRM 
 *                    driver related info.
 * 
 * @param[in] settled_irq_status settled interrupt status
 * 
 * @param[in] num_settings num of settings 
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_target_settled_irq_handler(vrm_info* vrm_ptr, uint64* settled_irq_status, uint32 num_settings)
{
   return VRM_SUCCESS;
}

