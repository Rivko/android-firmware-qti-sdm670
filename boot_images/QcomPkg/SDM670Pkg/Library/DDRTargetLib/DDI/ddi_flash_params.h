/** 
  @file  ddi_flash_params.h
  @brief defines DDI parameters stored in flash
*/
/*=============================================================================

                                DDI Flash PARAMETERS
                                Header File
GENERAL DESCRIPTION
This file defines the DDI parameters stored in DDR Flash partition

  Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary.
===============================================================================

===============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/19/17   na      Initial version.
==============================================================================*/
#ifndef __DDI_FLASH_PARAMS_H__
#define __DDI_FLASH_PARAMS_H__

/*==============================================================================
                               INCLUDES
==============================================================================*/
#include "HALcomdef.h"
#include "ddr_aop_params.h"
#include "ddrss_training.h"

/*==============================================================================
                                  TYPES & DEFINES
==============================================================================*/

/******************************************************/
/*** DDR Parameter partition datastructure ***/
/******************************************************/

#define DDI_MAJOR_VERSION 1
#define DDI_MINOR_VERSION 0

#define MAX_DDI_OUTPUT_SIZE    (80 * 1024) /*10KB*/
#define DDI_INT_OUT_BUF_SIZE 50

// cookie written to beginning of flash partition
#define DDI_CMDP_COOKIE 0xC001FACE

#define member_size(type, member) sizeof(((type *)0)->member)

typedef enum {
  DDI_CMD_NOT_COMPLETE = 0,
  DDI_CMD_COMPLETE = 1,
  DDI_FAIL = 2,
  DDI_INVALID_INPUT = 4
} ddi_error;

typedef enum {
  DDI_MEM_OP_RD,
  DDI_MEM_OP_WR_CMP, // it is cmp if cmp_params is defined, else wr
} ddi_mem_op_type;

typedef struct __attribute__((packed)) ddi_rd_params
{
  uint64 start_addr;
  uint64 end_addr;
  uint64 repeat_count; // 0 means infinite
} ddi_rd_params;

#define DDI_WR_FLAGS_MASK_RANDOM 0x1
#define DDI_WR_FLAGS_SHIFT_RANDOM 0
#define DDI_WR_FLAGS_MASK_INVERT_ROW 0x2
#define DDI_WR_FLAGS_SHIFT_INVERT_ROW 1
typedef struct __attribute__((packed)) ddi_wr_params
{
  uint64 start_addr;
  uint64 end_addr;
  uint64 repeat_count; // 0 means infinite
  uint8  flags;
  uint32 random_seed;
  uint64 random_stride;
  uint8 padding[3];
  uint64 pattern_size;
  // followed by pattern
} ddi_wr_params;

#define DDI_RDWR_FLAGS_MASK_VERIFY 0x1
#define DDI_RDWR_FLAGS_SHIFT_VERIFY 0
#define DDI_RDWR_FLAGS_MASK_RANDOM 0x2
#define DDI_RDWR_FLAGS_SHIFT_RANDOM 1
#define DDI_RDWR_FLAGS_MASK_INVERT_ROW 0x4
#define DDI_RDWR_FLAGS_SHIFT_INVERT_ROW 2
typedef struct __attribute__((packed)) ddi_rdwr_params
{
  uint64 start_addr;
  uint64 end_addr;
  uint64 repeat_count; // 0 means infinite
  uint8  flags; // bit 0: verify, bit 1: random
  uint32 random_seed;
  uint64 random_stride;
  uint8 padding[3];
  uint64 pattern_size;
  // followed by pattern
} ddi_rdwr_params;

#define DDI_RET_FLAGS_MASK_FIND 0x1
#define DDI_RET_FLAGS_SHIFT_FIND 0
#define DDI_RET_FLAGS_MASK_RANDOM 0x2
#define DDI_RET_FLAGS_SHIFT_RANDOM 1
#define DDI_RET_FLAGS_MASK_SR_VALIDATE 0x4
#define DDI_RET_FLAGS_SHIFT_SR_VALIDATE 2
#define DDI_RET_FLAGS_MASK_INVERT_ROW 0x8
#define DDI_RET_FLAGS_SHIFT_INVERT_ROW 3
typedef struct __attribute__((packed)) ddi_ret_params
{
  uint64 start_addr;
  uint64 end_addr;
  uint8  flags; // bit 0: find, bit 1: random, bit 2: SR validate
  uint16 min_fails; // if min_fails is non-zero, delay and repeat will be ignored.
  uint32 delay_ms;
  uint32 random_seed;
  uint64 random_stride;
  uint8 padding[5];
  uint64 pattern_size;
  // followed by pattern
} ddi_ret_params;

