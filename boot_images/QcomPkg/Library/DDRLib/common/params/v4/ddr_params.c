/*
===========================================================================

FILE:         ddr_params.c

DESCRIPTION:
  Information of DDR devices supported by target.

===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     --------------------------------------------------------
03/12/14   sr      Initial revision.

===========================================================================
                   Copyright 2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
===========================================================================
*/

/*==============================================================================
                                    INCLUDES
==============================================================================*/
#include "ddr_params.h"
#include "crc.h"
#include "ddr_log.h"
#include "dev_flash_params.h"
#define USE_LOADER_UTILS
#include "LoaderUtils.h"
#include "Target_cust.h"
#include "ddr_hwio.h"
#include "ddr_target.h"
#include "ddr_sdi_log.h"
#include "boot_util.h"

/*==============================================================================
                                      DATA
==============================================================================*/

BOOT_DDR_STRUCT *ddrsns_share_data = (BOOT_DDR_STRUCT *)SCL_SBL1_DDR_STRUCT_BASE;
DDR_SDI_LOGS *ddr_sdi_logs_ptr = (DDR_SDI_LOGS *)SDI_COOKIES_STRUCT_BASE;


/*==============================================================================
                                   FUNCTIONS
==============================================================================*/


/* ============================================================================
**  Function : ddr_set_params
** ============================================================================
*/
/**
*   @brief
*   This function sets the DDR driver's device table to an external device table.
*   It will also update the interleaving mode information according to the parameter
*   header that's passed in.
*   User should append ddr_device_params_header_v1 header in front of their own DDR device 
*   table and pass  a pointer points to the beginning of this header via the this API to 
*   force DDR driver to utilize the user defined parameters.
*   The user defined ddr device table must contain a minimal number of entries to 
*   match the system's ddr devices.
*   For example if a system has two ddr devices attached then the external table must 
*   define at least two entries to match those two devices.
*   However, more entries can be added to the end of table to support other ddr devices
*   with different parameters.
*
*   @param[in]  ddr_params_ptr   Pointer to the external ddr parameters. It should
*                                point to the user defined ddr device table with ddr
*                                parameters header appended in front. This pointer must
*                                points to a 4 bytes aligned address.
*                         
*
*   @return
*   TRUE if DDR device table is updated, FALSE if an error happens and this operation
*   is aborted
*
*   @dependencies
*   Must be called prior to ddr_initialize_device. 
*   This API will take no effect after ddr_initialize_device
*
*   @sa
*   None
*
*   @sa
*   None
*/
boolean ddr_set_params(boolean dload_mode)
{
  ddr_set_sdi_struct();

  /* memset the 5KB dataram pointer to zero */
  memset(ddrsns_share_data, 0x0, sizeof(BOOT_DDR_STRUCT));
  ddrsns_share_data->shrm_ddr_ptr = (SHRM_DDR_STRUCT *)SCL_SBL1_DDR_SHRM_STRUCT_BASE;
  if ((!dload_mode) || (ddr_sdi_logs_ptr->pass3_cookie->p_cookie_value == DDR_WENT_INTO_PASS3) || (HWIO_INF(AOSS_CC_RESET_FSM_STATUS, DBG_PDC_TRIGGER)))
  {
    qmemscpy((void *)ddrsns_share_data->shrm_ddr_ptr+SIZE_16KB, sizeof(SHRM_DDR_STRUCT), (void *)ddrsns_share_data->shrm_ddr_ptr, sizeof(SHRM_DDR_STRUCT));
    memset(ddrsns_share_data->shrm_ddr_ptr, 0x0, sizeof(SHRM_DDR_STRUCT));
  }    
  
  return TRUE;
}

/* ============================================================================
**  Function : ddr_set_sdi_struct
** ============================================================================
*/
/**
*   @brief
*   This function sets the addresses for the SDI logging space.
*
*   @param[in]  void
*
*   @return
*
*   @dependencies
*   Must be called prior to ddr_set_params
*
*   @sa
*   None
*
*   @sa
*   None
*/

void ddr_set_sdi_struct(void)
{
    /* Fill the SDI Cookie struct with addresses */
  ddr_sdi_logs_ptr->pass1_cookie = (SDIInfo *)PASS1_COOKIE;
  ddr_sdi_logs_ptr->pass2_cookie = (SDIInfo *)PASS2_COOKIE;
  ddr_sdi_logs_ptr->pass3_cookie = (SDIInfo *)PASS3_COOKIE;
  ddr_sdi_logs_ptr->sdi_csr_log = (DDR_SDI_CSR_LOGS *)SDI_REG_LOG_START_ADDR;
}


