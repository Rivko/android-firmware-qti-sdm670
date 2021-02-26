/**
  @file ListenProcSvc_Includes.h
  @brief This file contains Elite Listen Processing service include components.
*/

/*==============================================================================
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/listen/services/dynamic_svcs/listen_proc_svc/src/ListenProcSvc_Includes.h#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  06/23/14    Unni     Added LSM topology support
  11/26/12    Sudhir   Initial version
==============================================================================*/
#ifndef _LISTENPROCSVC_INC_H_
#define _LISTENPROCSVC_INC_H_

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "qurt_elite.h"
#include "Elite.h"
#include "EliteMsg_Custom.h"
#include "ListenProcSvc.h"
#include "audio_basic_op_ext.h"
#include "adsp_lsm_api.h"
#include <stringl/stringl.h>
#include "topo_utils.h"
#include "Elite_CAPI_V2_types.h"
#include "AFEInterface.h"
#include "ListenProcSvc_mmpm.h"
#include "EliteLoggingUtils_i.h"
#include "diagpkt.h"

/*------------------------------------------------------------------------------
 * Constant / Define Declarations
 *----------------------------------------------------------------------------*/
static const uint32_t LISTEN_PROC_CMD_SIG = 0x80000000;
static const uint32_t LISTEN_PROC_DATA_SIG = 0x40000000;
static const uint32_t LISTEN_PROC_RESP_SIG = 0x20000000;
static const uint32_t LISTEN_PROC_OUT_SIG = 0x10000000;
static const uint32_t LISTEN_PROC_NUM_Q = 2;

/**@brief Maximum data messages in data queue.*/
static const uint32_t MAX_DATA_Q_ELEMENTS = 16;

/**@brief Maximum number of commands expected ever in command queue. */
static const uint32_t MAX_CMD_Q_ELEMENTS = 8;

/* Macro for the number of buffers between dynamic service and AFE. */
#define NUM_DATA_BUFFERS  2

/* Number of channels */
#define LSM_MAX_NUM_CHANNEL  8

/* bytes per channel */
#define BYTES_PER_CHANNEL  2

/* interleaved or de intelreaved */
#define INTERLEAVED_DATA 0

/* Tap ID for logging LSM input */
#define LSMLOG_IN_TAP_ID    0x0000

/* Tap ID for logging LSM output */
#define LSMLOG_OUT_TAP_ID    0x0001

/*------------------------------------------------------------------------------
 * Global Data Definitions
 *----------------------------------------------------------------------------*/
extern uint32_t lsm_memory_map_client;


/* This is to specify to which client LSM is connected to */
typedef enum
{
  /* Clients*/
  LSM_CONNECTED_TO_NO_CLIENT = 0,    /**< LSM Session is not connected to any client */
  LSM_CONNECTED_TO_AFE ,             /**< LSM Session is connected to AFE*/
  LSM_CONNECTED_TO_ADM               /**< LSM Session is connected to ADM*/
}lsm_proc_svc_client_connection_t;

/* This is the state of each session */
typedef enum
{
  /* Stable States*/
  LISTEN_PROC_STATE_STOP = 0,    /**< Session is in STOP state. No data
                                     processing happens in this state.. */
  LISTEN_PROC_STATE_RUN ,        /**< Session is in RUN state. Data processing
                                     happens in this state. */
}lsm_proc_svc_state_t;

typedef enum
{
	LISTEN_PROC_SUCCESS = 0,
	LISTEN_PROC_FAIL = 1,
	LISTEN_PROC_NEEDMORE,
	LISTEN_PROC_NOTPREPARED,	
	LISTEN_PROC_NULLREF,
	LISTEN_PROC_WRONGSIZE,
	LISTEN_PROC_WRONGMODEL,
	LISTEN_PROC_FATAL_BUFFER_OVERFLOW,
	LISTEN_PROC_DISABLED,
	LISTEN_PROC_LASTERROR,
}listen_proc_err_return_t;

typedef enum
{
  LISTEN_PROC_CMD_Q = 0,
  LISTEN_PROC_INPUT_DATA_Q,
  LISTEN_PROC_OUTPUT_DATA_Q,
}listen_proc_q_type_t;

typedef struct
{
  uint32_t out_buf_wr_offset;
  /**<Offset in output buffer */

  uint32_t empty_bytes;
  /**<Empty bytes in output buffer */

  elite_mem_shared_memory_map_t shared_memmap_type_node;
  /**< Shared Memory Node for Client buffers*/

} lsm_proc_svc_out_buf_param_t;


