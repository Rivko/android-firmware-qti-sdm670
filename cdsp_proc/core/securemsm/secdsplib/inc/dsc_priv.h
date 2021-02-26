/*
@file dsc_priv.h
@brief DSC -> DSP Shared Channel
*/
/*===========================================================================
  Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/core.qdsp6/2.1.c4/securemsm/secdsplib/inc/dsc_priv.h#2 $
  $DateTime: 2018/08/30 07:36:57 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
===========================================================================*/

#ifndef __DSC_PRIV_H__
#define __DSC_PRIV_H__

#include "stdint.h"

#define MAX_BUFS        30

struct dsc_feat_priv {
    uint64_t dsc_addr;
    uint64_t dsc_len;
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

#endif // __DSC_PRIV_H__
