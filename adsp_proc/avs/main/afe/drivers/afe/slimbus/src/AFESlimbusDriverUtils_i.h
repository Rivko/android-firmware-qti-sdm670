#ifndef __AFE_SLIMBUS_DRIVER_UTILS_I_H__
#define __AFE_SLIMBUS_DRIVER_UTILS_I_H__
/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/src/AFESlimbusDriverUtils_i.h#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFESlimbusDriverUtils_i.h

DESCRIPTION: This file should contain Slimbus driver utilities

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

  Copyright (c) 2011-2016 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Technologies, Inc. (QTI) Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/src/AFESlimbusDriverUtils_i.h#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
08-08-2010  mspk  Initial Draft

============================================================================*/
#include "qurt_elite.h"
#include "ddislimbus.h"
#include "AFEInternal.h"
#include "AFEInterface.h"
#include "AFESlimbusDriver.h"
#include "adsp_afe_service_commands.h"
#include "AFESbMultiEp.h"
/*=====================================================================
 Macros
 ======================================================================*/
// enable the SLIMBUS driver debug messages
// #define SLIMBUS_DBG_ENABLE

// 24-bit 'word' counter used for progress counters
#define MAX_DMA_WORD_TRANSFERED             0xFFFFFF
#define MAX_DESCRIPTORS_PER_STREAM          3
#define MIN_FIFO_SIZE_IN_BYTES              32 //in bytes
#define AFE_SB_PORT_CFG_WATERMARK_SIZE_1    0 //in bytes
#define AFE_SB_PORT_CFG_WATERMARK_SIZE_2    4 //in bytes
#define AFE_SLIMBUS_ROOT_FREQUENCY          24576
#define BITS_TO_BYTE_SHIFT_VALUE            3
#define SLIMBUS_EMUM_ADDR_SIZE_IN_BYTES     6
#define SLIMBUS_READ_MAX_BYTES              16

#define FIFO_WATERMARK_TIMEOUT_IN_MSEC      5   // 5 msec
#define SLIMBUS_FIFO_SLEEP_DURATION         250 // in microseconds
#define AFE_DESC_MEM_DDR                    0x0
#define AFE_DESC_MEM_LPM                    0x1

#define SLIMBUS_ALIGN_32_BYTES_MASK 0x1F     //for 32 bytes alignment
#define SLIMBUS_ALIGN(r,a) ( ((r)+((a))) & ~((uint32)(a)) )

/* Maximum number of master port count, equal to
 * maximum number of audio channels*/
#define SB_MAX_MASTER_PORTS_CNT             AFE_PORT_MAX_AUDIO_CHAN_CNT

/** @brief Enums for slimbus interface cfg versions */
typedef enum
{
  SB_CFG_V1 = 0x1
}afe_param_id_sb_cfg_minor_version;
/*=====================================================================
 Typedef's
 ======================================================================*/
typedef struct afe_slimbus_device_config_t
{
  uint32_t      afe_slimbus_id;       /**< SLIMbus device ID 	*/
  bool_t        b_is_initialized;     /**< Initialization flag  */
  uint32_t      avtimer_latch_type;   /**< SW latched or HW latched */
  bool_t        b_is_hw_support_multi_endpoint; /**< indicates if HW supports multiple endpoints */
  /**< list of maximum end points supported by master ports*/  
  uint32_t      max_eps_per_master[NUM_SB_EPS_COMB];
}afe_slimbus_device_config_t;

typedef struct afe_slimbus_driver
{
  uint32_t             afe_slimbus_id;     /**< SLIMbus device ID   */
  DalDeviceHandle      *p_core_driver;     /**< Handle to core driver */
  qurt_elite_mutex_t   sb_drv_mutex;       /**< Mutex for the core driver */
  uint32_t             dev_open_counter;   /**< Device open counter */
  qurt_elite_mutex_t   reg_seq_mutex;      /**< Mutex for the reg sequence operation */
  bool_t               is_it_multi_end_point;  /**< Indicates if Slimbus multi-end point is supported */
}afe_slimbus_driver_t;

typedef struct afe_sb_cust_desc_info
{
  uint32 virt_addr; /**< virtual address of the descriptor   */
}afe_sb_cust_desc_info_t;


typedef struct afe_sb_stream_info
{
  SlimBusResourceHandle h_master_port;
  SlimBusBamIOVecType IOVec[MAX_DESCRIPTORS_PER_STREAM];
  afe_sb_cust_desc_info_t cust_desc_info[MAX_DESCRIPTORS_PER_STREAM]; /**< Customized data for each descriptor  */
} afe_sb_master_port_info_t;

