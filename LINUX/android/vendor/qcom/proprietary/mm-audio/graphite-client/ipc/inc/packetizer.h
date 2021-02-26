/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __PACKETIZER_H__
#define __PACKETIZER_H__

/* pktzr_cmd_rsp structure holds the response buffer information */
struct pktzr_cmd_rsp {
    /* Requested resp buffer */
    void *buf;

    /* Requested resp buffer size */
    uint32_t buf_size;

    /* Received resp buffer size */
    uint32_t rsp_size;

    /* Basic response or command response */
    bool is_basic_rsp;
};

typedef int32_t (*event_cb)(void *payload, uint32_t payload_size);
typedef int32_t (*data_cmd_cb)(void *payload, uint32_t payload_size,
                               void *priv_data, bool *is_basic_rsp);

/*
 * pktzr_init - Init function. Has to call before anything to initialize
 * @event_cb_handler:        Call back function to be called when event is detected
 * @data_cmd_cb_handler:     Call back function to be called for data cmd
 */
int32_t pktzr_init(event_cb event_cb_handler, data_cmd_cb data_cmd_cb_handler);

/* pktzr_deinit - Deinit function to clean up the resources */
void pktzr_deinit(void);

/*
 * pktzr_ready_to_use - Block until packetizer is ready to use
 *                      or 1 second timeout whichever is less
 */
int32_t pktzr_ready_to_use(void);

/*
 * pktzr_graph_open - Open API to open the graph.
 * @payload:          Graph payload to send
 * @payload_size:     Size of the payload
 * @rsp:              Response structure to fill the response
 */
int32_t pktzr_graph_open(void *payload, uint32_t payload_size, struct pktzr_cmd_rsp *rsp);

/*
 * pktzr_graph_close - Close API to close the graph.
 * @payload:          Graph payload to send
 * @payload_size:     Size of the payload
 * @rsp:              Response structure to fill the response
 */
int32_t pktzr_graph_close(void *payload, uint32_t payload_size, struct pktzr_cmd_rsp *rsp);

/*
 * pktzr_set_config - Get config API to set the configuration.
 * @payload:          Set config command payload to send
 * @payload_size:     Size of the payload
 * @rsp:              Response structure to fill the response
 */
int32_t pktzr_set_config(void *payload, uint32_t payload_size, struct pktzr_cmd_rsp *rsp);

/*
 * pktzr_get_config - Get config API to get the configuration
 * @payload:          Get config command payload to send
 * @payload_size:     Size of the payload
 * @rsp:              Response structure to fill the response
 */
int32_t pktzr_get_config(void *payload, uint32_t payload_size, struct pktzr_cmd_rsp *rsp);

/*
 * pktzr_load_data - Load data API to load the data like sound model.
 * @payload:          Load data command payload to send
 * @payload_size:     Size of the payload
 * @rsp:              Response structure to fill the response
 * @use_island:       Flag to indicate whether to use island or non-island memory
 */
int32_t pktzr_load_data(void *payload, uint32_t payload_size, struct pktzr_cmd_rsp *rsp,
                        bool use_island);

/*
 * pktzr_unload_data - Unload data API to unload the data.
 * @payload:          Unload data command payload to send
 * @payload_size:     Size of the payload
 * @rsp:              Response structure to fill the response
 */
int32_t pktzr_unload_data(void *payload, uint32_t payload_size, struct pktzr_cmd_rsp *rsp);

/*
 * pktzr_data_cmd - Data cmd API to send data command
 * @payload:          Data command payload to send
 * @payload_size:     Size of the payload
 * @priv_data:        Private data to be returned with the response callback
 */
int32_t pktzr_data_cmd(void *payload, uint32_t payload_size, void *priv_data);
#endif /* __PACKETIZER_H__ */


