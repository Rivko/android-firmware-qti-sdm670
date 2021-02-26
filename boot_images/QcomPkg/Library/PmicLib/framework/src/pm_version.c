/*! \file pm_version.c 
*  \n
*  \brief This file contains PMIC version auto-detection implementation
*  \n
*  \n &copy; Copyright 2010-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/*===================================================================
			        EDIT HISTORY FOR MODULE
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/18/17   ks      Add support to read PMIC Variant revision
04/14/15   al      Adding changes for Hawker support
07/16/14   akm     Comm change Updates
05/15/13   kt      New PMIC Version driver format.
04/16/13   kt      Added pm_get_pbs_info API.
01/26/13   kt      Added pm_get_pmic_info API.
01/24/13   kt      Adding device index param to pmic model and revision APIs.
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_version.h"
#include "DALSys.h"
#include "pm_utils.h"
#include "pm_comm.h"
#include "device_info.h"
#include "pm_config_sbl.h"
#include "pm_config_target.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/* QC PMIC Peripheral type value */
#define PM_TYPE_QC_DEVICE        0x51

/* PMIC REV_ID Reg Base Address */
#define PM_REVID_BASE_ADDR       0x0100

typedef enum
{
    PM_VER_INFO__RFU = 0,
    PM_VER_INFO__VARIANT,
    PM_VER_INFO__METAL,
    PM_VER_INFO__ALL_LAYER,
    PM_VER_INFO__PMIC_TYPE,
    PM_VER_INFO__PMIC_MODEL,
    PM_VER_INFO__MAX
} pm_version_info_reg;

/*===========================================================================

                        GLOBAL DATA DEFINITIONS

===========================================================================*/
/* Static global variable to store the pmic device info */
static pm_device_info_type pm_device_info_arr[PM_MAX_NUM_PMICS];
static pm_pdm_chip_info_type pm_pdm_chip_info_arr[PM_MAX_NUM_PMICS];

/* Static global variable to store the pmic device primary slave id value */
static uint32 pm_primary_slave_id[PM_MAX_NUM_PMICS];

/* Flag to check if PMIC Version driver is initialized */
static boolean pm_version_initialized = FALSE;

/*num of slaves per pmic should be fixed for a target - current
plan is to support fixed number of slaves per pmic, 
with the existing pmic config we cannot support a 
target having both 1 and 2 slaves per pmic on the same target*/
static const uint8 pm_num_slaves_per_pmic = PM_MAX_NUM_SLAVE_IDS_PER_PMIC; 

/*=========================================================================
      Function Definitions
==========================================================================*/
/** 
 * @name pm_version_detect 
 *  
 * @brief Please refer to device_info.h file for further info on
 *        this function.
 */
