/*
@file dsc_priv.h
@brief DSC -> DSP Shared Channel
*/
/*===========================================================================
  Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/apps.tz/2.0.2/securemsm/trustzone/qsapps/secdsplib/dsc_priv.h#1 $
  $DateTime: 2018/08/09 06:10:01 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
===========================================================================*/

#ifndef __DSC_PRIV_H__
#define __DSC_PRIV_H__

#include "object.h"
#include "stdint.h"

#define MAX_BUFS        30

/* TO-DO: Store the list of memory objects also */
struct dsc_feat_priv {
    uint32_t feat_id;
    uint64_t dsc_addr;
    uint64_t dsc_len;
    Object dsc_region;
    Object dsc_map;
};

typedef struct feat_buffer_struct {
    uint32_t type;
    uint32_t size;
    uint64_t addr;
} __attribute__ ((packed)) feat_buf_t;

typedef struct feat_config_struct {
    uint32_t version;
    uint32_t num_buffers;
    uint32_t state;
    uint32_t params;
    feat_buf_t white_list[MAX_BUFS];
} __attribute__ ((packed)) feat_config_t;

typedef struct channel_rw_config_struct {
    feat_config_t rd_config;
    feat_config_t wr_config;
    size_t len;
} channel_rw_config;

#endif // __DSC_PRIV_H__
