/*
 * gcs_cal_ipq.c
 *
 * This file adds the graph calibration for 6 channel audio
 * capture using the DMA1 interface.
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdlib.h>
#include <osal_errorcodes.h>
#include <osal_log.h>

#define SG_TYPE_DEVICE 1
#define SG_TYPE_STREAM 2
#define SG_TYPE_SINK 3

/* Structure Definitions for data */
#include <osal_pack_start.h>
struct sg_list {
    uint32_t num_sgs;
    uint32_t sg_ids[3];
}
#include <osal_pack_end.h>
;

#include <osal_pack_start.h>
struct sg_param_hdr {
    uint32_t mid;
    uint16_t iid;
    uint16_t rsvd;
    uint32_t pid;
    uint32_t size;
}
#include <osal_pack_end.h>
;

#include <osal_pack_start.h>
struct sg_mod_list_data {
    uint32_t mid;
    uint16_t iid;
    uint16_t meta;
    uint16_t ip_pins;
    uint16_t op_pins;
}
#include <osal_pack_end.h>
;

#include <osal_pack_start.h>
struct sgt_mod_conn_edge_hdr {
    uint32_t src_mid;
    uint16_t src_iid;
    uint16_t num_dst_mods;
}
#include <osal_pack_end.h>
;

#include <osal_pack_start.h>
struct sgt_mod_conn_dst_mods {
    uint32_t dst_mid;
    uint16_t dst_iid;
    uint16_t num_edges;
}
#include <osal_pack_end.h>
;

#include <osal_pack_start.h>
struct sgt_mod_conn_edge_info {
    uint16_t src_pin_id;
    uint16_t dst_pin_id;
}
#include <osal_pack_end.h>
;

#include <osal_pack_start.h>
struct sgt_mod_list {
    struct sg_param_hdr hdr;
    struct sg_mod_list_data data;
}
#include <osal_pack_end.h>
;

#include <osal_pack_start.h>
struct sgt_mod_list_devsg {
   struct sg_param_hdr hdr;
   struct sg_mod_list_data dma_m;
   struct sg_mod_list_data busyloop_m;
}
#include <osal_pack_end.h>
;

#include <osal_pack_start.h>
struct sgt_mod_conn_list {
    struct sg_param_hdr hdr;
    struct sgt_mod_conn_edge_hdr e_hdr;
    struct sgt_mod_conn_dst_mods dst_mods;
    struct sgt_mod_conn_edge_info e_info[6];
}
#include <osal_pack_end.h>
;

#include <osal_pack_start.h>
struct mod_dma_src_cal {
    struct sg_param_hdr hdr;
    uint8_t minor_version;
    uint8_t num_channels;
    uint8_t bit_width;
    uint8_t data_format;
    uint32_t sample_rate;
    uint32_t interface;
    uint32_t samples_per_frame;
}
#include <osal_pack_end.h>
;

#include <osal_pack_start.h>
struct mod_mch_bridgebuf_cal {
    struct sg_param_hdr hdr;
    uint32_t minor_version;
    uint32_t ch_buf_size[8];
}
#include <osal_pack_end.h>
;

#include <osal_pack_start.h>
struct mod_mch_nrt_cal {
    struct sg_param_hdr hdr;
    uint32_t minor_version;
    uint32_t buf_size;
    uint32_t ch_mask;
}
#include <osal_pack_end.h>
;
/*
 * Data for 6-ch DMA-->BB->NRT graph
 * device subgraph = DMA module
 * stream subgraph = BB module
 * sink subgraph = NRT module
 */
static struct sg_list sg_list = {
    .num_sgs = 3,
    .sg_ids[0] = 1,
    .sg_ids[1] = 2,
    .sg_ids[2] = 3,
};

static struct sgt_mod_list_devsg sgt_dev_mod_list = {
    .hdr = {
        .mid = 0x20001,
        .iid = 0,
        .rsvd = 0,
        .pid = 0x20002,
        .size = sizeof(struct sg_mod_list_data) * 2,
    },
    .dma_m = {
        .mid = 0x20021,
        .iid = 0x02,
        .meta = 0x00,
        .ip_pins = 0x00,
        .op_pins = 0x06,
    },
    .busyloop_m = {
        .mid = 0x2002A,
	.iid = 0x02,
	.meta = 0x00,
	.ip_pins = 0x00,
	.op_pins = 0x00,
    },
};

static struct sgt_mod_list sgt_stream_mod_list = {
    .hdr = {
        .mid = 0x20001,
        .iid = 0,
        .rsvd = 0,
        .pid = 0x20002,
        .size = sizeof(struct sg_mod_list_data),
    },
    .data = {
        .mid = 0x20028,
        .iid = 0x02,
        .meta = 0x01,
        .ip_pins = 0x08,
        .op_pins = 0x08,
    },
};

static struct sgt_mod_list sgt_sink_mod_list = {
    .hdr = {
        .mid = 0x20001,
        .iid = 0,
        .rsvd = 0,
        .pid = 0x20002,
        .size = sizeof(struct sg_mod_list_data),
    },
    .data = {
        .mid = 0x20013,
        .iid = 0x02,
        .meta = 0x00,
        .ip_pins = 0x08,
        .op_pins = 0x00,
    },
};