typedef struct afe_sb_cfg_info
{
  bool is_it_share_chan;
  uint32_t num_master_ports;
  uint32_t num_slave_ports;
  uint32_t num_channels;
  uint32_t sample_rate;
  int16_t bit_width;
  uint16_t bytes_per_channel;
  uint16_t slimbus_dev_id;
  uint16_t cfg_version;
  uint16_t data_format;
  SlimBusLogicalAddrType slave_dev_pgd_la;
  afe_reg_prop_t slave_dev;
  uint8_t shared_channel_mapping[AFE_PORT_MAX_AUDIO_CHAN_CNT];
  uint16_t slave_port_mapping[AFE_PORT_MAX_AUDIO_CHAN_CNT];
  cdc_reg_dev_cfg_t *slave_port_enable_reg_cfg;
  cdc_reg_dev_cfg_t *slave_port_watermark_reg_cfg;
  cdc_reg_dev_cfg_t *slave_port_multi_chan_0_reg_cfg;
  cdc_reg_dev_cfg_t *slave_port_multi_chan_1_reg_cfg;
} afe_sb_cfg_info_t;

typedef struct vfr_drift_info
{
  uint64_t prev_pg_sample_cnt;
  // previous dw progress counter: needed to handle prog count overflow.
  uint32_t prev_dw_prog_count;
  // accumulated value of dw progress counter
  uint64_t acc_dw_prog_counter;
} vfr_drift_detect_t;

typedef struct afe_slimbus_state_struct_t
{
  // high level driver specific handles
  afe_dev_port_t *afe_port_ptr;
  afe_slimbus_driver_t *p_sb_driver;

  // vfr drift info
  vfr_drift_detect_t vfr[NUM_MAX_VFR_SRC];

  // AV timer drift info
  afe_dev_avt_drift_t avt;

  // for BAM config and events
  SlimBusBamRegisterEventType bam_cfg;

  // configuration passed from apps
  afe_sb_cfg_info_t afe_sb_cfg;

  // stream related params
  uint32_t direction;
  /* BAM transfer size per channel */
  uint32_t bam_trfr_size_per_ch;
  uint16_t samples_per_dword;
  int16_t sample_rate_index;
  uint16_t num_descptrs;
  uint16_t desc_mem_type;
  SlimBusPortOptionsType pack_mode;

  afe_sb_master_port_info_t a_master_port_info[SB_MAX_MASTER_PORTS_CNT];
  SlimBusPipeConfigType pipe_cfg_info[SB_MAX_MASTER_PORTS_CNT];
  SlimBusResourceHandle h_data_channel[AFE_PORT_MAX_AUDIO_CHAN_CNT];
  /* indicates the number of channels assigned to each master ports.
   * In the case of
   *        Legacy, number of channels per port will be always 1. Whereas
   *        for multi end point, it will be greater than or equal to 1,
   *        based on master ports allocation
   * */
  uint8_t a_num_ch_per_port[SB_MAX_MASTER_PORTS_CNT];
  /* Handle to channel groups. Applicable only for multi end point
   * master ports, not used for Legacy ports.
   *
   * Each master port that supports multi end point should
   * contain a unique channel group   */
  SlimBusResourceHandle a_h_multi_ep_chan_group[SB_MAX_MASTER_PORTS_CNT];
  SlimBusResourceHandle h_chan_group;
  SlimBusResourceHandle h_progress_ctr;
  DALSYSEventHandle master_port_event;

  uint16_t port_error_cnt;

  // progress counter (double word) overflow handling:for get_time
  // previous dw progress counter
  uint32_t prev_dw_prog_count;
  // accumulated value of dw progress counter
  uint64_t acc_dw_prog_counter;
  // previous dw progress counter: for avt based afe.
  uint32_t avt_afe_prev_dw_prog_count;
  // accumulated value of dw progress counter
  uint64_t avt_afe_acc_dw_prog_counter;
  // mutex for above counters
  qurt_elite_mutex_t prog_count_mutex;
  bool_t  device_open_success;
  //SW latched timestamp read in BAM interrupt context
  uint64_t sw_latched_timestamp;
  //SW latched device time read in BAM interrupt context
  uint64_t sw_latched_dev_time;
} afe_slimbus_state_struct_t;
/*=====================================================================
 Functions
 ======================================================================*/
void*  afe_dal_bam_callback(void *cbCtxt, uint32 uSize1, void *pBuf2, uint32 uSize2);
void*  afe_dal_port_callback(void *cbCtxt, uint32 uSize1, void *pBuf2, uint32 uSize2);

ADSPResult afe_sb_open_stream_ports(afe_slimbus_state_struct_t *p_dev_state);
ADSPResult afe_sb_close_stream_ports(afe_slimbus_state_struct_t *p_dev_state);
ADSPResult afe_sb_open_data_channels(afe_slimbus_state_struct_t *p_dev_state);
ADSPResult afe_sb_close_data_channels(afe_slimbus_state_struct_t *p_dev_state);
ADSPResult afe_sb_set_data_format(afe_slimbus_state_struct_t *p_dev_state);
ADSPResult afe_sb_get_slimbus_cfg(afe_sb_cfg_info_t *p_afe_sb_cfg, void *cfg, uint16_t cfg_payload_size, uint16_t cfg_version);
ADSPResult afe_slimbus_dev_cfg_init(void);
#endif /*#ifndef __AFE_SLIMBUS_DRIVER_UTILS_I_H__*/
