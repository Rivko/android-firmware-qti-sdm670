/*! \file  pmic_sbl_gen_driver.c
 *  
 *  \brief  File Contains the PMIC Set Mode Implementation
 *  \details Set Mode implementation is responsible for setting  
 *  all mode settings such as Register values, memory values, etc.
 *  
 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2013-2017 Qualcomm Technologies Inc, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/framework/src/pm_config_sbl.c#2 $ 
$DateTime: 2019/02/13 01:11:23 $  $Author: pwbldsvc $


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/03/17   aab     Added update to use xblconfig image
08/03/16   akm     Changed pm_clk_busy_wait to pm_busy_wait
03/03/16   aab     Updated driver to support Mask write
10/22/15   al      KW fix
10/12/15   al      Adding API to set PMIC ELF start address 
09/01/15   aab     Updated driver to support SBL Conditional configuration feature
07/13/15   pb      Moved out Settings validation test code to separate file
04/17/15   aab     Added pm_sbl_config2 to support msm8996 V2/V1 compatibility
11/10/14   aab     Updated driver to get pbs data size from sbl config data
07/16/14   akm     Comm change Updates
06/17/14   akm     Added PSI support
08/07/13   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_sbl_boot.h"				/* Needed for pm_clk_busy_wait and sbi_mini initialization */
#include "pm_config_sbl.h"	                        /* Contains the type definitions and static register tables */
#include "pm_target_information.h"
#include "CoreVerify.h"
#include "pm_version.h"
#include "XBLConfig.h"


static uint64 pm_bl_psi_addr = SCL_PMIC_CONFIG_BUF_BASE;
#define PM_SBL_COND_REV_ID_REG 0x52

pm_err_flag_type
pm_sbl_validate_cond_config( pm_sbl_config_info_type *sbl_config_info_ds,
                              pm_sbl_cond_seq_type   *cond_ds_ptr, 
                                            uint8    cond_start_index,
                                            uint8    cond_end_index,
                                            boolean  *valid_config_flag )
{
   pm_err_flag_type err_flag =  PM_ERR_FLAG_SUCCESS;
   uint8 reg_read_data;
   uint8 index = cond_start_index;
  uint32 pmic_index;
  pm_device_info_type pmic_info;
   pm_sbl_cond_seq_type *cond_cfg_tbl = &cond_ds_ptr[cond_start_index - 1]; //conditional reg config table ptr. Index 0 is used in cond table
   uint8 max_num_cond_config = sbl_config_info_ds->cond_num_entries; //Max number of entries in conditiaonl table
   pm_err_flag_type err_flag_chip_rev = PM_ERR_FLAG_SUCCESS;
   boolean valid_sid = FALSE;
   uint32 rev_id_read;


  if ( ((cond_start_index == 0)  && (cond_end_index  > 0 )) ||  
       ((cond_start_index  > 0)  && (cond_end_index == 0 )) ||
       ( cond_start_index   > max_num_cond_config)       ||
       ( cond_end_index     > max_num_cond_config)     )
   {
       err_flag |= PM_ERR_FLAG_INVALID_PARAMETER; //Invalid cond config indexing
       *valid_config_flag = FALSE;
   }
   else
   {
       for(; index <= cond_end_index; index++, cond_cfg_tbl++)  //Execute conditional check
       {
          //Check for valid SID
          err_flag_chip_rev |= pm_resolve_chip_revision(cond_cfg_tbl->cond_sid, &valid_sid, &rev_id_read);
          if (valid_sid == FALSE)
          {
             *valid_config_flag = FALSE;
             continue;  //This configuation does NOT apply to this SID (PMIC does not exist)
          }
	    if(cond_cfg_tbl->cond_address == PM_SBL_COND_REV_ID_REG) // new clogic for Rev ID check 
	    {
	      pmic_index = (uint8)(cond_cfg_tbl->cond_sid / 2);
	      pm_get_pmic_info(pmic_index,&pmic_info); 
	      reg_read_data = ((pmic_info.nPmicAllLayerRevision << 4) |(pmic_info.nPmicMetalRevision));
	    }
	    else
	    {
	           err_flag |= pm_comm_read_byte_mask( cond_cfg_tbl->cond_sid, 
	                                               cond_cfg_tbl->cond_address, 
	                                               cond_cfg_tbl->cond_mask, 
	                                               &reg_read_data, 1);
	    }
	     switch(cond_cfg_tbl->cond_operation)
         {
         case EQUAL:
               *valid_config_flag &= (reg_read_data == cond_cfg_tbl->cond_data)?TRUE:FALSE;
            break;
         case GREATER:
               *valid_config_flag &= (reg_read_data > cond_cfg_tbl->cond_data)?TRUE:FALSE;
            break;
         case GREATER_OR_EQUAL:
               *valid_config_flag &= (reg_read_data >= cond_cfg_tbl->cond_data)?TRUE:FALSE;
            break;
         case LESS:
               *valid_config_flag &= (reg_read_data < cond_cfg_tbl->cond_data)?TRUE:FALSE;
            break;
         case LESS_OR_EQUAL:
               *valid_config_flag &= (reg_read_data <= cond_cfg_tbl->cond_data)?TRUE:FALSE;
            break;
         default:
               *valid_config_flag = FALSE;
            break;
         }
      }
   }

   return err_flag;
}