/* ============================================================================
**  Function : ddr_get_detected_params
** ============================================================================
*/
/**
*   @brief
*   Get DDR device parameters.
*
*   @param[in]  interface_name  Interface to get DDR device parameters for
*
*   @return
*   DDR device parameters
*
*   @dependencies
*   None
*
*   @sa
*   None
*
*   @sa
*   None
*/
struct detected_ddr_device_params *ddr_get_detected_params(DDR_CHANNEL interface_name)
{
  struct detected_ddr_device_params *ddr_detected_params = NULL;
  uint8 i = 0;

  // i = ((interface_name == DDR_CH0) ? 0 : (interface_name == DDR_CH1) ? 1 : (interface_name == DDR_CH2) ? 2 : (interface_name == DDR_CH3) ? 3 : 0);                                       

  while (interface_name != 1)
  {
    interface_name >>= 1;
    i++;
  } 

  ddr_detected_params = &(ddrsns_share_data->shrm_ddr_ptr->detected_ddr_device[i]);

  return ddr_detected_params;
} /* ddr_get_detected_params */

/* ============================================================================
**  Function : ddr_param_interleaved
** ============================================================================
*/
/**
*   @brief
*   Return the interleave setting based on ddr parameters
*
*   @return
*   return a bitmask of interleave status
*
*   @dependencies
*   Must be called after ddr_set_params. Calling it before ddr_set_params will
*   return the default setting which is TRUE.
*
*   @sa
*   None
*
*   @sa
*   None
*/
uint32 ddr_param_interleaved(void)
{
  return DDR_CS0_CS1_INTERLEAVE;
}

/* =============================================================================
**  Function : ddr_params_get_training_data
** =============================================================================
*/
/**
*   @brief
*   Get DDR parameter partition: partition base is returned, and partition size
*   is updated through pointer.
*
*   @param[out]  size  DDR parameter partition size
*
*   @return
*   DDR parameter partition base
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void *ddr_params_get_training_data(uint32 *size)
{
  *size = sizeof(struct ddr_params_partition);

  return (void *)&(ddrsns_share_data->training_results);

} /* ddr_params_get_partition */

