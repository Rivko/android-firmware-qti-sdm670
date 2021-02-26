/*
===========================================================================

FILE:         ddr_external_api.c

DESCRIPTION:  
  This is external APIs to support usage of SCALe sequences

  Copyright (c) 2014, 2017 QUALCOMM Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 
===========================================================================
                                EDIT HISTORY

================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/04/17   naa     Added DDI interface support
03/29/17   kpa     Update xblconfig_read function parameters.
02/22/17   kpa     Update XBLCONFIG_ERR_NONE to XBLCONFIG_SUCCESS.
04/05/14   sr      Initial version.
================================================================================
*/

/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include <string.h>
#include "ddr_common.h"
#include "ddr_drivers.h"
#include "seq_hwio.h"
#include "ddr_external.h"
#include "ddr_params.h"
#include "ClockBoot.h"
#include "ddr_target.h"
#include "busywait.h"
#include "ddr_hwio.h"
#include "XBLConfig.h"
#include "CoreVerify.h"
#include "DALSys.h"
#include "Target_cust.h"
#include "ddr_internal.h"
#include "dcb.h"
#include "boot_util.h"
#include  "boot_error_if.h"
#include "LoaderUtils.h"

/*==============================================================================
                                  MACROS
==============================================================================*/
#define MAX_AVAILABLE_TEMP_TRAINING_BUFFER ( 64 * 1024 ) /* 100kb */
#define DCB_NAME_BUF_LEN_MAX 22

/*==============================================================================
                                  DATA
==============================================================================*/
/* reserve 20kb for training data */
static uint32 training_data_offset = 20 * 1024;

/* starting from offset of 0 for extended training log */
static uint32 training_log_offset_size = 0; 
/*==============================================================================
                                  ASSEMBLY
==============================================================================*/


/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
void sbl1_save_ddr_training_data_to_partition
(
  uint8* ddr_training_data_ptr,
  uint32 ddr_training_data_size,
  uint32 ddr_training_data_offset
);

void sbl1_save_data_to_ddr_partition
(
  uint8* ddr_partition_data_ptr,
  uint32 ddr_partition_data_size,
  uint32 ddr_partition_data_offset
);


void ddr_external_snprintf(char* str, size_t size, const char* format, ...)
{
//#if defined( COMPILING_TINY_AXF )
    int max_size = 0;
    va_list vargs;
    va_start(vargs, format);
    max_size = vsnprintf(str, size, format, vargs);
    va_end(vargs);
    if(max_size >= size)
    {
        ddr_printf(DDR_ERROR, "ERROR: Printing to a string was truncated!\n");
        ddr_abort();
    }
//#endif
    return;
}

void ddr_external_memscpy(void *dest, size_t dst_size, const void *src, size_t copy_size)
{
    size_t min_size = 0;
    
    if(dst_size < copy_size)
    {
      min_size = dst_size;
    }
    else
    {
      min_size = copy_size;
    }
    memcpy(dest, src, min_size);
    
    if(min_size < copy_size)
    {
        ddr_printf(DDR_ERROR, "ERROR: Memory copy was truncated!\n");
        ddr_abort();
    }
    
    return;
}

/* ============================================================================
**  Function : ddr_external_set_clk_speed
** ============================================================================
*/
/*!
*   @brief
*   This function is called by external library to set the clock speed
*   
*   @details
*   This api will set the clock speed accordingly as specified in the parameter to this function
*   
*   @param 
*   @param clk_in_khz       -  [IN] clock speed in khz 
*   
*   @par Dependencies
*   
*   @par Side Effects
*   None
*   
*   @retval  boolean
*   
*   @sa None
*/

boolean ddr_external_set_clk_speed(uint32 clk_in_khz)
{
  Clock_SetBIMCSpeed(clk_in_khz);
  return TRUE;
}