/**< @brief Listen Proc Svc data structure */
typedef struct
{
  lsm_proc_svc_client_connection_t lsm_client_connection;
  /**< variable to check to which client LSM is connected */
                 
   elite_svc_handle_t svc_handle;
   /**<Handle to give out */

  elite_svc_handle_t *afe_svc_handle;
  /**< AFE service handle */

  elite_svc_handle_t *mtmx_svc_handle;
  /**< MtMx service handle, this is returned by ADM during ADM connect */

  uint32_t mtmx_out_port_id;
  /**< MtMx port id, this is returned by ADM during ADM connect */

   qurt_elite_queue_t* response_q_ptr;
   /**< Response Q */

   qurt_elite_channel_t channel;
   /**< Mask for MQ's owned by this obj */

   uint32_t curr_bit_field;
   /**< Waiting signals indicator */

   uint32_t channel_status;
   /**< status of the channel */
   
   elite_msg_any_t cmd_msg;
   /**< Command message */
 
   elite_msg_any_t input_data_msg;
   /**< Input data message */

   lsm_callback_handle_t callback_data;
   /**< Local copy of call back data */

   lsm_proc_svc_state_t state;
   /**< Internal state of the session */

   uint32_t afe_port_id;
   /**< AFE port ID as provided by client */

   topo_handle_t topo_handle;
   /**< Libraries currently opened */

   uint16_t num_channels;
   /**< Num of channels of LSM session*/

   uint32_t sample_rate;
   /**< sampling rate of LSM session*/

   int16_t bits_per_sample;
   /**< bits per sample of LSM session*/

   uint8_t channel_mapping[LSM_MAX_NUM_CHANNELS];
   /**< Channel mapping of LSM session*/

   uint16_t session_id;
   /**< Session Id of this particular session*/

   uint32_t input_frame_size;
   /**< the minimum number of samples that listen block needs to process */

   uint16_t mode;
   /**< Mode set by clients, that indicates DSP to send corresponding detection events*/

#ifndef DISABLE_DC_RESET  //<-- Need to remove once Algo update is available
   bool_t is_data_to_process;
   /**< Variable which indicates to process in coming data or not This will be
        true when it receiveds MEdia Format command. This will be set to false
        after sending DC reset command */
#endif
   lsm_mmpm_info_t mmpm_info;
   /**< Info/Params used for mmpm interactions */

   uint32_t cmds_received;
   /**< Variable to track the list of cmds received */

#ifdef DEBUG_LISTEN
   uint16_t afe_buffer_cnt;
   /**< for debugging purpose */
#endif

   elite_mem_shared_memory_map_t model_params;
   /**< Sound model params */

   int8_t detection_status;
   /**< Status of Detection Engine */

   void *lab_struct_ptr;
   /**< Pointer to LAB structure */

   elite_msg_any_t output_data_msg;
   /** output data message popped from output data queue */

   lsm_proc_svc_out_buf_param_t out_buf_params;
   /**< output buffer parameters*/

   uint32_t kw_end_max_delay_bytes;
   /**< max frame delay that SVA Algo can return */

   topo_media_fmt_t media_fmt;
   /**< Media Format to be sent to topology*/

   int8_t *output_buf_ptr;
   /**< Output buffer pointer to topology*/

   uint32_t output_buf_size;
   /**< Output buffer size*/

   int8_t *input_log_buf_ptr;
   /**< input Log buffer pointer*/

   uint32_t input_log_buf_size;
   /**< Input log buffer size*/
}listen_proc_svc_t;

/**< @brief Afe regiser criteria Argument structure */
typedef struct
{
  afe_client_register_criteria_type criteria;
  /**< Criterion for which Client wants to register with AFE */

  bool_t criteria_enable;
  /**< TRUE = Enable, FALE = Disable */
}lsm_send_afe_reg_criteria_t;
 

/*------------------------------------------------------------------------------
 * Function declarations
 *----------------------------------------------------------------------------*/

/*
  This function creates all service queues

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult listen_proc_svc_create_svc_q(listen_proc_svc_t* me_ptr);

/*
  This function destroys the Listen Proc service

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  None

  @dependencies
  None.
*/
void listen_proc_svc_destroy_svc(listen_proc_svc_t* me_ptr);

