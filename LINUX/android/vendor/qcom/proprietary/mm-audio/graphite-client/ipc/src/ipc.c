/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <osal_log.h>
#include <osal_errorcodes.h>
#include <osal_list.h>
#include <osal_mem.h>
#include <osal_thread.h>
#include <osal_dev.h>
#include <osal_types.h>
#include <ipc.h>
#include <platform_info.h>

#if defined(_OPEN_WRT_)
    #define IPC_RESP_SIZE_MAX  (8 * 1024)
#else
    #define IPC_RESP_SIZE_MAX  (4 * 1024)
#endif /* _OPEN_WRT_ */

#define IPC_CLIENTS_MAX        1
#define IPC_INVALID_CLIENT_ID  1
#define IPC_DRIVER_NAME "/dev/wcd-dsp-glink"

struct ipc_client {
    uint8_t id;
    resp_cb cb;
};

struct ipc_priv {
    struct ipc_client client[IPC_CLIENTS_MAX];
    osal_dev_t dev;
    int32_t init_ref_cnt;
    osal_thread_t resp_thread;
    struct platform_info *pinfo;
    bool exit_thread;
};
static struct ipc_priv gpriv;

static int32_t ipc_receive_pkt(void *data, size_t size);

void ipc_resp_thread(void *param __unused)
{
    int32_t ret = OSAL_SUCCESS, hdr_size, resp_size;
    struct ipc_pkt *resp;
    struct ipc_hdr *hdr;

    resp = osal_mem_calloc(1, IPC_RESP_SIZE_MAX);
    if (!resp) {
        OSAL_LOGE("%s: Failed to allocate memory for resp\n", __func__);
        return;
    }
    hdr_size = sizeof(struct ipc_hdr);

    while (1) {

        if (gpriv.exit_thread)
            break;

        if ((resp_size = osal_dev_read(gpriv.dev, resp, IPC_RESP_SIZE_MAX)) <= 0) {
            OSAL_LOGE("%s: read failed %d\n", __func__, resp_size);
        } else {
            hdr = &resp->hdr;
            OSAL_LOGD("%s: opcode: 0x%x, payload_size: %d, resp_buf size: %d\n",
                      __func__, hdr->opcode, hdr->payload_size, resp_size);
            if ((resp_size < hdr_size) ||
                (hdr->payload_size > ((uint32_t)resp_size - hdr_size))) {
                OSAL_LOGE("%s: Invalid payload_size = %d, resp_buf size = %d, hdr_size = %d\n",
                          __func__, hdr->payload_size, resp_size, hdr_size);
                continue;
            }

            switch (hdr->client_id) {
            case CLIENT_ID_AUDIO:
                if (gpriv.client[hdr->client_id].cb)
                    gpriv.client[hdr->client_id].cb(resp);
                else
                    OSAL_LOGE("%s: cb is NULL for client = %d\n", __func__, hdr->client_id);
                break;
            default:
                OSAL_LOGE("%s: Response for unknown client = %d\n", __func__, hdr->client_id);
                break;
            }
        }
    }
    osal_mem_free(resp);
    osal_thread_close_from_thread_fn();
    OSAL_LOGD("%s: Exiting response thread\n", __func__);
}

static int32_t ipc_write(void *data, uint32_t size, uint8_t pkt_type)
{
    int32_t ret = OSAL_SUCCESS;
    struct wdsp_write_pkt *wpkt;
    uint32_t wpkt_size;

    wpkt_size = sizeof(struct wdsp_write_pkt) + size;
    wpkt = (struct wdsp_write_pkt *)osal_mem_calloc(1, wpkt_size);
    if (!wpkt) {
        OSAL_LOGE("%s: Failed to allocate memory for wpkt\n", __func__);
        ret = OSAL_ENOMEMORY;
        goto done;
    }

    wpkt->pkt_type = pkt_type;
    osal_mem_cpy(wpkt->payload, size, data, size);
    OSAL_LOGV("%s: write size: %d\n", __func__, wpkt_size);

    ret = osal_dev_write(gpriv.dev, wpkt, wpkt_size);
    osal_mem_free(wpkt);

done:
    return ret;
}

int32_t ipc_send_msg(void *payload, size_t payload_size,
                     uint16_t opcode, bool use_island)
{
    int32_t ret = OSAL_SUCCESS;
    struct wdsp_cmd_pkt *cmd_pkt;
    uint32_t i, cmd_pkt_size;

    if (!payload) {
        OSAL_LOGE("%s: Invalid payload\n", __func__);
        ret = OSAL_EBADPARAM;
        goto done;
    }

    cmd_pkt_size = sizeof(struct wdsp_cmd_pkt) + payload_size;
    cmd_pkt = (struct wdsp_cmd_pkt *)osal_mem_calloc(1, cmd_pkt_size);
    if (!cmd_pkt) {
        OSAL_LOGE("%s: Failed to allocate cmd memory\n", __func__);
        ret = OSAL_ENOMEMORY;
        goto done;
    }

    ret = platform_get_ch_name(opcode, use_island, cmd_pkt);
    if (ret < 0) {
        OSAL_LOGE("%s: Failed to get channel name\n", __func__);
        ret = OSAL_EBADPARAM;
        goto err_ch_name;
    }

    cmd_pkt->payload_size = payload_size;
    osal_mem_cpy(cmd_pkt->payload, payload_size, payload, payload_size);

    ret = ipc_write(cmd_pkt, cmd_pkt_size, WDSP_CMD_PKT);
    if (ret < 0)
        OSAL_LOGE("%s: Failed to send cmd pkt, ret = %d\n", __func__, ret);

err_ch_name:
    osal_mem_free(cmd_pkt);

done:
    return ret;
}

