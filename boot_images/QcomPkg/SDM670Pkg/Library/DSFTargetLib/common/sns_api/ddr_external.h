#ifndef __DDR_EXTERNAL_H__
#define __DDR_EXTERNAL_H__

/*=============================================================================

                                DDR EXTERNAL
                                Header File
GENERAL DESCRIPTION
 This file defines the external environment APIs that DDR HAL layers may need to invoke


Copyright (c) 2014-2017, Qualcomm Technologies Incorporated. All rights reserved.
=============================================================================*/

/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/01/13   dp      Initial version.
==============================================================================*/
/*==========================================================================
                               INCLUDE FILES
===========================================================================*/
#include "HALcomdef.h"
#include "seq_hwio.h"

/*==============================================================================
                                  TYPES
==============================================================================*/

// enum{
    // DDR_SOC_SDM845_V1 = 0x0   //To-do: Fill in the correct SOC value here
// };

typedef enum 
{	
  DDR_TRAINING_READ,	
  DDR_TRAINING_WRITE, 
  DDR_TRAINING_CACK
}ddr_training_type;

typedef struct 
{
  uint8 currentChannel;
  uint8 currentChipSelect;
  uint8 currentFreqIndex;
  uint8 best_cdc_value;
  uint8 best_vref_value;
  uint32 FreqInKhz;
  ddr_training_type trainingType;	
}ddr_training_args;

/*==============================================================================
                                  MACROS
==============================================================================*/
/* Macro for round-up division */
#define div_ceil(n, d)  (((n) + (d) - 1) / (d))

/*==============================================================================
                                  FUNCTION PROTOTYPES
==============================================================================*/

#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED
  boolean ddr_external_set_clk_speed(uint32 clk_in_khz);
#else
  static inline boolean ddr_external_set_clk_speed(uint32 clk_in_khz)
  {
    return TRUE;
  }
#endif



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
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED 
  void ddr_mpm_config_ebi1_freeze_io( boolean flag ); 
#else 
  static inline void ddr_mpm_config_ebi1_freeze_io( boolean flag ) 
  { 
    return; 
  } 
#endif