/**
  Flushes buffers present in Queue

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t
         q_type [in] Type of listen proc queue to flush
         response_result [in] (Only Output Queue)This is the response error result to be propagated to client
  @return
  Success/failure

  @dependencies
  None.
*/
ADSPResult listen_proc_svc_flush_q(listen_proc_svc_t *me_ptr,
                                   listen_proc_q_type_t q_type,
                                   ADSPResult response_result);

/*
   This function processes  LSM_SESSION_CMD_SET_PARAM_V2/V3 command that is sent to a given session.

   @param pMe[in/out] This points to the instance of ListenStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @return Success or Failure
 */

ADSPResult listen_proc_svc_set_param_handler(listen_proc_svc_t *me_ptr);

/*
   This function processes  LSM_SESSION_CMD_GET_PARAM_V2/V3 command that is sent to a given session.

   @param pMe[in/out] This points to the instance of ListenStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @return Success or Failure
 */

ADSPResult listen_proc_svc_get_param_handler(listen_proc_svc_t *me_ptr);

/* 
  This function moves dynamic session from STOP to RUN state 

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Indication of success or failure.

  @dependencies
  None. 
*/

ADSPResult listen_proc_svc_start_handler(listen_proc_svc_t *me_ptr);

/* 
  This function stops dynamic session
 
  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t
 
  @return
  Indication of success or failure.

  @dependencies
  None. 
*/
ADSPResult listen_proc_svc_stop_handler(listen_proc_svc_t *me_ptr);

/*
  This function do the actual processing on data

  @param [in/out] me_ptr : pointer to instance of listen_proc_svc_t
  @param [in] inp_buf_size : size of input buffer
  @param [in] inp_buf_ptr : pointer to input data buffer
  @param [in] timestamp : TimeStamp populated from upstream
  @param [in] is_timestamp_valid : TimeStamp validity field populated from upstream
  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult listen_proc_svc_process(listen_proc_svc_t *me_ptr,
                                   uint32_t inp_buf_size,
                                   int8_t *inp_buf_ptr,
                                   uint64_t timestamp,
                                   bool_t is_timestamp_valid);

/* 
  This function sends the commands to AFE
 
  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t
  @param afe_port_id [in]  afe port id to which cmd needs to be send
  @param cmd [in]   type of command send to AFe
  @param params [in]   Arguments (if any) to populate the cmd msg

  @return
  Indication of success or failure.

  @dependencies
  None. 
*/
ADSPResult listen_proc_send_cmd_to_afe(listen_proc_svc_t *me_ptr,
                                       uint16_t afe_port_id,
                                       uint32_t cmd,
                                       void* params);

/*
  This function Handles Capi_V2 Events

  @param inp_buf_params_ptr [in] pointer to input buf params

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
capi_v2_err_t lsm_capi_v2_event_callback(void *context_ptr,
                                         capi_v2_event_id_t id,
                                         capi_v2_event_info_t *event_ptr);

/*
  This function stops buffering and do dc reset

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult listen_proc_svc_eob_handler(listen_proc_svc_t *me_ptr);

/*
 *Function Name: listen_proc_svc_send_event_to_client
 *
 *Parameters: cb_data_ptr : pointer to callback structure containing apr related information
 *            detection_status : pointer to detection engine status
 *            apr_payload_size : size of the apr payload that needs to be allocated
 *            event_id    : UID of the event that needs to be sent to client
 *            status : status that needs to be sent in the payload
 *Description: Sends APR Event to Client with the payload passed to this function
 *
 *Returns: ADSPResult
 *
*/
ADSPResult listen_proc_svc_send_event_to_client(lsm_callback_handle_t *cb_data_ptr,
                                                void* detection_status,
                                                uint32_t apr_payload_size,
                                                uint32_t event_id, uint32_t status);

/* 
  This function executes the disconnect sequence from AFE port

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Indication of success or failure.

  @dependencies
  None. 
 */
ADSPResult listen_proc_disconnect_from_afe_cmd_sequece(listen_proc_svc_t *me_ptr);

/* 
  This function executes the disconnect sequence from ADM 

  @param me_ptr [in/out] This points to the instance of listen_proc_svc_t

  @return
  Indication of success or failure.

  @dependencies
  None. 
 */
ADSPResult listen_proc_disconnect_from_adm_cmd_sequece(listen_proc_svc_t *me_ptr);
#endif /* _LISTENPROCSVC_INC_H_ */