/* =============================================================================
**  Function : ddr_params_set_training_data
** =============================================================================
*/
/**
*   @brief
*   Set DDR parameter partition.
*
*   @param[in]  base  DDR parameter partition base
*   @param[in]  size  DDR parameter partition size
*
*   @retval  TRUE   Partition set successfully
*   @retval  FALSE  Partition set unsuccessfully
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean ddr_params_set_training_data(void *base, uint32 size)
{
  boolean success = FALSE;

  if ( base != NULL && size == sizeof(struct ddr_params_partition) )
  {
    ddrsns_share_data->training_results = *(struct ddr_params_partition *)base;
    success = TRUE;
  }
  return success;
}


/* =============================================================================
**  Function : ddr_get_training_checksum
** =============================================================================
*/
/**
*   @brief
*   Get DDR training checksum.
*
*   @param
*   None
*
*   @return
*   DDR training data checksum, 0 if no valid training partition is found
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
uint32 ddr_get_training_checksum(uint32 serial_number)
{
  uint32 tr_data_size_in_bits = 0;
  uint32 crc32_training = 0x0UL;
  
  uint8 crc_buff[sizeof(struct ddr_clock_plan) * MAX_NUM_CLOCK_PLAN + sizeof(uint32)] = {0}; //buffer clock plan/fmax
  uint32 clk_plan_tbl_size = sizeof(struct ddr_clock_plan) * ddrsns_share_data->shrm_ddr_ptr->ddr_runtime.ddr_num_clock_levels;
  uint32 clock_freq_size_in_bits = 0, i = 0;
  uint32 crc32_clock_freq = 0x0UL;
  
  if (&(ddrsns_share_data->training_results) != NULL)
  {
    /* Calculate the training data size in bits */
    tr_data_size_in_bits = sizeof(uint8) * (TRAINING_DATA_SIZE_IN_BYTES-1) * 8; /* TRAINING_DATA_SIZE_IN_BYTES is the training size for LP4 , allocated in ddr_params_partition */
    crc32_training = crc_32_calc((uint8 *)(ddrsns_share_data->training_results.training_data), (uint16)(tr_data_size_in_bits), serial_number);

    /* buffer clock plan */
    for(i = 0; i < clk_plan_tbl_size; i++) {
      crc_buff[i] = *((uint8*)&(ddrsns_share_data->shrm_ddr_ptr->ddr_runtime.clock_plan) + i);
  }

    /* buffer fmax*/
    for(i = 0; i < sizeof(uint32); i++) {
      crc_buff[clk_plan_tbl_size + i] = *((uint8*)&(ddrsns_share_data->shrm_ddr_ptr->ddr_runtime.max_ddr_frequency) + i);
    }

    /* Calculate clock_plan/max_ddr_frequency crc */
    clock_freq_size_in_bits = sizeof(uint8) * (clk_plan_tbl_size + sizeof(uint32)) * 8;
    crc32_clock_freq = crc_32_calc(crc_buff, (uint16)(clock_freq_size_in_bits), serial_number);

    ddr_printf (DDR_NORMAL, "DDR: Computed CRC checksum retrieved from flash is %d", crc32_training ^ crc32_clock_freq);
    return crc32_training ^ crc32_clock_freq;
  }
  else
  {
    return 0;
  }
}
/* =============================================================================
**  Function : ddr_set_training_checksum
** =============================================================================
*/
/**
*   @brief
*   Compute the checksum over entire training data
*
*   @param
*   Serial number
*
*   @return
*   CRC checksum
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean ddr_set_training_checksum(uint32 serial_number)
{
  uint32 tr_data_size_in_bits = 0;
  uint32 crc32_training = 0x0UL;
	
  uint8 crc_buff[sizeof(struct ddr_clock_plan) * MAX_NUM_CLOCK_PLAN + sizeof(uint32)] = {0}; //buffer clock plan/fmax
  uint32 clk_plan_tbl_size = sizeof(struct ddr_clock_plan) * ddrsns_share_data->shrm_ddr_ptr->ddr_runtime.ddr_num_clock_levels;
  uint32 clock_freq_size_in_bits = 0, i = 0;
  uint32 crc32_clock_freq = 0x0UL;

  if (&(ddrsns_share_data->training_results) != NULL)
  {
      /* Calculate the training data size in bits */
    tr_data_size_in_bits = sizeof(uint8) * (TRAINING_DATA_SIZE_IN_BYTES-1) * 8; /* TRAINING_DATA_SIZE_IN_BYTES is the training size for LP4 , allocated in ddr_params_partition */ 
    crc32_training = crc_32_calc((uint8 *)(ddrsns_share_data->training_results.training_data), (uint16)(tr_data_size_in_bits), serial_number);

    /* buffer clock plan */
    for(i = 0; i < clk_plan_tbl_size; i++) {
      crc_buff[i] = *((uint8*)&(ddrsns_share_data->shrm_ddr_ptr->ddr_runtime.clock_plan) + i);
    }

    /* buffer fmax*/
    for(i = 0; i < sizeof(uint32); i++) {
      crc_buff[clk_plan_tbl_size + i] = *((uint8*)&(ddrsns_share_data->shrm_ddr_ptr->ddr_runtime.max_ddr_frequency) + i);
    }

    /* Calculate clock_plan/max_ddr_frequency crc */
    clock_freq_size_in_bits = sizeof(uint8) * (clk_plan_tbl_size + sizeof(uint32)) * 8;
    crc32_clock_freq = crc_32_calc(crc_buff, (uint16)(clock_freq_size_in_bits), serial_number);

    ddrsns_share_data->training_results.checksum = crc32_training ^ crc32_clock_freq;
    ddr_printf (DDR_NORMAL, "DDR: Checksum to be stored on flash is %d", ddrsns_share_data->training_results.checksum);
	  return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/* =============================================================================
**  Function : ddr_get_shared_info_details
** =============================================================================
*/
/**
*   @brief
*   Gets the shared info details from detected ddr device params
*
*   @param
*   ddr_details_entry_ptr
*
*   @return
*   status
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/


boolean ddr_get_shared_info_details(ddr_details_entry_ptr_type ddr_details_entry_ptr)
{
	  boolean status = TRUE;
	  /* allocate memory in shared memory region for ddr info table */
	  struct detected_ddr_device_params * detected_param_ptr = NULL;
	  uint8 channel_no=0, channel=0, rank=0;
		  

	  for(channel_no = 0, channel = 1; channel_no<DDR_MAX_NUM_CH ; channel_no++, channel = channel << 1)
	  {
	    /*Get the detected DDR device params for all channels*/
		detected_param_ptr = ddr_get_detected_params(channel);
		/*Assuming we have a single manufacturer and device type, Can I ? */
		ddr_details_entry_ptr->manufacturer_id= detected_param_ptr->manufacturer_id;
	    ddr_details_entry_ptr->device_type= detected_param_ptr->device_type; 

		for(rank=0; rank < 2; rank++)
		{
			/* Revision ID1 */
			ddr_details_entry_ptr->ddr_params[channel_no].revision_id1[rank] = detected_param_ptr->revision_id_1[rank];
			/* Revision ID2 */
			ddr_details_entry_ptr->ddr_params[channel_no].revision_id2[rank] = detected_param_ptr->revision_id_2[rank];
			/* Width */
			ddr_details_entry_ptr->ddr_params[channel_no].width[rank] = detected_param_ptr->device_io_width[rank];
			/* Density*/
			ddr_details_entry_ptr->ddr_params[channel_no].density[rank] = detected_param_ptr->device_density[rank];	   
		}
		
	  }
	  return status;
}