static struct sgt_mod_conn_list sgt_dev_to_stream_conns = {
    .hdr = {
        .mid = 0x20001,
        .iid = 0,
        .rsvd = 0,
        .pid = 0x20003,
        .size = sizeof(struct sgt_mod_conn_list) -
                sizeof(struct sg_param_hdr),
    },
    .e_hdr = {
        .src_mid = 0x20021,
        .src_iid = 0x02,
        .num_dst_mods = 0x01,
    },
    .dst_mods = {
        .dst_mid = 0x20028,
        .dst_iid = 0x02,
        .num_edges = 6,
    },
    .e_info = {
        {0, 0}, {1, 1}, {2, 2},
        {3, 3}, {4, 4}, {5, 5},
    },
};

static struct sgt_mod_conn_list sgt_stream_to_sink_conns = {
    .hdr = {
        .mid = 0x20001,
        .iid = 0,
        .rsvd = 0,
        .pid = 0x20003,
        .size = sizeof(struct sgt_mod_conn_list) -
                sizeof(struct sg_param_hdr),
    },
    .e_hdr = {
        .src_mid = 0x20028,
        .src_iid = 0x02,
        .num_dst_mods = 0x01,
    },
    .dst_mods = {
        .dst_mid = 0x20013,
        .dst_iid = 0x02,
        .num_edges = 6,
    },
    .e_info = {
        {0, 0}, {1, 1}, {2, 2},
        {3, 3}, {4, 4}, {5, 5},
    },
};

static struct mod_dma_src_cal wdma1_cal = {
    .hdr = {
        .mid = 0x20021,
        .iid = 0x02,
        .rsvd = 0,
        .pid = 0x12C25,
        .size = 0x10,
    },
    .minor_version  =  0x01,
    .num_channels = 0x06,
    .bit_width = 0x10,
    .data_format = 0x00,
    .sample_rate = 0x3E80,
    .interface = 0x02,
    .samples_per_frame = 0xA0,
};

static struct mod_mch_bridgebuf_cal bb_cal = {
    .hdr = {
        .mid = 0x20028,
        .iid = 0x02,
        .rsvd = 0,
        .pid = 0x20029,
        .size = 0x24,
    },
    .minor_version = 0x01,
    .ch_buf_size = {
        0x3E80, 0x3E80, 0x3E80, 0x3E80,
        0x3E80, 0x3E80, 0, 0,
    },
};

static struct mod_mch_nrt_cal nrt_cal = {
    .hdr = {
        .mid = 0x20013,
        .iid = 0x02,
        .rsvd = 0,
        .pid = 0x20014,
        .size = 0xC,
    },
    .minor_version = 0x02,
    .buf_size = 0x1F40,
    .ch_mask = 0x3F,
};

int32_t gcs_cal_wrapper_init(void)
{
    /* Nothing to init */
    return 0;
}

int32_t gcs_cal_wrapper_deinit(void)
{
    /* Nothing to de-init */
    return 0;
}

int32_t gcs_cal_get_subgraph_list_size(uint32_t u_id, uint32_t d_id,
                                       uint32_t *cal_size)
{
    *cal_size =  sizeof(struct sg_list);

    return 0;
}

int32_t gcs_cal_get_subgraph_list(uint32_t u_id, uint32_t d_id,
                                  uint8_t *cal_buff, size_t cal_size)
{
    if (cal_size != sizeof(struct sg_list))
        return OSAL_EBADPARAM;

    osal_mem_cpy(cal_buff, cal_size, &sg_list, cal_size);

    return 0;
}

int32_t gcs_cal_get_subgraph_mod_info_list_size(uint32_t u_id, uint32_t d_id,
                                                uint32_t sg_id,
                                                uint32_t *cal_size)
{
    switch (sg_id) {
    case SG_TYPE_DEVICE:
        *cal_size = sizeof(sgt_dev_mod_list);
        break;
    case SG_TYPE_STREAM:
        *cal_size = sizeof(sgt_stream_mod_list);
        break;
    case SG_TYPE_SINK:
        *cal_size = sizeof(sgt_sink_mod_list);
        break;
    default:
        return OSAL_EBADPARAM;
    }

    return 0;
}

int32_t gcs_cal_get_subgraph_mod_info_list(uint32_t u_id, uint32_t d_id,
                                           uint32_t sg_id,
                                           uint8_t *cal_buff, size_t cal_size)
{
    switch (sg_id) {
    case SG_TYPE_DEVICE:
        osal_mem_cpy(cal_buff, cal_size, &sgt_dev_mod_list, cal_size);
        break;
    case SG_TYPE_STREAM:
        osal_mem_cpy(cal_buff, cal_size, &sgt_stream_mod_list, cal_size);
        break;
    case SG_TYPE_SINK:
        osal_mem_cpy(cal_buff, cal_size, &sgt_sink_mod_list, cal_size);
        break;
    default:
        return OSAL_EBADPARAM;
    }

    return 0;
}

