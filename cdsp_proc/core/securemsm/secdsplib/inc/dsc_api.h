/*
@file dsc_api.h
@brief DSC -> DSP Shared Channel
*/
/*===========================================================================
  Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/core.qdsp6/2.1.c4/securemsm/secdsplib/inc/dsc_api.h#2 $
  $DateTime: 2018/08/30 07:36:57 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
===========================================================================*/

#ifndef __DSC_API_H__
#define __DSC_API_H__

#include "stddef.h"
#include "stdint.h"

typedef enum dsc_errors
{
    DSC_ERROR = 1000,       //Default error code
    DSC_OOM_ERROR,          //Out of Memory code
    DSC_AC_ERROR,           //Access Control Violation
    DSC_RULE_ERROR,         //Access Control Rule updation failure
    DSC_STATE_ERROR,        //Buffer state udpate failure
    DSC_OOB_ERROR,          //Out of buffers in secure channel
    DSC_BUFFER_ERROR,       //Buffers shared is not valid
    DSC_CONFIG_ERROR,       //Secure channel configure failure
    DSC_UNKNOWN_ERROR,      //Generic error for most of the condition failures
    DSC_CHANNEL_ERROR,      //Secure channel corruption error
    DSC_READ_ERROR,         //DSC get configuration  failure
    DSC_WRITE_ERROR,        //DSC set configuration  failure
} dsc_errors;

//ION buffers shared in shared channel, these states are usecase defined
typedef enum dsc_buffer_type {
    MODEL = 1,
    AUTH = 2,
    CAMERA = 3,
} dsc_buf_type_t;

// Encapsulated structure serving as a handle
typedef struct dsc_feat_priv dsc_feat_handle_t;

int32_t dsc_get_feat_handle(dsc_feat_handle_t **out_handle);

void dsc_release_feat_handle(dsc_feat_handle_t *handle);

int32_t dsc_verify_buffer(dsc_feat_handle_t *handle, uint32_t buf_type, uint64_t addr, size_t size);

int32_t dsc_get_state(dsc_feat_handle_t *handle, uint32_t *cur_state);

int32_t dsc_set_state(dsc_feat_handle_t *handle, uint32_t new_state);

#endif // __DSC_API_H__
