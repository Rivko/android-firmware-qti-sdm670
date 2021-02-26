/*
@file dsc_api.h
@brief DSC -> DSP Shared Channel
*/
/*===========================================================================
  Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/apps.tz/2.0.2/securemsm/trustzone/qsapps/secdsplib/dsc_api.h#1 $
  $DateTime: 2018/08/09 06:10:01 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
===========================================================================*/

#ifndef __DSC_API_H__
#define __DSC_API_H__

#include "qsee_access_control.h"
#include "stdint.h"

typedef enum dsc_errors
{
    DSC_ERROR = 1000,
    DSC_OOM_ERROR,
    DSC_AC_ERROR,
    DSC_RULE_ERROR,
    DSC_STATE_ERROR,
    DSC_OOB_ERROR,
    DSC_BUFFER_ERROR,
    DSC_CONFIG_ERROR,
    DSC_UNKNOWN_ERROR,
    DSC_CHANNEL_ERROR,
    DSC_READ_ERROR,
    // This error will most likely be caused by state change by DSP,
    // Client should re-try the operation
    DSC_WRITE_ERROR,
} dsc_errors;

// Encapsulated structure serving as a handle
typedef struct dsc_feat_priv dsc_feat_handle_t;

int32_t dsc_get_feat_handle(
        uint32_t feat_id, dsc_feat_handle_t **out_handle);

void dsc_release_feat_handle(dsc_feat_handle_t *handle);

int32_t dsc_register_buffer(
      dsc_feat_handle_t *handle, uint32_t buf_type, uint64_t addr, size_t size,
      ACVirtualMachineId *vm_list, uint32_t nr_vm_list);

int32_t dsc_deregister_buffer(dsc_feat_handle_t *handle, uint64_t addr, size_t size);

int32_t dsc_verify_buffer(dsc_feat_handle_t *handle, uint64_t addr, size_t size);

int32_t dsc_get_state(dsc_feat_handle_t *handle, uint32_t *cur_state);

int32_t dsc_set_state(dsc_feat_handle_t *handle, uint32_t new_state);

#endif // __DSC_API_H__
