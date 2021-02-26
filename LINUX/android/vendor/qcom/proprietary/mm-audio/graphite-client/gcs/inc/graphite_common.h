/*
 * graphite_common.h
 *
 * This header contains common structures and other definitions for
 * different Graphite clients. These are used for communicating with
 * Graphite server.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __GRAPHITE_COMMON_H__
#define __GRAPHITE_COMMON_H__

/* Graphite Calibration header */
#include <osal_pack_start.h>
struct graphite_cal_header {
    /* Module ID of the particular module receiving the payload */
    uint32_t module_id;

    /*
     * Instance ID of the particular module receiving the payload.
     * instance_id is not applicable to framework modules, and will be
     * ignored.
     */
    uint16_t instance_id;

    /* Reserved field, must be set to 0. */
    uint16_t reserved;

    /*
     * Indicates a particular calibration parameter that needs to be applied to
     * the module/instance ID listed above.
     * For Events types, this represents the particular event that is raised
     * by Graphite.
     */
    uint32_t param_id;

    /*
     * Size of the payload for this calibration parameter.
     * For GRAPHITE_CMD_GET_CONFIG, set size to 0xFFFFFFFF to indicate that the
     * size of the calibration parameter is unknown to the client, and needs
     * to be calculated by Graphite.
     */
    uint32_t size;
}
#include <osal_pack_end.h>
;

/* Graphite GRAPHITE_CMD_DATA header */
#include <osal_pack_start.h>
struct graphite_data_cmd_hdr
{
    /* Module ID of a particular module. */
    uint32_t module_id;

    /* Instance ID of the particular module. */
    uint16_t instance_id;

    /* Reserved field, must be set to 0. */
    uint16_t reserved;

    /* ID of the module's command. */
    uint32_t cmd_id;

    /* Size of the payload in number of bytes. */
    uint32_t size_in_bytes;

    /*
     * Token, to be used by caller, to synchronize the data command
     * and its response.
     */
    uint32_t token;
}
#include <osal_pack_end.h>
;

/* Graphite GRAPHITE_CMDRSP_DATA header */
#include <osal_pack_start.h>
struct graphite_data_cmdrsp_hdr {
    /* Module ID of a particular module. */
    uint32_t module_id;

    /* Instance ID of the particular module. */
    uint16_t instance_id;

    /*  Reserved field, must be set to 0. */
    uint16_t reserved;

    /* ID of the module's command */
    uint32_t cmd_id;

    /* Size of the payload in number of bytes. */
    uint32_t size_in_bytes;

    /*
     * Token, to be used by caller, to synchronize the data command
     * and its response.
     */
    uint32_t token;
}
#include <osal_pack_end.h>
;

#endif /* __GRAPHITE_COMMON_H__ */
