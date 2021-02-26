/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdlib.h>
#include <osal_errorcodes.h>
#include <osal_log.h>
#include <osal_list.h>
#include <osal_mem.h>
#include <osal_lock.h>
#include <osal_cond.h>
#include <osal_types.h>
#include <ipc.h>
#include <packetizer.h>
#include <platform_info.h>

#define WAIT_TIMEOUT_IN_MS 3000

struct pktzr_node {
    osal_list_t list;
    uint16_t token;
    uint16_t cmd;
    void *priv;
    osal_cond_t wait_cond;
};

struct pktzr_priv {
    osal_list_t wait_list;
    uint16_t token;
    osal_lock_t lock;
    event_cb evt_cb;
    data_cmd_cb  data_cmd_cb;
};
static struct pktzr_priv *ppriv;

int32_t pktzr_resp_cb(void *payload)
{
    int32_t ret = OSAL_SUCCESS;
    uint16_t token;
    struct pktzr_node *wnode, *n;
    struct ipc_pkt *pkt;
    struct pktzr_cmd_rsp *rsp;
    struct ipc_hdr *hdr;
    bool is_basic_rsp = false;

    if (!payload) {
        OSAL_LOGE("%s: payload is NULL\n", __func__);
        ret = OSAL_EBADPARAM;
        goto done;
    }

    osal_lock_acquire(ppriv->lock);
    pkt = (struct ipc_pkt *)payload;
    hdr = &pkt->hdr;
    OSAL_LOGD("%s: opcode = 0x%x size = %d token = %d\n", __func__,
              hdr->opcode, hdr->payload_size, hdr->token);

    if (hdr->opcode == GRAPHITE_EVENT) {
        if (ppriv->evt_cb != NULL)
            ret = ppriv->evt_cb(pkt->payload, hdr->payload_size);
        else
            OSAL_LOGE("%s: event callback function is NULL\n", __func__);
    } else {
        token = hdr->token;
        if (hdr->opcode == GRAPHITE_BASIC_RESPONSE_RESULT)
            is_basic_rsp = true;

        osal_list_for_each_entry_safe(struct pktzr_node, wnode, n, &ppriv->wait_list, list) {
            OSAL_LOGV("%s: wnode %p, wnode->token = %d\n", __func__, wnode, wnode->token);

            if (wnode->token == token) {
                OSAL_LOGD("%s: token %d matched\n", __func__, token);

                if (wnode->cmd == GRAPHITE_CMD_DATA) {
                    if (ppriv->data_cmd_cb != NULL)
                        ret = ppriv->data_cmd_cb(pkt->payload, hdr->payload_size,
                                                 wnode->priv, &is_basic_rsp);
                    else
                        OSAL_LOGE("%s: read callback function is NULL\n", __func__);

                    osal_list_del(&wnode->list);
                    osal_mem_free(wnode);
                } else {
                    rsp = (struct pktzr_cmd_rsp *)wnode->priv;
                    if (rsp->buf == NULL) {
                        OSAL_LOGE("%s: Invalid response buffer\n", __func__);
                        break;
                    }
                    rsp->is_basic_rsp = is_basic_rsp;

                    if (hdr->payload_size <= rsp->buf_size) {
                        osal_mem_cpy(rsp->buf, hdr->payload_size, pkt->payload,
                                     hdr->payload_size);
                        rsp->rsp_size = hdr->payload_size;
                    } else {
                        OSAL_LOGE("%s: received size %d is greater than requested buf size %d\n",
                                  __func__, hdr->payload_size, rsp->buf_size);
                        osal_mem_cpy(rsp->buf, rsp->buf_size, pkt->payload,
                                     rsp->buf_size);
                        rsp->rsp_size = rsp->buf_size;
                    }
                    osal_cond_signal(wnode->wait_cond);
                }
                break;
            }
        }
    }
    osal_lock_release(ppriv->lock);

done:
    return ret;
}

