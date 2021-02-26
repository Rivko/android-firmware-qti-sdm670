/*
 * gcs_api.h
 *
 * This library contains the Graphite Client Service APIs to load,
 * setup, start, stop and tear-down a use case for Codec DSP using
 * Graphite (Graph-Lite) architecture.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __GCS_API_H__
#define __GCS_API_H__

/* Max payload size GCS accepts from caller */
#define MAX_GCS_PLD_SIZE (256 * 1024)

/* Graph data transfer direction */
enum gcs_data_xfer {
    GCS_XFER_READ = 0,
    GCS_XFER_WRITE,
    GCS_XFER_MAX
};

struct gcs_module_info {
    /* Module ID of the particular module */
    uint32_t MID;

    /* Instance ID of the particular module */
    uint32_t IID;
};

struct gcs_module_param_info {
    /* MID/IID of the module which raises the event */
    struct gcs_module_info module_info;

    /* Param ID for the specific event for the module */
    uint32_t PID;
};

struct gcs_event_rsp {
    /* MID/IID/PID info for the specific event for the module */
    struct gcs_module_param_info module_param_info;

    /* size of the event payload */
    uint32_t payload_size;

    /* event payload from the module raising the event */
    void *payload;
};

typedef int32_t (*event_cb_ptr)(uint32_t, struct gcs_event_rsp *, void *);

/*
 * data_cmd_cb_ptr - Typedef for data command handler to be registered with GCS
 *
 * @graph_handle:    handle to the graph in GCS
 * @rsp:             pointer to the data command response buffer
 * @rsp_size:        response buffer size
 * @cookie:          private data pointer from caller, that was passed when
 *                   registering data command callback handler.
 * @cmd_status       command status from Graphite server.
 */
typedef int32_t (*data_cmd_cb_ptr)(uint32_t graph_handle, void *rsp,
                                   uint32_t rsp_size, void *cookie,
                                   int32_t cmd_status);

/*
 * Below APIs are defined by GCS and can be used by caller to open,
 * enable, disable and close a graph.
 * The APIs also enables the caller to load/unload data to/from
 * persistent memory and register for an event generated from a
 * module.
 *
 * These APIs are the main communication mechanism between a caller (HAL etc.)
 * and GCS.
 */

/*
 * gcs_init - Initialize GCS
 *
 * Return 0 on success or error code otherwise
 *
 * Prerequisites
 *    None.
 */
int32_t gcs_init(void);

/*
 * gcs_deinit - De-initialize GCS
 *
 * Return 0 on success or error code otherwise
 *
 * Prerequisites
 *    GCS must be initialized.
 */
int32_t gcs_deinit(void);

/*
 * gcs_open - Setup a new graph with GCS
 *
 * @u_id:            unique ID to identify the graph to be setup
 * @d_id:            device ID of the device associated with the graph
 * @graph_handle:    handle to the graph in GCS
 *
 * Return 0 on success or error code otherwise
 *
 * Prerequisites
 *    GCS must be initialized.
 */
int32_t gcs_open(uint32_t u_id, uint32_t d_id, uint32_t *graph_handle);

/*
 * gcs_close - Tear down an existing graph in GCS
 *
 * @graph_handle:    handle to the graph in GCS
 *
 * Return 0 on success or error code otherwise
 *
 * Prerequisites
 *    Graph must be setup with GCS.
 *
 * NOTE:
 *    All information about persistent data and event will be flushed
 *    for the graph, even if explicit call to unload_data and
 *    event_deregister is not made.
 */
int32_t gcs_close(uint32_t graph_handle);

/*
 * gcs_enable - Enable a graph
 *
 * @graph_handle:        handle to the graph in GCS
 * @non_persist_ucal:    pointer to user calibration data that are not expected
 *                       to persist across multiple enable/disable of
 *                       the graph.
 * @size_ucal:           size of the non-persistent user calibration data blob.
 *
 * Return 0 on success or error code otherwise
 *
 * Prerequisites
 *    Graph must be setup with GCS.
 */
int32_t gcs_enable(uint32_t graph_handle, void *non_persist_ucal,
                   uint32_t size_ucal);

/*
 * gcs_disable - Disable an active graph
 *
 * @graph_handle:    handle to the graph in GCS
 *
 * Return 0 on success or error code otherwise
 *
 * Prerequisites
 *    Graph must be active.
 */
int32_t gcs_disable(uint32_t graph_handle);

/*
 * gcs_load_data - Load user data in persistent memory for a graph
 *
 * @graph_handle:    handle to the graph in GCS
 * @data:            data to be loaded into persistent memory
 * @data_size:       size of the data to be loaded into persistent memory
 * @data_handle:     pointer to a handle to be returned by GCS for the
 *                   data once loaded into memory
 *
 * Return 0 on success or error code otherwise
 *
 * Prerequisites:
 *    Graph must be setup with GCS, but must NOT be active in Graphite server.
 *    The 'data' blob sent to this API must be packaged in Graphite format.
 *
 * Note:
 *    Information for all Persist data blob loaded for a graph will be sent to
 *    Graphite server only when enabling a graph (gcs_enable). Hence, the loaded
 *    data will be available to graphite server only at use case boundary.
 */
int32_t gcs_load_data(uint32_t graph_handle, void *data, uint32_t data_size,
                      uint32_t *data_handle);

