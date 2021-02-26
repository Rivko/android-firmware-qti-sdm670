/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __IPC_H__
#define __IPC_H__

/* Values defined by graphite server */
#define VERSION_ID          0x80
#define CLIENT_ID_AUDIO     0
#define DOMAIN_ID_WDSP      0
#define DOMAIN_ID_APPS      1

/* IPC(CMI) header format */
#include <osal_pack_start.h>
struct ipc_hdr {
    uint8_t version;
    uint8_t reserved;
    uint16_t opcode;
    uint8_t client_id;
    uint8_t domain_id;
    uint16_t token;
    uint32_t payload_size;
}
#include <osal_pack_end.h>
;

/* IPC(CMI) pkt format */
struct ipc_pkt {
    struct ipc_hdr hdr;
    uint8_t payload[0];
};

/* Response call back function definition */
typedef int32_t (*resp_cb)(void *payload);

/*
 * ipc_send_msg - To send pkt to server
 * @payload:          Payload to send to server
 * @payload_size:     Size of the payload
 * @opcode:           Opcode number
 * @use_island:       Island memory to be used or not
 */
int32_t ipc_send_msg(void *payload, size_t payload_size, uint16_t opcode,
                     bool use_island);

/*
 * ipc_ready_to_use -  Block until IPC is ready to use or 1 second
 *                     timeout whichever is less
 */
int32_t ipc_ready_to_use();


/* ipc_init - To initialize the ipc resources */
int32_t ipc_init();

/* ipc_deinit - To free up the ipc resources */
int32_t ipc_deinit();

/*
 * ipc_register - To register with IPC
 * @id:           Client ID
 * @cb:           Call back function to call when response is
 *                received from server
 */
int32_t ipc_register(uint8_t id, resp_cb cb);

/* ipc_deregister - Deregister client */
int32_t ipc_deregister(uint8_t id);
#endif  /* __IPC_H__ */