pm_err_flag_type pm_version_detect(void) 
{
  pm_err_flag_type  err_flag = PM_ERR_FLAG_SUCCESS;
  uint8             pmic_index = 0, slave_id = 0;
  uint8             pmic_rev_info[PM_VER_INFO__MAX] = {0};
 
 

  /* This function needs to be called only once to read the PMIC
     version info and store it */
  if (pm_version_initialized)
  {
    return err_flag;
  }

  memset(pm_primary_slave_id, INVALID_SLAVE_ID, sizeof(uint32)* PM_MAX_NUM_PMICS);

  for(slave_id = 0; slave_id < PM_MAX_NUM_SLAVE_IDS;
                slave_id = slave_id + pm_num_slaves_per_pmic,pmic_index++)
    {
    if(pmic_index >= PM_MAX_NUM_PMICS)
    {
      break;
    }

    /* Read PMIC Device revision information */
    err_flag = pm_comm_read_byte_array(slave_id, PM_REVID_BASE_ADDR, PM_VER_INFO__MAX, (uint8 *)pmic_rev_info, 0);

    /* Make sure if the QC PMIC device exists or not based on if the SPMI
       read operation for PMIC peripheral type value for the specific slave
       id is successful or not and if the data read is equal to QC PMIC
       peripheral type value or not */

 if ((err_flag != PM_ERR_FLAG_SUCCESS) || (pmic_rev_info[PM_VER_INFO__PMIC_TYPE] != PM_TYPE_QC_DEVICE) )
    {
      /* Once it's confirmed that the QC PMIC device corresponding to the
         slave id doesn't exist, then disregard the error flag returned
         from SPMI read operation and continue to check for other slave ids */
      err_flag = PM_ERR_FLAG_SUCCESS;
      continue;      
    } 
    /* Store PMIC chip revison information in static global array */
    pm_device_info_arr[pmic_index].ePmicModel             = (pm_model_type)pmic_rev_info[PM_VER_INFO__PMIC_MODEL];
    pm_device_info_arr[pmic_index].nPmicAllLayerRevision  = (uint32)pmic_rev_info[PM_VER_INFO__ALL_LAYER];
    pm_device_info_arr[pmic_index].nPmicMetalRevision     = (uint32)pmic_rev_info[PM_VER_INFO__METAL];
    
    pm_pdm_chip_info_arr[pmic_index].pdmChipRevision      =  (pmic_rev_info[PM_VER_INFO__ALL_LAYER] << 24) |
                                                             (pmic_rev_info[PM_VER_INFO__METAL] << 16) |
                                                             (pmic_rev_info[PM_VER_INFO__VARIANT] << 8) |
                                                             (pmic_rev_info[PM_VER_INFO__RFU]);

    /* Store PMIC Device Primary slave id value */
    pm_primary_slave_id[pmic_index] = slave_id;
  }
  pm_version_initialized = TRUE;
  return err_flag;
}

pm_err_flag_type pm_version_deinit(void)
{
  pm_version_initialized = FALSE;
  
  return PM_ERR_FLAG_SUCCESS;
}


/** 
 * @name pm_get_pmic_model 
 *  
 * @brief This is a public API. Please refer to pm_version.h 
 *        file for further info on this function.
 */
pm_model_type pm_get_pmic_model(uint8 pmic_index)
{
  pm_model_type model = PMIC_IS_INVALID;

  if((!pm_version_initialized) ||  (pmic_index >= PM_MAX_NUM_PMICS)) 
  {
    return model;
  }

  model = pm_device_info_arr[pmic_index].ePmicModel;

  return model;
}

/** 
 * @name pm_get_pmic_revision 
 *  
 * @brief This is a public API. Please refer to pm_version.h 
 *        file for further info on this function.
 */
uint8 pm_get_pmic_revision(uint8 pmic_index)
{
  uint8 all_layer_rev = 0;

  if((!pm_version_initialized) || (pmic_index >= PM_MAX_NUM_PMICS)) 
  {
    return all_layer_rev;
  }

  all_layer_rev = pm_device_info_arr[pmic_index].nPmicAllLayerRevision;

  return all_layer_rev;
}

/** 
 * @name pm_get_slave_id
 *  
 * @brief Please refer to device_info.h file for further info on
 *        this function.
 */
pm_err_flag_type pm_get_slave_id(uint8 pmic_index, uint8 slave_id_index, uint32* slave_id_ptr)
{

  if((slave_id_ptr == NULL) || (!pm_version_initialized) || 
     (slave_id_index > 1)) 
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  if(pmic_index >= PM_MAX_NUM_PMICS)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  *slave_id_ptr = pm_primary_slave_id[pmic_index] + slave_id_index;

  if(*slave_id_ptr >= PM_MAX_NUM_SLAVE_IDS) 
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  return PM_ERR_FLAG_SUCCESS;
}

uint8 pm_get_num_slaves_per_pmic(void)
{
  return pm_num_slaves_per_pmic;
}


pm_err_flag_type pm_get_pmic_info(uint8 pmic_index, pm_device_info_type* pmic_device_info_ptr)

