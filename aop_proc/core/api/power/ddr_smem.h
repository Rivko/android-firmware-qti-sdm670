#ifndef __DDR_SMEM_H__
#define __DDR_SMEM_H__

/*=============================================================================

                                DDR SMEM PARAMETERS
                                Header File
GENERAL DESCRIPTION
This file defines the AOP parameters provided from DCB in SMEM

Copyright 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*==========================================================================
                               INCLUDE FILES
===========================================================================*/
#include "ddr_aop_params.h"

/*==============================================================================
                                  TYPES & DEFINES
==============================================================================*/
#define AOP_PARAMS_MAJOR_VERSION 0x1
#define AOP_PARAMS_MINOR_VERSION 0x0

#define DDR_AOP_SMEM_ID 0x25C   //604
#define DDR_SMEM_BASE_PHY                   0x86000000
#define DDR_SMEM_FIRST_FIXED_BUFFER_SIZE    0x40
#define DDR_SMEM_VERSION_INFO_SIZE          0x80
#define DDR_SMEM_HEAP_INFO_SIZE             0x10
#define DDR_SMEM_ALLOCATION_TABLE_SIZE      0x10

typedef struct
{
  uint16 size;
  uint16 offset;
}shared_table_header_type;

typedef enum
{
  MC_STATES = 0,
  SHUB_STATES,
  DEFAULT_ALC_POLICY,
  ALC_TIMINGS,
  MAX_NUM_TABLES,
}smem_tables;

typedef struct
{
  uint16 major_version;
  uint16 minor_version;
  shared_table_header_type       shared_table_header[MAX_NUM_TABLES];
}ddr_smem_info;

void *get_ddr_smem_struct(smem_tables table_id, uint16 *size);
FREQ_STATE *get_ddr_table(smem_tables table_id, uint8 *entries);

#endif /* __DDR_SMEM_H__ */