pm_err_flag_type 
pm_sbl_config()
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   pm_err_flag_type err_flag_chip_rev = PM_ERR_FLAG_SUCCESS;
   pm_sbl_seq_type *rc = NULL; //Reg config ptr
   boolean valid_sid = FALSE;
   uint32 rev_id_read;
   boolean valid_config_flag = TRUE;

   uint32 xbl_cfg_h = 0;
   uint32 read_offset = 0;
   uint32 xbl_cfg_size = 0;

   xblconfig_status_type xblcfg_err_flag = XBLCONFIG_SUCCESS;
   void *pm_sbl_pdm_config_ds_buff = (void*)PM_BL_PSI_ADDR;
   pm_sbl_pdm_config_type  *pm_sbl_pdm_config_ds = NULL;

   xblcfg_err_flag |= xblconfig_open("/pmic_settings.bin", &xbl_cfg_h, &xbl_cfg_size);
   if(xblcfg_err_flag != XBLCONFIG_SUCCESS)
   {
      return err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }

   xblcfg_err_flag |= xblconfig_read(xbl_cfg_h, read_offset, xbl_cfg_size, (void*)pm_sbl_pdm_config_ds_buff);
   if((xblcfg_err_flag != XBLCONFIG_SUCCESS) || (xbl_cfg_size == 0) )
   {
      return err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }

   xblcfg_err_flag |= xblconfig_close(xbl_cfg_h);
   if(xblcfg_err_flag != XBLCONFIG_SUCCESS)
   {
      return err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }

   //PSI: Get PDM data pointer from xblconfig data image
   pm_sbl_pdm_config_ds   = (pm_sbl_pdm_config_type*)pm_sbl_pdm_config_ds_buff;

   CORE_VERIFY_PTR(pm_sbl_pdm_config_ds);
   CORE_VERIFY_PTR(pm_sbl_pdm_config_ds->pdm_data);
   pm_sbl_seq_type         *sbl_reg_config_ds      = pm_sbl_pdm_config_ds->pdm_data->pm_sbl_seq; 
   pm_sbl_cond_seq_type    *sbl_cond_config_ds     = pm_sbl_pdm_config_ds->pdm_data->pm_sbl_cond_seq;
   pm_pbs_ram_data_type    *pbs_ram_config_ds      = pm_sbl_pdm_config_ds->pdm_data->pm_pbs_ram_seq;
   pm_sbl_config_info_type *sbl_config_info_ds     = pm_sbl_pdm_config_ds->pm_psi_info;  
   
   if ((sbl_reg_config_ds == NULL)||(sbl_cond_config_ds == NULL)|| (pbs_ram_config_ds == NULL))
   {
      return PM_ERR_FLAG_INVALID_PARAMETER;
   }

   if( (sbl_config_info_ds->signature != PSI_SIGNATURE) ||
       (sbl_config_info_ds->major_ver != 1) ||
       (sbl_config_info_ds->minor_ver != 2) ) //Added mask write support
   { 
     CORE_VERIFY(0); //Error fatal if the pmic image signature doesn't match 
   } 

   
   for(rc = sbl_reg_config_ds; (rc->reg_operation != PM_SBL_OPERATION_INVALID); rc++)  
   {
      if (rc->reg_operation != PM_SBL_DELAY ) 
      {
          err_flag_chip_rev |= pm_resolve_chip_revision(rc->sid, &valid_sid, &rev_id_read);
          if (valid_sid == FALSE)
      {
             continue;  //This configuration does NOT apply to this SID (PMIC does not exist)
      }
      }
   
      if ( (rc->cond_start_index == 0) && (rc->cond_end_index == 0) ) 
      {
        valid_config_flag = TRUE; //Normal configuration
      }
      else
      {
          valid_config_flag = TRUE;
          err_flag |= pm_sbl_validate_cond_config(sbl_config_info_ds, sbl_cond_config_ds, rc->cond_start_index, rc->cond_end_index, &valid_config_flag); //check conditional config
      }
   

      if( valid_config_flag == TRUE )
     { 
        switch(rc->reg_operation)  
        {
           case PM_SBL_WRITE:
              {
                 if(rc->mask == 0xFF)
                 { 
                   err_flag |= pm_comm_write_byte(rc->sid, rc->address, rc->data, 1);
                 }
                 else
                 {
                    err_flag |= pm_comm_write_byte_mask(rc->sid, rc->address, rc->mask, rc->data,1);
                 }
              }
              break;
           case PM_SBL_DELAY: 
              {
                 pm_busy_wait(rc->address); //Address field contains delay in us
              }
              break;
           case PM_SBL_PBS_RAM:
              {
                  err_flag |= pm_pbs_ram_image_load(rc->sid, &pbs_ram_config_ds[PBS_RAM_DATA_SIZE * rc->data], rc->address); //data: Index in PBS RAM Table; address: Size of PBS RAM
              }
              break;
           case PM_SBL_NOP:
              break;
           default:
                 err_flag |= PM_ERR_FLAG_FAILURE;
              break;
        }
     }
   }

   return err_flag;
}

pm_err_flag_type pm_psi_set_base_address(uint64 address)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  if (address != 0x0)
  {
    /*update variable that holds PMIC elf start address*/
    pm_bl_psi_addr = address;
  }
  else
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
   
   return err_flag;
} 


pm_err_flag_type pm_psi_get_base_address(uint64* address)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   /*update with variable that holds PMIC elf start address*/
   if(NULL != address)
   { 
     *address = pm_bl_psi_addr;
   }
   else
   {
     err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
   }
   return err_flag;
} 