/*
 * gcs_unload_data - Unload/Free previously loaded data for a graph from
 *                   persistent memory.
 *
 * @graph_handle:    handle to the graph in GCS
 * @data_handle:     handle to the data loaded
 *
 * Return: 0 on success and error code otherwise
 *
 * Prerequisites:
 *    Graph must be setup with GCS, but must NOT be active in Graphite server.
 */
int32_t gcs_unload_data(uint32_t graph_handle, uint32_t data_handle);

/*
 * gcs_register_for_event - Register a callback with GCS for an event
 *                          from a module.
 *
 * @graph_handle:         handle to the graph in GCS
 * @module_param_info:    pointer to structure of type gcs_module_param_info that
 *                        contains Module and Event identifiers (MID, IID, PID).
 * @cb_handler:           pointer to the event callback handler
 * @cookie:               private data pointer that caller wants to get back with
 *                        Callback handler.
 *
 * Return 0 on success or error code otherwise
 *
 * Prerequisites
 *    Graph must be setup with GCS.
 */
int32_t gcs_register_for_event(uint32_t graph_handle,
                               struct gcs_module_param_info *module_param_info,
                               event_cb_ptr cb_handler, void *cookie);

/*
 * gcs_register_data_cmd_handler - Register a callback with GCS for receiving
 *                                 data command responses from Graphite server
 *
 * @graph_handle:           handle to the graph in GCS
 * @data_cmd_cb_handler:    pointer to the data command callback handler
 * @cookie:                 pointer to caller's private data. This will be passed
 *                          back to caller as a parameter in callback handler.
 *
 * Return 0 on success or error code otherwise
 *
 * Prerequisites
 *    Graph must be setup with GCS.
 */
int32_t gcs_register_data_cmd_handler(uint32_t graph_handle,
                                      data_cmd_cb_ptr data_cmd_cb_handler,
                                      void *cookie);

/*
 * gcs_deregister_data_cmd_handler - Unregister the data command callback
 *                                   handler from GCS
 *
 * @graph_handle:    handle to the graph in GCS
 *
 * Return 0 on success or error code otherwise
 *
 * Prerequisites
 *    Graph must be setup with GCS.
 */
int32_t gcs_deregister_data_cmd_handler(uint32_t graph_handle);

/*
 * gcs_start_buff_xfer - Prepare the graph for data transfer in a particular
 *                       direction
 *
 * @graph_handle:    handle to the graph in GCS
 * @xfer:            data transfer type
 *
 * Return 0 on success or error code otherwise
 *
 * Prerequisites
 *    Graph must be setup with GCS.
 *    Graph must be in active state.
 *    Graph must contain specific subgraphs, responsible for data transfer
 *    type refered by @xfer.
 */
int32_t gcs_start_buff_xfer(uint32_t graph_handle, enum gcs_data_xfer xfer);

/*
 * gcs_stop_buff_xfer - Disable data transfer in a specific direction, for
 *                      the particular graph refered with @graph_handle
 *
 * @graph_handle:    handle to the graph in GCS
 * @xfer:            data transfer type
 *
 * Return 0 on success or error code otherwise
 *
 * Prerequisites
 *    Graph must be setup with GCS.
 *    Graph must be in buffering state in the specific data transfer type
 *    specified by @xfer.
 */
int32_t gcs_stop_buff_xfer(uint32_t graph_handle, enum gcs_data_xfer xfer);

/*
 * gcs_send_data_cmd - Send a data command for a graph
 *
 * @graph_handle:    handle to the graph in GCS
 * @payload:         data command payload
 * @payload_size:    size of the data command payload
 *
 * Return 0 on success or error code otherwise
 *
 * Prerequisites
 *    Graph must be setup with GCS.
 *    Graph must be active in Graphite server.
 */
int32_t gcs_send_data_cmd(uint32_t graph_handle, void *payload,
                          uint32_t payload_size);

/*
 * gcs_enable_device - Enable subgraph for the device @device_id
 *
 * @graph_handle:    handle to the graph in GCS
 * @d_id:            The new device id to be setup
 * @non_persist_cal: Any possible user-specific calibration for this
 *                   device-id. NULL in case there is none.
 * @size_ucal:       size of the user calibration, should be 0 if there
 *                   is no user calibration provided.
 * Prerequisites
 *     Graph must be setup with GCS
 *     gcs_disable_device should have been called
 *
 * Note: It is callers responsibility to disable device for all the
 * usecases that have shared modules across device subgraphs before
 * gcs_enable_device can be called on any graph.
 */
int32_t gcs_enable_device(uint32_t graph_handle, uint32_t d_id,
                          void *non_persist_ucal, uint32_t size_ucal);

/*
 * gcs_disable_device - Disable device subgraph
 *
 * @graph_handle:    handle to the graph in GCS
 *
 * If already enabled, this API will disable the device
 * specific subgraph
 *
 * Will return error if bufferring on the graph is active
 *
 * Prerquisites
 *     Graph must be setup with GCS
 */
int32_t gcs_disable_device(uint32_t graph_handle);

/*
 * gcs_set_config - Send configuration parameters to an active module
 *                  in a graph
 *
 * @graph_handle:    handle to the graph in GCS
 * @payload:         set config command payload
 * @payload_size:    size of the set config command payload
 *
 * Return 0 on success or error code otherwise
 *
 * Prerequisites
 *    Graph must be active in Graphite server.
 */
int32_t gcs_set_config(uint32_t graph_handle, void *payload,
                       uint32_t payload_size);
#endif /* __GCS_API_H__ */