/* ============================================================================
**  Function : ddr_mpm_config_ebi1_freeze_io_sdi
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
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED 
  void ddr_mpm_config_ebi1_freeze_io_sdi( boolean flag ); 
#else 
  static inline void ddr_mpm_config_ebi1_freeze_io_sdi( boolean flag ) 
  { 
    return; 
  } 
#endif


/* ============================================================================
**  Function : seq_wait
** ============================================================================
*/
/*!
*   @brief
*   This function is called by external library to introduce wait/delay
*   
*   @details
*   This api will call the relevant busywait function to introduce needed delay.
*   
*   @param 
*   @param time_value       -  [IN] delay time .
*   @param time_unit        -  [IN] time in units (SEC/MS/US/NS etc)
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
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED
  void seq_wait(uint32 time_value, SEQ_TimeUnit time_unit);
#else
  static inline void seq_wait(uint32 time_value, SEQ_TimeUnit time_unit)
  {
    return;
  } 
#endif

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
*   @param size               -  [IN/OUT] uint32 *  we will populate with size of 
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
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED 
  boolean ddr_external_get_buffer(void **buffer, uint32 *size);
#else
  static inline boolean ddr_external_get_buffer(void **buffer, uint32 *size)
  {
    return TRUE;
  } 
#endif

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
*   @param size               -  [IN] uint32 * size of data in buffer
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

#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED 
void ddr_external_page_to_storage( void * buffer, uint32 size );
#else
  static inline void ddr_external_page_to_storage( void * buffer, uint32 size )
  {
    return;
  } 
#endif

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
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED 
uint32 ddr_external_get_training_log_size(void);
#else
  static inline uint32 ddr_external_get_training_log_size(void)
  {
    return 0;
  } 
#endif

/* ============================================================================
**  Function : ddr_external_set_ddrss_cfg_clk
** ============================================================================
*/
/*!
*   @brief
*   This function is called by external library to set( ON/OFF ) the GCC_DDR_DIM_CFG
*   
*   @details
*   This function is called by external library to set( ON/OFF ) the GCC_DDR_DIM_CFG
*   
*   @param 
*   @param boolean            -  [IN] boolean flag to enable/disable GCC_DDR_DIM_CFG clock
*                                                   
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

#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED 
  boolean ddr_external_set_ddrss_cfg_clk(boolean on_off);
#else
  static inline boolean ddr_external_set_ddrss_cfg_clk(boolean on_off)
  {
    return TRUE;
  } 
#endif

/* ============================================================================
**  Function : ddr_abort(void)
** ============================================================================
*/
/*!
*   @brief
*   This function is called by external library to abort if a small eye is found.
*   
*   @details
*   This function is called by external library to abort if a small eye is found.
*   
*   @param 
*   @param boolean            -  [IN] boolean flag to enable/disable GCC_DDR_DIM_CFG clock
*                                                   
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

#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED 
  void ddr_abort(void);
#else
  static inline void ddr_abort(void)
  {
    return;
  } 
#endif

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
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED
  void ddr_send_data_to_ddi(void * buffer, uint32 size, ddr_training_args* tArgs);
#else 
  static inline void ddr_send_data_to_ddi(void * buffer, uint32 size, ddr_training_args* tArgs)
  { 
    return; 
  } 
#endif

#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED 
  void ddr_external_get_dsf_config_buffer(void **buffer, uint32 size); 
#else 
  static inline void ddr_external_get_dsf_config_buffer(void **buffer, uint32 size) 
  { 
    *buffer = (void*)0xBAD0BAD0;
    return; 
  } 
#endif
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED 
  void ddr_external_dsf_config_open(const char* name, uintnt* handle, uint32* size); 
#else 
  static inline void ddr_external_dsf_config_open(const char* name, uintnt* handle, uint32* size) 
  { 
    return; 
  } 
#endif
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED 
  void ddr_external_dsf_config_read(const uintnt handle, uint32 offset, uint32 size, void *buffer); 
#else 
  static inline void ddr_external_dsf_config_read(const uintnt handle, uint32 offset, uint32 size, void *buffer) 
  { 
    return; 
  } 
#endif
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED 
  void ddr_external_dsf_config_close(const uintnt handle); 
#else 
  static inline void ddr_external_dsf_config_close(const uintnt handle) 
  { 
    return; 
  } 
#endif


#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED 
  void pxi_gpio_trigger(uint8 ch); 
#else 
  static inline void pxi_gpio_trigger(uint8 ch)
  { 
    return; 
  } 
#endif

#ifndef DSF_COMPILING_SHRM
#if defined FEATURE_BOOT_EXTERN_DDR_COMPLETED && defined FEATURE_BOOT_DDI_ENABLE
  void* ddr_external_read_ddi_binary(); 
#else 
  static void* ddr_external_read_ddi_binary()
  { 
    return NULL; 
  } 
#endif

#if defined FEATURE_BOOT_EXTERN_DDR_COMPLETED && defined FEATURE_BOOT_DDI_ENABLE
  void save_ddi_data2ddr_partition
  (
    uint8* ddi_data_ptr,
    uint32 ddi_data_size,
    uint32 ddi_data_offfset
  );
#else 
  static void save_ddi_data2ddr_partition
  (
    uint8* ddi_data_ptr,
    uint32 ddi_data_size,
    uint32 ddi_data_offfset
  )
  {
    return;
  }
#endif
#endif

#if defined FEATURE_BOOT_EXTERN_DDR_COMPLETED
  void ddr_external_snprintf(char* str, size_t size, const char* format, ...);
#else
  static inline void ddr_external_snprintf(char* str, size_t size, const char* format, ...)
  {
    return;
  }
#endif

#if defined FEATURE_BOOT_EXTERN_DDR_COMPLETED
  void ddr_external_memscpy(void *dest, size_t dst_size, const void *src, size_t copy_size);
#else
  static inline void ddr_external_memscpy(void *dest, size_t dst_size, const void *src, size_t copy_size)
  {
    return;
  }
#endif

#endif /* __DDR_EXTERNAL_H__ */
