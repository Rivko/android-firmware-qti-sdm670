/**
    @file  fw_devcfg.h
    @brief device firmware interface
 */
/*=============================================================================
          Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                            All rights reserved.
            Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "comdef.h"
#include "qup_common.h"

typedef enum se_mode
{
    NONE,
    GSI,
    FIFO,
    CPU_DMA,
    MIXED,

} se_mode;

typedef struct se_cfg
{
    uint32      offset;
    uint32      protocol;
    se_mode     mode;
    boolean     load_fw;
    boolean     dfs_mode;

} se_cfg;

typedef struct qup_cfg
{
    uint8      *qup_base;
    uint32      common_offset;
    boolean     load_fw;

} qup_cfg;

typedef struct fw_type
{
    void    *start;
    uint32   size;

} fw_type;