int32_t gcs_cal_get_subgraph_mod_conn_size(uint32_t u_id, uint32_t d_id,
                                           uint32_t sg_id,
                                           uint32_t *cal_size)
{
    /*
     * All subgraphs have single modules,
     * no connections within the subgraph
     */
    return 0;
}

int32_t gcs_cal_get_subgraph_mod_conn(uint32_t u_id, uint32_t d_id,
                                      uint32_t sg_id,
                                      uint8_t *cal_buff, size_t cal_size)
{
    /*
     * Its error to call this API as
     * get_subgraph_mod_conn_size is returning 0
     */
    return OSAL_EBADPARAM;
}

int32_t gcs_cal_get_subgraph_conn_size(uint32_t u_id, uint32_t d_id,
                                       uint32_t src_sg_id,
                                       uint32_t dst_sg_id,
                                       uint32_t *cal_size)
{
    *cal_size = 0;
    OSAL_LOGD("%s: Enter src %u dst %u cal_size %u\n",
              __func__, src_sg_id, dst_sg_id, *cal_size);
    switch (src_sg_id) {
    case SG_TYPE_DEVICE:
        if (dst_sg_id == SG_TYPE_STREAM) {
            *cal_size = sizeof(sgt_dev_to_stream_conns);
            OSAL_LOGD("%s: src %u dst %u cal_size %u\n",
                      __func__, src_sg_id, dst_sg_id, *cal_size);
        }
        break;
    case SG_TYPE_STREAM:
        if (dst_sg_id == SG_TYPE_SINK) {
            OSAL_LOGD("%s: src %u dst %u cal_size %u\n",
                      __func__, src_sg_id, dst_sg_id, *cal_size);
            *cal_size = sizeof(sgt_stream_to_sink_conns);
        }
        break;
    }

    return 0;
}

int32_t gcs_cal_get_subgraph_conn(uint32_t u_id, uint32_t d_id,
                                  uint32_t src_sg_id, uint32_t dst_sg_id,
                                  uint8_t *cal_buff, size_t cal_size)
{
    switch (src_sg_id) {
    case SG_TYPE_DEVICE:
        if (dst_sg_id == SG_TYPE_STREAM) {
            OSAL_LOGD("%s: src %u dst %u cal_size %zu\n",
                      __func__, src_sg_id, dst_sg_id, cal_size);
            osal_mem_cpy(cal_buff, cal_size,
                         &sgt_dev_to_stream_conns, cal_size);
        }
        else
            /* Size is returned only for DEV to STREAM subgraphs */
            return OSAL_EBADPARAM;
        break;

    case SG_TYPE_STREAM:
        if (dst_sg_id == SG_TYPE_SINK) {
            OSAL_LOGD("%s: src %u dst %u cal_size %zu\n",
                      __func__, src_sg_id, dst_sg_id, cal_size);
            osal_mem_cpy(cal_buff, cal_size,
                         &sgt_stream_to_sink_conns, cal_size);
        }
        else
            /* Size is returned only for DEV to STREAM subgraphs */
            return OSAL_EBADPARAM;
        break;
    default:
        return OSAL_EBADPARAM;
    }

    return 0;
}

int32_t gcs_cal_get_subgraph_mod_cal_size(uint32_t u_id, uint32_t d_id,
                                          uint32_t sg_id,
                                          uint32_t *cal_size)
{
    switch (sg_id) {
    case SG_TYPE_DEVICE:
        *cal_size = sizeof(wdma1_cal);
        break;
    case SG_TYPE_STREAM:
        *cal_size = sizeof(bb_cal);
        break;
    case SG_TYPE_SINK:
        *cal_size = sizeof(nrt_cal);
        break;
    }
    return 0;
}

int32_t gcs_cal_get_subgraph_mod_cal(uint32_t u_id, uint32_t d_id,
                                     uint32_t sg_id,
                                     uint8_t *cal_buff, size_t cal_size)
{
    switch (sg_id) {
    case SG_TYPE_DEVICE:
        osal_mem_cpy(cal_buff, cal_size, &wdma1_cal, cal_size);
        break;
    case SG_TYPE_STREAM:
        osal_mem_cpy(cal_buff, cal_size, &bb_cal, cal_size);
        break;
    case SG_TYPE_SINK:
        osal_mem_cpy(cal_buff, cal_size, &nrt_cal, cal_size);
        break;
    }
    return 0;
}

int32_t gcs_cal_get_boot_time_mod_list_size(uint32_t *cal_size)
{
    /* Not Required */
    return 0;
}

int32_t gcs_cal_get_boot_time_mod_list(uint8_t *cal_buff, size_t cal_size)
{
    /* Not Required */
    return 0;
}

int32_t gcs_cal_get_mod_loading_meta_info_size(uint32_t mod_type,
                                               uint32_t mod_count,
                                               uint8_t *mod_list,
                                               uint32_t *cal_size)
{
    /* Not Required */
    return 0;
}

int32_t gcs_cal_get_mod_loading_meta_info(uint32_t mod_type,
                                          uint32_t mod_count,
                                          uint8_t *mod_list,
                                          uint8_t *cal_buff,
                                          size_t cal_size)
{
    /* Not Required */
    return 0;
}