/* ============================================================================
**  Function : ddr_external_get_dsf_config_buffer
** ============================================================================
*/
/*!
*   @brief
*   This function is called by external library to get an intermediate buffer
*   
*   @details
*   This function is called by external library to get an intermediate buffer
*   for reading DDR config blob values into
*   
*   @param 
*   @param buffer             -  [IN/OUT] uint32 ** buffer where we will populate 
*                                                   with pointer to buffer
*   @param size               -  [IN] uint32  Passed by DSF
*   
*   @par Dependencies
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/

void ddr_external_get_dsf_config_buffer(void **buffer, uint32 size)
{
  *buffer = (void *)(SCL_SBL1_DDR_DSF_CFG_BUF_BASE);
  if (size > SCL_SBL1_DDR_DSF_CFG_BUF_SIZE)
    return;
  memset(*buffer, 0, size);
  return;
}

/* ============================================================================
**  Function : ddr_external_dsf_config_open
** ============================================================================
*/
/*!
*   @brief
*   This function is called by external library to open DCB from XBL Config
*   
*   @details
*   This api will open the DCB binary from XBL Config and pass the handle and size of the binary
*
*   @param   
*   @param name       -  [IN]      Config item name string
*   @param handle     -  [IN/OUT]  Config item handle, if found
*   @param size       -  [IN/OUT]  Size of config item
*   
*   @par Dependencies
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/

void ddr_external_dsf_config_open (const char* name, uintnt* handle, uint32* size)
{
  CORE_VERIFY(xblconfig_open(name, (uint32 *)handle, size) == XBLCONFIG_SUCCESS);
}

/* ============================================================================
**  Function : ddr_external_dsf_config_read
** ============================================================================
*/
/*!
*   @brief
*   This function is called by external library to read a config item from XBL Config
*   
*   @details
*   This api will read data of given size, starting at given offset into a given buffer
*   
*   @param 
*   @param handle       -  [IN]      Config item handle
*   @param offset       -  [IN]      Offset from data is to be read
*   @param size         -  [IN]      Size of data to be read
*   @param buffer       -  [IN/OUT]  Buffer in which data is read
*   
*   @par Dependencies
*   Should have called ddr_external_dsf_config_open first to receive the config item handle
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/

void ddr_external_dsf_config_read (const uintnt handle, uint32 offset, uint32 size, void *buffer)
{
  CORE_VERIFY(xblconfig_read(handle, offset, size, buffer) == XBLCONFIG_SUCCESS);
}

/* ============================================================================
**  Function : ddr_external_dsf_config_close
** ============================================================================
*/
/*!
*   @brief
*   This function is called by external library to close the config item
*   
*   @details
*   This api will close the config item mentioned by the input handle
*   
*   @param 
*   @param handle       -  [IN] Config item handle 
*   
*   @par Dependencies
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/

void ddr_external_dsf_config_close (const uintnt handle)
{
  CORE_VERIFY(xblconfig_close(handle) == XBLCONFIG_SUCCESS);
}

/* ============================================================================
**  Function : ddr_mpm_config_ebi1_freeze_io
** ============================================================================
*/
/*!
*   @brief
*   This function is called by external library while entering/exiting Power Collapse
*   
*   @details
*   This api will be called by SNS library to enable/disable IO clamps during PC
*   
*   @param 
*   @param flag         -  [IN] boolean flag to enable/disable IO clamps
*   
*   @par Dependencies
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/

void ddr_mpm_config_ebi1_freeze_io( boolean flag ) 
{ 
	// if(flag)
	// {
		// HWIO_OUTF(MPM2_MPM_DDR_PHY_FREEZEIO_EBI1, DDR_PHY_FREEZEIO_EBI1, 0x1);
	// }
	// else
	// {
		// HWIO_OUTF(MPM2_MPM_DDR_PHY_FREEZEIO_EBI1, DDR_PHY_FREEZEIO_EBI1, 0x0);
	// }
}


/* ============================================================================
**  Function : ddr_external_get_buffer
** ============================================================================
*/
/*!
*   @brief
*   This function is called by external library to get a buffer for training
*   intermediate data
*   
*   @details
*   This function is called by external library to get a buffer for training
*   intermediate data
*   
*   @param 
*   @param buffer             -  [IN/OUT] uint32 ** buffer where we will populate 
*                                                   with pointer to buffer
*   @param size               -  [IN] uint32 *  we will populate with size of 
*                                                   buffer
*   
*   @par Dependencies
*   
*   @par Side Effects
*   None
*   
*   @retval  boolean
*   reutrn operation success\failure
*   
*   @sa None
*/
boolean ddr_external_get_buffer( void ** buffer, uint32 *size )
{
  *buffer = (void *)(INTERMEDIATE_TRAINING_BUF); 
  if ( *size > MAX_AVAILABLE_TEMP_TRAINING_BUFFER )
	  return FALSE ;
  memset(*buffer, 0, *size);
  return TRUE;
}