{
  if((pmic_device_info_ptr == NULL) || (!pm_version_initialized) || 
     (pmic_index >= PM_MAX_NUM_PMICS))
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  pmic_device_info_ptr->ePmicModel = pm_device_info_arr[pmic_index].ePmicModel;
  pmic_device_info_ptr->nPmicAllLayerRevision = pm_device_info_arr[pmic_index].nPmicAllLayerRevision;
  pmic_device_info_ptr->nPmicMetalRevision = pm_device_info_arr[pmic_index].nPmicMetalRevision;

  return PM_ERR_FLAG_SUCCESS;
}

pm_err_flag_type pm_get_pmic_variant(uint8 pmic_index, uint8 *pmic_variant)
{
  if((!pm_version_initialized) || (pmic_index >= PM_MAX_NUM_PMICS))
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if(!pmic_variant)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  pm_err_flag_type  err_flag = PM_ERR_FLAG_SUCCESS;
  uint8             slave_id = (2 * pmic_index);  /* REV_ID Periph is on SlaveID '0' */

  /* Read PMIC Device Variant information */
  err_flag = pm_comm_read_byte(slave_id, (PM_REVID_BASE_ADDR + PM_VER_INFO__VARIANT), pmic_variant, 0);

  return err_flag;
}

pm_err_flag_type pm_resolve_chip_revision(uint32 slave_id, boolean* valid_sid, uint32* pmic_revision )
{
   pm_err_flag_type  err_flag = PM_ERR_FLAG_SUCCESS;
   uint8 pmic_index;
   pm_model_type pmic_model = PMIC_IS_INVALID;

   pmic_index = (uint8)(slave_id / 2);  //calculate pmic_index from sid

   pmic_model = pm_get_pmic_model(pmic_index);
   if ((pmic_model == PMIC_IS_INVALID) || (pmic_model == PMIC_IS_UNKNOWN) || (pmic_index >= PM_MAX_NUM_PMICS))
   {
      *valid_sid = FALSE;
      err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else
   {
      *valid_sid = TRUE;
      *pmic_revision =  pm_pdm_chip_info_arr[pmic_index].pdmChipRevision;
   }

   return err_flag;
}



pm_err_flag_type 
pm_get_peripheral_info(uint8 sid_index, peripheral_info_type *peripheral_info)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type base_address = (pm_register_address_type)peripheral_info->base_address;
    pm_register_data_type temp_peripheral_info[PM_PERIPH_INFO__MAX] = {0};

    /* Reset the peripheral info */
    peripheral_info->peripheral_type = 0;
    peripheral_info->peripheral_subtype = 0;
    peripheral_info->analog_major_version = 0;
    peripheral_info->analog_minor_version = 0;
    peripheral_info->digital_major_version = 0;
    peripheral_info->digital_minor_version = 0;

    /* Burst read the peripheral info */
    pm_comm_read_byte_array(sid_index, base_address, PM_PERIPH_INFO__MAX, temp_peripheral_info, 0);

    /* Valid peripheral, type/subtype can never be 0 */
    if( (temp_peripheral_info[PM_PERIPH_INFO__TYPE] == 0) || (temp_peripheral_info[PM_PERIPH_INFO__SUBTYPE] == 0) )
    {
        return PM_ERR_FLAG_INVALID_PERIPHERAL;
    }

    peripheral_info->peripheral_type = temp_peripheral_info[PM_PERIPH_INFO__TYPE];
    peripheral_info->peripheral_subtype = temp_peripheral_info[PM_PERIPH_INFO__SUBTYPE];
    peripheral_info->digital_major_version = temp_peripheral_info[PM_PERIPH_INFO__DIG_MAJOR];
    peripheral_info->digital_minor_version = temp_peripheral_info[PM_PERIPH_INFO__DIG_MINOR];
    peripheral_info->analog_major_version = temp_peripheral_info[PM_PERIPH_INFO__ANA_MAJOR];
    peripheral_info->analog_minor_version = temp_peripheral_info[PM_PERIPH_INFO__ANA_MINOR];

    return err_flag;
}
