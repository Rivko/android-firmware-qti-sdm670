/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __PLATFORM_INFO_H__
#define __PLATFORM_INFO_H__

#ifdef _ANDROID_
#include "sound/wcd-dsp-glink.h"

#else
#define WDSP_CH_NAME_MAX_LEN 50

enum {
	WDSP_REG_PKT = 1,
	WDSP_CMD_PKT,
	WDSP_READY_PKT,
};

/*
 * struct wdsp_reg_pkt -  Glink channel information structure format
 * @no_of_channels:   Number of glink channels to open
 * @payload[0]:       Dynamic array contains all the glink channels information
 */
struct wdsp_reg_pkt {
	uint8_t no_of_channels;
	uint8_t payload[0];
};

/*
 * struct wdsp_cmd_pkt - WDSP command packet format
 * @ch_name:         Name of the glink channel
 * @payload_size:    Size of the payload
 * @payload[0]:      Actual data payload
 */
struct wdsp_cmd_pkt {
	char ch_name[WDSP_CH_NAME_MAX_LEN];
	uint32_t payload_size;
	uint8_t payload[0];
};

/*
 * struct wdsp_write_pkt - Format that userspace send the data to driver.
 * @pkt_type:      Type of the packet(REG or CMD PKT)
 * @payload[0]:    Payload is either cmd or reg pkt structure based on pkt type
 */
struct wdsp_write_pkt {
	uint8_t pkt_type;
	uint8_t payload[0];
};

/*
 * struct wdsp_glink_ch_cfg - Defines the glink channel configuration.
 * @ch_name:           Name of the glink channel
 * @latency_in_us:     Latency specified in micro seconds for QOS
 * @no_of_intents:     Number of intents prequeued
 * @intents_size[0]:   Dynamic array to specify size of each intent
 */
struct wdsp_glink_ch_cfg {
	char name[WDSP_CH_NAME_MAX_LEN];
	uint32_t latency_in_us;
	uint32_t no_of_intents;
	uint32_t intents_size[0];
};
#endif

#define GLINK_CH_MAX      4
#define GRAPHITE_CMD_MAX  7

/* Graphite command values defined by graphite server */
#define GRAPHITE_BASIC_RESPONSE_RESULT 0x0002
#define GRAPHITE_CMD_OPEN              0x0003
#define GRAPHITE_CMD_CLOSE             0x0004
#define GRAPHITE_CMD_SET_CONFIG        0x0005
#define GRAPHITE_CMD_GET_CONFIG        0x0006
#define GRAPHITE_CMDRSP_GET_CONFIG     0x0007
#define GRAPHITE_CMD_LOAD_DATA         0x0008
#define GRAPHITE_CMDRSP_LOAD_DATA      0x0009
#define GRAPHITE_CMD_UNLOAD_DATA       0x000A
#define GRAPHITE_EVENT                 0x000B
#define GRAPHITE_CMD_DATA              0x000C
#define GRAPHITE_CMDRSP_DATA           0x000D

/* Structure to maintain graphite command to channel mapping */
struct cmd_ch_map {
    uint32_t cmd;
    char ch_name[WDSP_CH_NAME_MAX_LEN];
};

/* Structure to hold platform specific information */
struct platform_info {
    uint8_t no_of_glink_channels;
    struct wdsp_glink_ch_cfg *ch;
    uint8_t ch_copied;
    uint32_t ch_size;

    struct cmd_ch_map cmd_ch_map[GRAPHITE_CMD_MAX];
};

/* platform_get_ch_name - To get the channel name for the command
 * @opcode:       Opcode to get the channel name
 * @use_island:   Flag to indicate whether to use island or non-island memory
 * @pkt:          Command packet to be filld with channel name
 */
int32_t platform_get_ch_name(uint16_t opcode, bool use_island, struct wdsp_cmd_pkt *pkt);

/*
 * platform_info_init - To initialize the platform information
 *                      like glnk channel configuration
 * @info:    Platform info strutcure to allocate
 */
int32_t platform_info_init(struct platform_info **info);

/* platform_info_deinit - To clean up the resources
 * @info:    Platform info structure pointer to free
 */
int32_t platform_info_deinit(struct platform_info *info);
#endif /* __PLATFORM_INFO_H__ */
