/**
@file UltrasounStreamMgr_Util.h
@brief This file declares utility functions that are used by USM 
       service and Ultrasound dynamic services 
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The elite_audio_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      elite_audio_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "audio_stream" group
      description in the elite_audio_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header:

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/04/13   LD      File created 
06/10/13   NR      Add support for updating bit masks and format validation 
06/18/13   LD      Add AFE disconnect utility functions 
==========================================================================*/

/*-----------------------------------------------------------------------
 Copyright (c) 2013 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 -----------------------------------------------------------------------*/

#ifndef ULTRASOUND_STREAM_MGR_UTIL_H
#define ULTRASOUND_STREAM_MGR_UTIL_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/* System */
#include "qurt_elite.h"

/* Audio */
#include "Elite.h"

/* Ultrasound */
#include "adsp_usm_stream_commands.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup ultrasound_stream
@{ */

/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/
/** @} */ /* end_addtogroup ultrasound_stream */
/** @addtogroup ultrasound_stream
 * @{ */

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/
/**
  Wait for a response to a sent message. The function will block the caller
  until a response arrives, or timeout occurs.
  Optionally return the response in the pResponse message, if specified.
  The returned response may contain a payload.
  The signal argument is used for the timer to handle the timeout.
  Both signal and response queue of the message must be added to the same channel.
  The channel must only be used in the calling thread (typically, it is the
  service thread).
  The response queue should not be used for anything else other than synchronous
  responses. 

  @param[in]  pMsg  Sent message to await response for.
  @param[in]  timeout Maximum time to wait for response, in microseconds (usec).
  @param[in]  pSignal Signal to use for timer.
  @param[out] pResponse Response message, can be NULL

  @return
  ADSP_EOK if response arrived successfully.
  Other error code if failure. ADSP_ENOTREADY is returned if timeout occured.

  @dependencies
  None.
*/
ADSPResult usm_util_wait_for_ack_and_get_msg (elite_msg_any_t *pMsg, int64_t timeout, 
                             qurt_elite_signal_t *pSignal, elite_msg_any_t *pResponse);

/**
  Wait for a response to a sent message. The function will block the caller
  until a response arrives, or timeout occurs.
  The signal argument is used for the timer to handle the timeout.
  Both signal and response queue of the message must be added to the same channel.
  The channel must only be used in the calling thread (typically, it is the
  service thread).
  The response queue should not be used for anything else other than synchronous
  responses. 

  @param[in]  pMsg  Sent message to await response for.
  @param[in]  timeout Maximum time to wait for response, in microseconds (usec).
  @param[in]  pSignal Signal to use for timer.

  @return
  ADSP_EOK if response arrived successfully.
  Other error code if failure. ADSP_ENOTREADY is returned if timeout occured.

  @dependencies
  None.
*/
ADSPResult usm_util_wait_for_ack (elite_msg_any_t *pMsg, int64_t timeout, 
                             qurt_elite_signal_t *pSignal);

/**
  Adds a mask to a certain bit field

  @param[in] curr_bit_field   Bit field to update
   
  @param[in] mask_to_add      Mask to be added to the current 
        bit field
  @return
  Updated bit field

  @dependencies
  None.
*/
uint32_t usm_util_add_mask (uint32_t * curr_bit_field, uint32_t mask_to_add); 


/**
  Removes a mask from a certain bit field

  @param[in] curr_bit_field   Bit field to update
   
  @param[in] mask_to_add      Mask to be removed from the 
        current bit field
  @return
  Updated bit field

  @dependencies
  None.
*/
uint32_t usm_util_remove_mask (uint32_t * curr_bit_field, uint32_t mask_to_add); 


/**
  Checks if the recieved format is a valid format for an RX 
  stream 

  @param[in] rx_fmt      Format to check
   

  @return
  1 if rx_fmt is a valid RX stream format, 
  0 otherwise 

  @dependencies
  None.
*/
uint32_t usm_util_is_valid_rx_fmt (uint32_t rx_fmt); 

/**
  Checks if the recieved format is a valid format for an TX 
  stream 

  @param[in] tx_fmt      Format to check
   

  @return
  1 if tx_fmt is a valid RX stream format, 0 otherwise 

  @dependencies
  None.
*/
uint32_t usm_util_is_valid_tx_fmt (uint32_t tx_fmt); 

/**
  Checks if the recieved format is a valid format for a session 
  with 2 stream - one for RX and one for TX

  @param[in] fmt      Format to check
   

  @return
  1 if fmt is a valid dual session format, 
  0 otherwise 

  @dependencies
  None.
*/
uint32_t usm_util_is_valid_dual_session_fmt (uint32_t fmt);

/**
  Sends an AFE disable or disconnect command to an AFE port 
  The function is synchronous, it will wait for the command to 
  be processed by AFE and return the result of processing 
   
  @param[in]  afe_id  AFE port ID
  @param[in]  svc_peer_handle The AFE peer service (the one 
        returned by AFE in the CONNECT message, in the
        svc_handle_ptr member of the payload)
  @param[in]  reqp_q queue to use for getting response from AFE
  @param[in]  opcode The message opcode to send. Can be 
        ELITEMSG_CUSTOM_AFEDISCONNECT_REQ or
        ELITEMSG_CUSTOM_AFECLIENTDISABLE.

  @return 
  If message was processed by AFE and result returned, return 
  the result from AFE (ADSP_EOK if success) 
  Return other errors if message could not be sent successfully.

  @dependencies
  None.
 */
ADSPResult usm_util_send_afe_disconnect_opcode(uint16_t afe_id, 
                                        elite_svc_handle_t* svc_peer_handle,
                                        qurt_elite_queue_t* resp_q,
                                        uint32_t opcode);

/**
  Sends an AFE ELITEMSG_CUSTOM_AFEDISCONNECT_REQ command to an 
  AFE port. The function is synchronous, it will wait for the 
  command to be processed by AFE and return the result of 
  processing 
   
  @param[in]  afe_id  AFE port ID
  @param[in]  svc_peer_handle The AFE peer service (the one 
        returned by AFE in the CONNECT message, in the
        svc_handle_ptr member of the payload)
  @param[in]  reqp_q queue to use for getting response from AFE

  @return 
  If message was processed by AFE and result returned, return 
  the result from AFE (ADSP_EOK if success) 
  Return other errors if message could not be sent successfully.

  @dependencies
  None.
 */
ADSPResult usm_util_send_afe_disconnect_port(uint16_t afe_id, 
                                        elite_svc_handle_t* svc_peer_handle,
                                        qurt_elite_queue_t* resp_q);

/**
  Sends an AFE ELITEMSG_CUSTOM_AFECLIENTDISABLE command to an 
  AFE port. The function is synchronous, it will wait for the 
  command to be processed by AFE and return the result of 
  processing 
   
  @param[in]  afe_id  AFE port ID
  @param[in]  svc_peer_handle The AFE peer service (the one 
        returned by AFE in the CONNECT message, in the
        svc_handle_ptr member of the payload)
  @param[in]  reqp_q queue to use for getting response from AFE

  @return 
  If message was processed by AFE and result returned, return 
  the result from AFE (ADSP_EOK if success) 
  Return other errors if message could not be sent successfully.

  @dependencies
  None.
 */
ADSPResult usm_util_send_afe_disable_port(uint16_t afe_id, 
                                        elite_svc_handle_t* svc_peer_handle,
                                        qurt_elite_queue_t* resp_q);

/** 
  Executes a sequence for disconnecting an AFE TX port. 
  When this function is called the AFE port is active and 
  running (call it from the same thread handling the AFE data 
  packets, typically the dynamic service thread). 
  The function will disconnect the port properly, and release 
  all TX data packets sent by AFE. 
  When function returns, if successful, the AFE port is 
  disconnected and all resources released. 
   
  @param[in]  afe_id  AFE port ID
  @param[in]  svc_peer_handle The AFE peer service (the one 
        returned by AFE in the CONNECT message, in the
        svc_handle_ptr member of the payload)
  @param[in] svc_afe_client_handle The AFE client service 
        handle. Contains the dataQ which is used by AFE to
        deliver TX packets.
  @param[in] data_msg If an existing TX data packet is being 
         handled, Pass it here so it will be released as well
         (in addition to the TX packets in the svc_afe_client
         data queue).
  @param[in]  reqp_q queue to use for getting response from AFE

  @return 
  ADSP_EOK if successful, other error code if not. 

  @dependencies
  None.
 */
ADSPResult usm_util_disconnect_afe_tx_port(uint16_t afe_id, 
                                           elite_svc_handle_t* svc_peer_handle,
                                           elite_svc_handle_t* svc_afe_client_handle,
                                           elite_msg_any_t *data_msg,
                                           qurt_elite_queue_t* resp_q);

/** 
  Validates a channel mapping vector, which is an array 
  defining the order of channels. 
   
  @param[in]  num_channels  total number of channels
  @param[in]  mapping_vector array of channel indices, size must 
        be atleast num_channels

  @return 
  ADSP_EOK if mapping valid, ADSP_EBADPARAM if not

  @dependencies
  None.
*/
ADSPResult usm_util_validate_channel_mapping (uint32_t num_channels, uint8_t *mapping_vector);

/** 
  Drain the queue. pop all messages and release them.
  @param [in] pQ the queue to drain 
   
  @return
  ADSP_EOK if successful, failure code if not

  @dependencies
  None.
 */
ADSPResult usm_svc_utilities_drain_queue(qurt_elite_queue_t* pQ);

/*---------------------------------------------------------------------------
Declarations of Types and Constants Which Are Common to All the Services
----------------------------------------------------------------------------*/

/**<@brief Mask for verifying 32 byte alignment */
static const uint32_t MASK_32BYTE_ALIGNMENT = 0x1F;


#define ELITEMSG_CUSTOM_USM_SIGNAL_DETECT_MODE     (0x002B0001L)

/** Payload structure for the encoder custom message signal detection mode  */
typedef struct
{
	elite_msg_custom_header_t custom_header;

	elite_apr_packet_t	apr_header;

	usm_stream_cmd_set_signal_detect_mode_t* pSignalDetectParams;

} elite_msg_ultrasound_enc_custom_signal_detect_mode_t;

/**< @brief ID for internal SET PARAM command for a specific module */
#define ELITEMSG_CUSTOM_USM_SET_PARAM     (0x002B0002L)

/**< @brief Payload structure for the custom message set 
 * parameter */ 
typedef struct
{
	elite_msg_custom_header_t custom_header;

	elite_apr_packet_t	apr_header;

	usm_stream_cmd_set_param_t* pSetParamParams;

} elite_msg_ultrasound_custom_set_param_t;

/**< @brief ID for internal GET PARAM command from a specific module */
#define ELITEMSG_CUSTOM_USM_GET_PARAM     (0x002B0003L)

/**< @brief Payload structure for the custom message get parameter */
typedef struct
{
	elite_msg_custom_header_t custom_header;

	elite_apr_packet_t	apr_header;

	usm_stream_cmd_get_param_t* pGetParamParams;

} elite_msg_ultrasound_custom_get_param_t;

/**< @brief Structure returns from the functions ultrasound_svc_get/set_param_handler */
typedef struct
{
	uint32_t module_id;

	uint32_t param_id;

    uint32_t buf_size;

	uint8_t* pBuf;

} param_data_t;

/** 
  Set service configuration parameter. 
  A pointer to the shared memory which contains the 
  configuration is returned to the callee. Therefore, the 
  command handling must be fully synchronous. 
   
  @param [in] pSetParamMsg message which contains information 
         about the new configuration
  @param [out] pParamData pointer to the configuration data 
   
  @return
  ADSP_EOK if successful, failure code if not

  @dependencies
  None.
 */
ADSPResult ultrasound_svc_set_param_handler(elite_msg_any_t* pSetParamMsg, param_data_t* pParamData);

/** 
  Get service configuration parameter. 
  A pointer to the shared memory which contains the 
  configuration is returned to the callee. Therefore, the 
  command handling must be fully synchronous. 
   
  @param [in] pGetParamMsg message which contains information 
         about the requested configuration
  @param [out] pParamData pointer to the configuration data 
   
  @return
  ADSP_EOK if successful, failure code if not

  @dependencies
  None.
 */
ADSPResult ultrasound_svc_get_param_handler(elite_msg_any_t* pGetParamMsg, param_data_t* pParamData);



/** @} */ /* end_addtogroup ultrasound_stream */
#ifdef __cplusplus
}
#endif //__cplusplus 

#endif // #ifndef ULTRASOUND_STREAM_MGR_UTIL_H