/* ============================================================================
**  Function : ddr_external_page_to_storage
** ============================================================================
*/
/*!
*   @brief
*   This function is called by external library to save ddr training logs to 
* 	external non-volatile storage
*   
*   @details
*   This function is called by external library to save ddr training logs to 
* 	external non-volatile storage. This api has to take care of offsets for 
* 	saving to external storage so we do not run out of space.
*   api will perform a memset 0 on input buffer of input size before returning
*   
*   @param 
*   @param buffer             -  [IN] uint32 * buffer where traiing logs are located
*   @param size               -  [IN] uint32  size of data in buffer
*   
*   @par Dependencies
*   
*   @par Side Effects
*   None
*   
*   @retval  void
*
*   
*   @sa None
*/
void ddr_external_page_to_storage( void * buffer, uint32 size )
{
	if(PcdGet32 (PcdBuildType) != 5)
	{
		if(buffer != NULL)
		{
			if( size != 0 && 
			(size+training_log_offset_size+training_data_offset) < DDR_MAX_LOG_SIZE)
			{
				/* call sbl1 function to save into flash storage */
				sbl1_save_ddr_training_data_to_partition(buffer, 
													   size, 
													   training_log_offset_size + training_data_offset);	

				/* increment offset for next page*/
				training_log_offset_size += size;

				/* check to see if offset is 1k aligned, if not we need to pad it */
				if(training_log_offset_size%1024 != 0)
				{
					/* divide by 1024 (round down divide), add 1 to the result and multiply by
					 * 1024 again to get the rounded up to 1k align size
					 * this is equivalent operation of doing a round up divide by 1024 then 
					 * multiply by 1024 again
					 */
					training_log_offset_size =  ((training_log_offset_size/1024)+1) * 1024;
				}
			}

			/* zero initialize buffer regardless  */
			memset(buffer, 0, size);
		}
	
	
	}
	else
	{
	 /* zero initialize buffer   */
		memset(buffer, 0, size);
	
	}

 
}

/* ============================================================================
**  Function : ddr_external_get_training_log_size
** ============================================================================
*/
/*!
*   @brief
*   This function is used to return the current stored training log size
*   
*   @details
*   This function is used to return the current stored training log size
*   
*   @param 
*   None
*   
*   @par Dependencies
*   
*   @par Side Effects
*   None
*   
*   @retval  uint32
*   size of current trainig log saved to emmc
*   
*   @sa None
*/
uint32 ddr_external_get_training_log_size(void)
{
  return training_log_offset_size;
}

boolean ddr_external_set_ddrss_cfg_clk(boolean on_off)
{
/* Clock_EnableClock & Clock_DisableClock APIs are not 
   supported in SBL1 and only supported on RPM */ 
   return TRUE;
}

void seq_wait(uint32 time_value, SEQ_TimeUnit time_unit)
  {

    if(time_unit == SEC)
    {
       busywait(time_value * 1000 * 1000);
    }
    else if(time_unit == MS)
    {
       busywait(time_value * 1000);
    }
    else if(time_unit == US)
    {
       busywait(time_value);
    }
    else
    {
       /* time_unit == NS */
       busywait(time_value / 1000);
    }

}