typedef struct __attribute__((packed)) ddi_delay_params
{
  uint32 delay_ms;
  uint8 padding[4];
} ddi_delay_params;

typedef struct __attribute__((packed)) ddi_freq_params
{
  uint32 freq_khz;
  uint8 padding[4];
} ddi_freq_params;

#define DDI_PLOT_RD 0
#define DDI_PLOT_WR 1
typedef struct __attribute__((packed)) ddi_plot_params
{
  uint8 type;
  uint32 freq_khz;
  uint8 padding[3];
} ddi_plot_params;

#define DDI_PARAM_TYPE_RD 0
#define DDI_PARAM_TYPE_WR 1
typedef struct __attribute__((packed)) ddi_mod_phy_offset_params
{
  uint8 type;
  uint8 cs_mask;
  uint8 channel_mask;
  uint8 dq_mask;
  uint32 freq_khz;
  int32 cdc_offset;
  int32 vref_offset;
} ddi_mod_phy_offset_params;

typedef struct __attribute__((packed)) ddi_command_header
{
  uint64 cmd_id;
  // followed by command params_header
} ddi_command_header;

#define DDI_BOOT_ID_EDL 0
#define DDI_BOOT_ID_HLOS 1
#define DDI_BOOT_ID_UEFI 2
#define DDI_BOOT_ID_REPEAT 3
typedef struct __attribute__((packed)) ddi_commands_header
{
  uint32 ddi_version;
  uint8  boot_id;
  uint32 num_cmds;
  uint8 padding[3];
  // followed by command_header
} ddi_commands_header;

typedef struct __attribute__((packed)) ddi_fail_summary
{
  uint32 total_fail_count;
  uint8 num_ranks;
  uint32 rank[NUM_CS];
  uint8 num_channels;
  uint32 channel[NUM_CH];
  uint32 num_fail_records;
} ddi_fail_summary;

typedef struct ddi_handle_fail_params
{
  ddi_fail_summary* fail_summary;
  uint32 max_fails;
  uint32 delay; // only used for retention test
} ddi_handle_fail_params;

typedef struct ddi_cmp_params {
  ddi_handle_fail_params* handle_fail_params;
  boolean (*handle_fail)(ddi_handle_fail_params*, uint64*, uint64, uint64, uint64*);
  uint64* output_iter;
} ddi_cmp_params;

typedef struct __attribute__((packed)) ddi_fail_addr
{
  uint64 fail_addr;
  uint64 expected_data;
  uint64 read_data;
} ddi_fail_addr;

typedef struct __attribute__((packed)) ddi_ret_fail_addr
{
  uint32 delay;
  ddi_fail_addr fail_addr;
} ddi_ret_fail_addr;

typedef struct __attribute__((packed)) ddi_output_header
{
  uint64 cmd_id;
  uint8 test_status;
} ddi_output_header;

typedef struct __attribute__((packed)) ddi_rd_output
{
} ddi_rd_output;

typedef struct __attribute__((packed)) ddi_wr_output
{
} ddi_wr_output;

typedef struct __attribute__((packed)) ddi_rdwr_output
{
  ddi_fail_summary fail_summary;
  // followed by array of failing data packets  
} ddi_rdwr_output;

typedef struct __attribute__((packed)) ddi_ret_output
{
  ddi_fail_summary fail_summary;
  // followed by array of failing retention data packets
} ddi_ret_output;

typedef struct __attribute__((packed)) ddi_delay_output
{
} ddi_delay_output;

typedef struct __attribute__((packed)) ddi_freq_output
{
} ddi_freq_output;

typedef struct __attribute__((packed)) ddi_plot_output
{
} ddi_plot_output;

typedef struct __attribute__((packed)) ddi_device_info
{
	uint32 chip_id;
	uint32 chip_sn;
	uint32 dsf_version;
	uint32 shrm_dsf_version;
	uint8 dram_manufacturer_id;
	uint8 dram_device_type;
	uint16 dram_rank0_MB;
	uint16 dram_rank1_MB;
} ddi_device_info;

typedef struct __attribute__((packed)) ddi_out_ecdt_settings
{
  uint32 freq_khz;
  uint8 number_settings;
  uint8 settings_buf;
} ddi_out_ecdt_settings;

#endif /* __DDI_FLASH_PARAMS_H__ */