int32_t ipc_ready_to_use(void)
{
    int32_t ret = OSAL_SUCCESS;

    ret = ipc_write("READY_PKT", sizeof("READY_PKT"), WDSP_READY_PKT);
    if (ret < 0)
        OSAL_LOGE("%s: Failed to send ready pkt, ret = %d\n", __func__, ret);

    return ret;
}

int32_t ipc_register(uint8_t id, resp_cb cb)
{
    int32_t ret = OSAL_SUCCESS, i;

    if (!cb) {
        OSAL_LOGE("%s: Callback is NULL\n", __func__);
        ret = OSAL_EBADPARAM;
        goto done;
    }

    for (i = 0; i < IPC_CLIENTS_MAX; i++) {
        if (gpriv.client[i].id == id) {
            OSAL_LOGD("%s: client_id is %d already registered\n", __func__, id);
            goto done;
        }
    }

    for (i = 0; i < IPC_CLIENTS_MAX; i++) {
        if (gpriv.client[i].id == IPC_INVALID_CLIENT_ID) {
            gpriv.client[i].id = id;
            gpriv.client[i].cb = cb;
            OSAL_LOGD("%s: client_id is %d\n", __func__, id);
            break;
        }
    }

done:
    return ret;
}

int32_t ipc_deregister(uint8_t id)
{
    int32_t ret = OSAL_SUCCESS, i;

    if (id == IPC_INVALID_CLIENT_ID) {
        OSAL_LOGE("%s: Invalid client ID\n", __func__);
        ret = OSAL_EBADPARAM;
        goto done;
    }

    for (i = 0; i < IPC_CLIENTS_MAX; i++) {
        if (gpriv.client[i].id == id) {
            gpriv.client[i].id = IPC_INVALID_CLIENT_ID;
            gpriv.client[i].cb = NULL;
            break;
        }
    }

done:
    return ret;
}

int32_t ipc_init(void)
{
    int32_t ret = OSAL_SUCCESS, i;
    struct wdsp_reg_pkt *reg_pkt;
    uint32_t reg_pkt_size, payload_size;
    struct wdsp_glink_ch_info *info;
    uint8_t *payload;

    if (++gpriv.init_ref_cnt != 1) {
        OSAL_LOGE("%s: already initialized, ref_cnt = %d\n", __func__,
                  gpriv.init_ref_cnt);
        goto done;
    }

    ret = osal_dev_open(&gpriv.dev, IPC_DRIVER_NAME, O_RDWR);
    if (ret < 0 ) {
        OSAL_LOGE("%s: dev open failed, ret = %d\n", __func__, ret);
        goto err_dev_open;
    }

    for (i = 0; i < IPC_CLIENTS_MAX; i++) {
        gpriv.client[i].id = IPC_INVALID_CLIENT_ID;
        gpriv.client[i].cb = NULL;
    }

    ret = platform_info_init(&gpriv.pinfo);
    if (ret < 0) {
        OSAL_LOGE("%s: Failed to initialize platform info, ret=%d\n",
                  __func__, ret);
        goto err_close;
    }

    payload_size = gpriv.pinfo->ch_size;
    reg_pkt_size = sizeof(struct wdsp_reg_pkt) + payload_size;
    OSAL_LOGD("%s: reg_pkt_size = %d,  ch_size = %d\n",
              __func__, reg_pkt_size, payload_size);

    reg_pkt = (struct wdsp_reg_pkt *)osal_mem_calloc(1, reg_pkt_size);
    if (!reg_pkt) {
        OSAL_LOGE("%s: Failed to allocate memory for reg_pkt\n", __func__);
        ret = OSAL_ENOMEMORY;
        goto err_close;
    }
    reg_pkt->no_of_channels = gpriv.pinfo->no_of_glink_channels;
    osal_mem_cpy(reg_pkt->payload, payload_size, gpriv.pinfo->ch, payload_size);

    ret = ipc_write(reg_pkt, reg_pkt_size, WDSP_REG_PKT);
    if (ret < 0) {
        OSAL_LOGE("%s: Failed to send reg pkt, ret = %d\n", __func__, ret);
        goto err_free;
    }

    gpriv.exit_thread = false;
    ret = osal_thread_create(&gpriv.resp_thread, NULL, 0, NULL, 0,
                             ipc_resp_thread, NULL);

    if (ret < 0) {
        OSAL_LOGE("%s: Failed to create thread, ret = %d\n", __func__, ret);
        goto err_free;
    }
    osal_mem_free(reg_pkt);
    OSAL_LOGD("%s: Done, ref_cnt = %d\n", __func__, gpriv.init_ref_cnt);
    goto done;

err_free:
    osal_mem_free(reg_pkt);

err_close:
    osal_dev_close(gpriv.dev);

err_dev_open:
    gpriv.init_ref_cnt--;

done:
    return ret;
}

int32_t ipc_deinit(void)
{
    int32_t ret = OSAL_SUCCESS;

    if (--gpriv.init_ref_cnt == 0) {
        platform_info_deinit(gpriv.pinfo);

        gpriv.exit_thread = true;
        ret = osal_dev_close(gpriv.dev);
        if (ret < 0)
            OSAL_LOGE("%s: Failed to close device ret = %d\n", __func__, ret);

        osal_thread_destroy(gpriv.resp_thread);
    }
    OSAL_LOGD("%s: Done, ref_cnt = %d\n", __func__, gpriv.init_ref_cnt);

    return ret;
}