static int32_t pktzr_send_cmd(void *payload, uint32_t size, void *rsp,
                              uint16_t cmd, bool use_island, bool sync_cmd)
{
    int32_t ret = OSAL_SUCCESS;
    struct ipc_pkt *pkt;
    struct pktzr_node *wnode, *node, *n;
    uint32_t pkt_size;

    if (!payload || !size || !rsp) {
        OSAL_LOGE("%s: Input parameters are NULL\n", __func__);
        ret = OSAL_EBADPARAM;
        goto done;
    }

    osal_lock_acquire(ppriv->lock);
    /*
     * Token is a unique identifier to match the responses.
     * 0 is not a valid token so assign 1 when it becomes 0               .
     * after UINT_MAX.
     */
    if (++ppriv->token == 0)
        ppriv->token = 1;
    OSAL_LOGD("%s: opcode = 0x%x, size = %d, token = %d\n",
              __func__, cmd, size, ppriv->token);

    pkt_size = sizeof(struct ipc_pkt) + size;
    if (pkt_size > UINT_MAX) {
        OSAL_LOGE("%s: pkt_size is greater than UINT_MAX, size = %d\n", __func__, size);
    }

    pkt = (struct ipc_pkt *)osal_mem_calloc(1, pkt_size);
    if (!pkt) {
        OSAL_LOGE("%s: Memory allocation failed for pkt\n", __func__);
        ret = OSAL_ENOMEMORY;
        goto err_pkt_alloc;
    }
    pkt->hdr.version = VERSION_ID;
    pkt->hdr.opcode = cmd;
    pkt->hdr.client_id = CLIENT_ID_AUDIO;
    pkt->hdr.domain_id = DOMAIN_ID_APPS;
    pkt->hdr.token = ppriv->token;
    pkt->hdr.payload_size = size;
    osal_mem_cpy(pkt->payload, size, (char *)payload, size);

    wnode = (struct pktzr_node *)osal_mem_calloc(1, sizeof(struct pktzr_node));
    if (!wnode) {
        OSAL_LOGE("%s: Failed to allocate memory for wnode\n", __func__);
        ret = OSAL_ENOMEMORY;
        goto err_node_alloc;
    }
    wnode->token = ppriv->token;
    wnode->cmd = cmd;
    wnode->priv = rsp;

    /* condition is only required for synchonous commands */
    if (sync_cmd)
        osal_cond_create(&wnode->wait_cond);

    osal_init_list(&wnode->list);
    osal_list_add_tail(&wnode->list, &ppriv->wait_list);
    OSAL_LOGV("%s: wnode %p with token %d added to wait queue\n",
              __func__, wnode, ppriv->token);
    ret = ipc_send_msg(pkt, pkt_size, cmd, use_island);
    if (ret < 0) {
        OSAL_LOGE("%s: Failed to send pkt, ret = %d\n", __func__, ret);
        if (ret == -ENETRESET) {
            osal_list_for_each_entry_safe(struct pktzr_node, node, n,
                                          &ppriv->wait_list, list) {
                osal_list_del(&node->list);
                osal_mem_free(node);
            }
            goto err_node_alloc;
        }
        goto err_send_msg;
    }
    osal_mem_free(pkt);
    osal_lock_release(ppriv->lock);

    if (sync_cmd) {
        /* Wait for the response */
        ret = osal_cond_timedwait(wnode->wait_cond, WAIT_TIMEOUT_IN_MS);

        osal_lock_acquire(ppriv->lock);
        osal_cond_destroy(wnode->wait_cond);
        osal_list_del(&wnode->list);
        osal_mem_free(wnode);
        osal_lock_release(ppriv->lock);
    }

    goto done;

err_send_msg:
    osal_list_del(&wnode->list);
    osal_mem_free(wnode);

err_node_alloc:
    osal_mem_free(pkt);

err_pkt_alloc:
    osal_lock_release(ppriv->lock);

done:
    OSAL_LOGV("%s: Done, ret = %d\n", __func__, ret);
    return ret;
}

int32_t pktzr_ready_to_use(void)
{
    return ipc_ready_to_use();
}

int32_t pktzr_graph_open(void *payload, uint32_t size, struct pktzr_cmd_rsp *rsp)
{
    return pktzr_send_cmd(payload, size, rsp, GRAPHITE_CMD_OPEN, false, true);
}

int32_t pktzr_graph_close(void * payload, uint32_t size, struct pktzr_cmd_rsp *rsp)
{
    return pktzr_send_cmd(payload, size, rsp, GRAPHITE_CMD_CLOSE, false, true);
}

int32_t pktzr_set_config(void *payload, uint32_t size, struct pktzr_cmd_rsp *rsp)
{
    return pktzr_send_cmd(payload, size, rsp, GRAPHITE_CMD_SET_CONFIG, false, true);
}

int32_t pktzr_get_config(void *payload, uint32_t size, struct pktzr_cmd_rsp *rsp)
{
    return pktzr_send_cmd(payload, size, rsp, GRAPHITE_CMD_GET_CONFIG, false, true);
}

int32_t pktzr_load_data(void *payload, uint32_t size, struct pktzr_cmd_rsp *rsp,
                        bool use_island)
{
    return pktzr_send_cmd(payload, size, rsp, GRAPHITE_CMD_LOAD_DATA, use_island, true);
}

int32_t pktzr_unload_data(void *payload, uint32_t size, struct pktzr_cmd_rsp *rsp)
{
    return pktzr_send_cmd(payload, size, rsp, GRAPHITE_CMD_UNLOAD_DATA, false, true);
}

int32_t pktzr_data_cmd(void *payload, uint32_t size, void *priv_data)
{

    return pktzr_send_cmd(payload, size, priv_data, GRAPHITE_CMD_DATA, false, false);
}

int32_t pktzr_init(event_cb event_cb_handler, data_cmd_cb data_cmd_cb_handler)
{
    int32_t ret = OSAL_SUCCESS;

    if (!ppriv) {
        ppriv = (struct pktzr_priv *)osal_mem_alloc(sizeof(struct pktzr_priv));
        if (!ppriv) {
            OSAL_LOGE("%s: Failed to allocate memory for ppriv\n", __func__);
            ret = OSAL_ENOMEMORY;
            goto done;
        }
    } else {
        OSAL_LOGE("%s: Already initialized\n", __func__);
        ret = OSAL_EBUSY;
        goto done;
    }

    ret = ipc_init();
    if (ret < 0) {
        OSAL_LOGE("%s: Initialization failed, ret = %d\n", __func__, ret);
        goto err_init;
    }

    ret = ipc_register(CLIENT_ID_AUDIO, pktzr_resp_cb);
    if (ret < 0) {
        OSAL_LOGE("%s: Failed to register audio client with ipc, ret = %d\n",
                  __func__, ret);
        goto err_reg;
    }

    osal_lock_create(&ppriv->lock);
    osal_init_list(&ppriv->wait_list);
    ppriv->token = 0;
    ppriv->evt_cb = event_cb_handler;
    ppriv->data_cmd_cb = data_cmd_cb_handler;

    OSAL_LOGV("%s: Done, ret = %d\n", __func__, ret);
    return ret;

err_init:
err_reg:
    osal_mem_free(ppriv);
    ppriv = NULL;

done:
    return ret;
}

void pktzr_deinit(void)
{
    if (!ppriv)
        return;

    ipc_deregister(CLIENT_ID_AUDIO);
    ipc_deinit();
    osal_list_del(&ppriv->wait_list);
    osal_lock_destroy(ppriv->lock);
    osal_mem_free(ppriv);
    ppriv = NULL;
}
