/*========================================================================
This file contains function header for the AFE Spdif internal driver

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/spdif/src/AFESpdifDriver_i.h#1 $
 ====================================================================== */


#ifndef _AFE_SPDIF_DRIVER_I_H_
#define _AFE_SPDIF_DRIVER_I_H_


#include "qurt_elite.h"
#include "AFEInternal.h"
//#include "AFEDalDeviceDriver.h"
#include "lpasshwio_devcfg.h"
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DalDevice.h"
#include "DALSysTypes.h"
#include "DALPropDef.h"
#include "DDIChipInfo.h"
#include "halspdif.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum
{
   SPDIF_STATE_NOINIT = 0,
   SPDIF_STATE_INIT   = 1,
   SPDIF_STATE_STOP   = 2,
   SPDIF_STATE_CONFIG = 3,
   SPDIF_STATE_START  = 4,
} spdif_driver_state_t;

typedef enum
{  
   SPDIF_NONE    = 0,
   SPDIF_VERSION_V1 = 0x10000000, // gandalf
} spdif_version_t;

typedef struct _afe_spdif_hw_buf_info
{
    uint32 *     hw_buf_phys_addr;
    uint32 *     hw_buf_virt_addr;
    uint32       hw_buf_length_per_int;       ///< in dword
    uint32       num_samples_per_int;      
    uint32       hw_buf_total_length;         ///< in dword
    uint32       num_sub_buf;
    uint32       allocInDDRFlag; 
} afe_spdif_hw_buf_info_t;

typedef struct _afe_spdif_intstat
{
   uint32 tx_block_done_int;
   uint32 fifo_uflow_int;
   uint32 ts_diff_int;
   uint32 mc_err_int;
} afe_spdif_intstat_t;

typedef struct _afe_spdif_statistics
{
   uint32 prev_num_frame;
   uint32 prev_num_block;
   uint32 rollover_cnt;
} afe_spdif_statistics_t;

typedef struct afe_spdif_driver_struct
{
   spdif_driver_state_t       state;               /* spdif driver state */
   afe_dev_port            *dev_port_ptr;

   lpasshwio_prop_spdif_struct_t  spdiftx_hw_info; /* spdif hardware information from device config */

   uint32                  spdif_virtual_addr;     /* spdif main register virtual address mapping */
   qurt_mem_region_t       spdif_qmem_region;      /* spdif register mapping region */
   uint32                  spdif_reset_virtual_addr;/* spdif soft reset register virtual address mapping */
   qurt_mem_region_t       spdif_reset_qmem_region; /* spdif register mapping region */

   
   afe_spdif_hw_buf_info_t spdif_hw_buf_info;      /* hardware buffer info */

   afe_spdif_intstat_t int_stat;                   /* interrupt statistics*/

   afe_spdif_statistics_t transfer_stat;           /* data transfer statistics */

   uint32 buffer_index;                            /* current index of ring buffer */

   uint64 tsdiff;                                  /* time stamp of the last transmitted sub buffer */

   uint64 start_timestamp;                         /* start avtimer timestamp */

} afe_spdif_driver_t;


typedef struct spdif_state_struct
{
	uint32_t           int_samples_per_period;
							/**< Number of samples per interrupt*/                             

   uint32_t                  spdif_cfg_minor_version;
   /**< Minor version used for tracking HDMI module configuration.

      @values #AFE_API_VERSION_HDMI_CONFIG */

   uint16_t                  data_format;
   /**< Supported data type.

     @values
      - #AFE_LINEAR_PCM_DATA
      - #AFE_NON_LINEAR_DATA*/

   uint32_t                  sample_rate;
   /**< Sampling rate of the port.

        @values
         - #AFE_PORT_SAMPLE_RATE_22_05K
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_44_1K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_176_4K
         - #AFE_PORT_SAMPLE_RATE_192K*/

   uint16_t                  num_channels;
   /**< Number of channels.
   
        @values 1 to 2 */


   uint16_t                  bit_width;
   /**< Bit width of the sample.

        @values 16, 24 */

   uint32_t                  channel_status_type;
   /*** < indicate if the channel status is for channel A or B

        @values
        - #AFE_CHSTATUS_A
        - #AFE_CHSTATUS_B @tablebulletend */

   spdif_ch_sts_buf_t tx_ch_sts_buf_a; // channel status buffer a

   spdif_ch_sts_buf_t tx_ch_sts_buf_b; // channel status buffer b.

} spdif_state_struct_t;

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // !defined(_AFE_SPDIF_DRIVER_I_H_)

