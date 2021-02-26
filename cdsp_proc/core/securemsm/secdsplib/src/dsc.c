/*
@file dsc.c
@brief DSC -> DSP Shared Channel
*/
/*===========================================================================
  Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/core.qdsp6/2.1.c4/securemsm/secdsplib/src/dsc.c#2 $
  $DateTime: 2018/08/30 07:36:57 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
===========================================================================*/

//#include "CDSPSharedChannel.h"
#include "dsc_api.h"
#include "dsc_priv.h"
#include "dsc_utils.h"
//#include "qsee_env.h"
//#include "qsee_heap.h"
#include <stdlib.h>
#include "stdbool.h"
#include "stringl.h"
#include "task_util.h"

#define VERSION     1
#define SUCCESS     0

typedef struct dsp_shared_channel_struct {
    uint8 tz_lock;
    uint8 dsp_lock;
    int32_t turn;
    feat_config_t feat_config;
} __attribute__ ((packed)) dsc_t; // dsp_shared_channel

/* TZ's lock/unlock is as follows:
static void atomic_lock(dsc_t *channel)
{
    // Peterson's algorithm
    channel->tz_lock = true;
    channel->turn = 1;
    while (channel->dsp_lock == true && channel->turn == 1) {
        // busy wait
    }
}

static void atomic_unlock(dsc_t *channel)
{
    channel->tz_lock = false;
}
*/

/* DSP's lock/unlock is as follows: */
static void atomic_lock(dsc_t *channel)
{
    // Peterson's algorithm
    channel->dsp_lock = true;
    channel->turn = 0;
    while (channel->tz_lock == true && channel->turn == 0) {
        // busy wait
    }
}

static void atomic_unlock(dsc_t *channel)
{
    channel->dsp_lock = false;
}


static int32_t dsc_set_feat_config(dsc_t *channel, feat_config_t *config)
{
    int32_t ret = SUCCESS;
    size_t num_bytes = 0;

    num_bytes = memscpy(
            &(channel->feat_config), sizeof(feat_config_t), config, sizeof(feat_config_t));

    CHECK_COND((num_bytes != sizeof(feat_config_t)), ret, DSC_WRITE_ERROR,
            "Could not write feat config in DSP Shared Channel");

end:
    return ret;
}

static int32_t dsc_get_feat_config(dsc_t *channel, feat_config_t *config)
{
    int32_t ret = SUCCESS;
    size_t num_bytes = 0;

    num_bytes = memscpy(
            config, sizeof(feat_config_t), &(channel->feat_config), sizeof(feat_config_t));

    CHECK_COND((num_bytes != sizeof(feat_config_t)), ret, DSC_READ_ERROR,
            "Could not read feat config from DSP Shared Channel");

end:
    return ret;
}

static int32_t dsc_find_buffer(feat_config_t config, feat_buf_t buf)
{
    int32_t i = 0;

    for (i = 0; i < MAX_BUFS; i++) {

        if ((buf.size == config.white_list[i].size) &&
            (buf.addr == config.white_list[i].addr) &&
            (buf.type == config.white_list[i].type)) {

            // Buffer registered
            return SUCCESS;
        }
    }

    return DSC_BUFFER_ERROR;
}

static int32_t dsc_buffer_ops(dsc_feat_handle_t *handle,
        uint32_t buf_type, uint64_t addr, size_t size)
{
    int32_t ret = SUCCESS;
    feat_config_t config;
    feat_buf_t reg_buf;
    dsc_t *channel = (dsc_t *)handle->dsc_addr;
    atomic_lock(channel);

    reg_buf.type = buf_type;
    reg_buf.size = size;
    reg_buf.addr = addr;

    CHECK_N_CALL(ret, dsc_get_feat_config(channel, &config));

    /* Fill the type of buffer from config if available */
    CHECK_N_CALL(ret, dsc_find_buffer(config, reg_buf));

end:
    atomic_unlock(channel);
    return ret;
}

static int32_t dsc_state_ops(
        dsc_feat_handle_t *handle, uint32_t *cur_state, uint32_t new_state)
{
    int32_t ret = SUCCESS;
    feat_config_t config;
    dsc_t *channel = (dsc_t *)handle->dsc_addr;
    atomic_lock(channel);

    CHECK_N_CALL(ret, dsc_get_feat_config(channel, &config));

    if (cur_state) {
        *cur_state = config.state;
        return SUCCESS;
    }

    config.state = new_state;
    CHECK_N_CALL(ret, dsc_set_feat_config(channel, &config));

end:
    atomic_unlock(channel);
    return ret;
}

int32_t dsc_get_feat_handle(dsc_feat_handle_t **out_handle)
{
    int32_t ret = SUCCESS;
    feat_config_t config;
    dsc_feat_handle_t *handle;
    dsc_t *channel = NULL;

    handle = (dsc_feat_handle_t *)malloc(sizeof(dsc_feat_handle_t));
    CHECK_COND((handle == NULL), ret, DSC_OOM_ERROR, "Out of memory");

    //TO-DO: Get DSC Address
    CHECK_N_CALL(ret, memory_mapping((unsigned int *)&handle->dsc_addr, 0x862dc000, 0x1000));
    handle->dsc_len = 0x1000;
    //TO-DO: Map DSC Address

    CHECK_COND(handle->dsc_len < sizeof(dsc_t), ret, DSC_CHANNEL_ERROR,
            "Channel is too small, expected size : %d, got : %d",
            sizeof(dsc_t), handle->dsc_len);

    channel = (dsc_t *)handle->dsc_addr;
    CHECK_N_CALL(ret, dsc_get_feat_config(channel, &config));

    CHECK_COND(config.version != VERSION, ret, DSC_UNKNOWN_ERROR,
            "Feature was not reset properly");

    *out_handle = handle;

end:
    if (ret) {
        dsc_release_feat_handle(handle);
    }
    return ret;
}

void dsc_release_feat_handle(dsc_feat_handle_t *handle)
{
    dsc_t *channel = NULL;

    if (!handle) {
        return;
    }

    if (!handle->dsc_addr) {
        goto end;
    }

    channel = (dsc_t *)handle->dsc_addr;

    //TO-DO: Unmap channel address
end:
    free(handle);
    return;
}

int32_t dsc_verify_buffer(dsc_feat_handle_t *handle, uint32_t buf_type,
        uint64_t addr, size_t size)
{
    return dsc_buffer_ops(handle, buf_type, addr, size);
}

int32_t dsc_get_state(dsc_feat_handle_t *handle, uint32_t *cur_state)
{
    return dsc_state_ops(handle, cur_state, 0);
}

int32_t dsc_set_state(dsc_feat_handle_t *handle, uint32_t new_state)
{
    return dsc_state_ops(handle, NULL, new_state);
}