/* ============================================================================
**  Function : ddr_send_data_to_ddi
** ============================================================================
*/
/*!
*   @brief
*   This function is called during training to pass training results to ddi
*   
*   @details
*   This function is called during training to pass training results to ddi
*   
*   @param 
*   @param flag             -  [IN] boolean flag to enable/disable IO clamps
*   
*   @par Dependencies
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/

void ddr_send_data_to_ddi(void * buffer, uint32 size, ddr_training_args* tArgs)
{ 
  // sendDatatoDDI(buffer, size, (ddr_training_args *)tArgs, training_params_ptr); 
} 

void ddr_abort(void)
{
  /* force assert boot error */
  BL_VERIFY(0,BL_ERR_CORE_VERIFY);
}

/* ============================================================================
**  Function : save_ddi_data2ddr_partition
** ============================================================================
*/
/*!
*   @brief
*   This function is called to save ddi output to ddr partition
*   
*   @param 
*   @param[in] ddi_data_ptr - Pointer to ddi data to be written to ddr partition
*   @param[in] ddi_data_size - size of ddi data to be written, should not exceed beyond 10K alloted for DDI
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/

#if defined FEATURE_BOOT_DDI_ENABLE
void save_ddi_data2ddr_partition
(
  uint8* ddi_data_ptr,
  uint32 ddi_data_size,
  uint32 ddi_data_offfset
)
{
   BL_VERIFY(((SIZE_724KB + ddi_data_offfset + ddi_data_size) <= SIZE_1MB), BL_ERR_DDI_SIZE_INVALID|BL_ERROR_GROUP_DDR);
   sbl1_save_data_to_ddr_partition(ddi_data_ptr, ddi_data_size, SIZE_724KB + ddi_data_offfset);  	  
}
#endif //FEATURE_BOOT_DDI_ENABLE

/* ============================================================================
**  Function : ddr_external_read_ddi_binary
** ============================================================================
*/
/*!
*   @brief
*   This function is called to read DDI binary from xbl_config partition
*   Read 1024 bytes of ddi data from offset 0x4800 (18 K) of xbl_config to dcb_buffer_ptr
*   Copy the last 8 bytes of DDI which is used for DDI test input to local variable test[2]
*   
*   @param None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
#if defined FEATURE_BOOT_DDI_ENABLE
void* ddr_external_read_ddi_binary(void)
{
  uintnt dcb_handle = 0;
  uint32 dcb_size = 0;
  void* ddi_buffer_ptr;

  /* Open dcb bin in xbl Config*/
  char dcb_name[DCB_NAME_BUF_LEN_MAX] = {0};

  // Generate DCB file name
  ddr_sprintf(dcb_name, DCB_NAME_BUF_LEN_MAX, "/%04X_%04X_%01X_dcb.bin",
      ddrsns_share_data->shrm_ddr_ptr->misc.platform_id,
      (ddrsns_share_data->shrm_ddr_ptr->misc.chip_version & 0xFF00),
      ddrsns_share_data->shrm_ddr_ptr->misc.target_silicon);

  ddr_external_dsf_config_open(dcb_name, &dcb_handle, &dcb_size);

  ddr_external_get_dsf_config_buffer((void**)&(ddrsns_share_data->dcb_ptr), dcb_size);
  CORE_VERIFY(ddrsns_share_data->dcb_ptr != NULL);
  ddr_external_dsf_config_read(dcb_handle, 0, dcb_size, ddrsns_share_data->dcb_ptr);

  ddr_external_dsf_config_close(dcb_handle);

  ddi_buffer_ptr = (void*)((size_t)(ddrsns_share_data->dcb_ptr) + (*ddrsns_share_data->dcb_ptr)[DDI_INPUT_IDX][0]);
  return ddi_buffer_ptr;
}
#endif //FEATURE_BOOT_DDI_ENABLE
