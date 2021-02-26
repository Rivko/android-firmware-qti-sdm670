/*========================================================================

*//** @file VoiceMsgs.h
@brief This file details the custom messages used by voice services

Copyright (c) 2009-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header:

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */


#ifndef VOICEMSGS_H
#define VOICEMSGS_H

#include "EliteMsg.h"
#include "qurt_elite.h"
#include "adsp_vsm_api.h"

/** Payload structure for configuring pkt exchange mode */
typedef struct
{
    qurt_elite_queue_t             *pBufferReturnQ;  /**< This is the queue to which this payload
                                                       buffer needs to be returned */
    qurt_elite_queue_t             *pResponseQ;      /**< This is the queue to send the ACK to.
                                                       NULL indicates no response is required*/
    uint32_t                      unClientToken;    /**< Token that should be given in the ACK. This is different
                                                       than the unResponeResult and can be used to identify
                                                       who send the ACK back by the server. */
    uint32_t                      unResponseResult; /**< This is to be filled with ACK results by the client. */
    uint32_t                      sec_opcode;      /**< This is the secondary opcode indicating
                                                       the format for the rest of payload */
    uint32_t                      pkt_exchange_mode;          /**< packet exchange mode */
} elite_msg_custom_pkt_exchange_mode_type;

/** Payload structure for configuring pkt exchange config */
typedef struct
{
    qurt_elite_queue_t             *pBufferReturnQ;  /**< This is the queue to which this payload
                                                       buffer needs to be returned */
    qurt_elite_queue_t             *pResponseQ;      /**< This is the queue to send the ACK to.
                                                       NULL indicates no response is required*/
    uint32_t                      unClientToken;    /**< Token that should be given in the ACK. This is different
                                                       than the unResponeResult and can be used to identify
                                                       who send the ACK back by the server. */
    uint32_t                      unResponseResult; /**< This is to be filled with ACK results by the client. */
    uint32_t                      sec_opcode;      /**< This is the secondary opcode indicating
                                                       the format for the rest of payload */
    vsm_config_packet_exchange_t   *param_data_ptr;          /**< parameters for OOB pkt exchange */
} elite_msg_custom_oob_pkt_exchange_config_type;


/** Payload structure for connecting two services */
typedef struct
{
    qurt_elite_queue_t             *pBufferReturnQ;  /**< This is the queue to which this payload
                                                       buffer needs to be returned */
    qurt_elite_queue_t             *pResponseQ;      /**< This is the queue to send the ACK to.
                                                       NULL indicates no response is required*/
    uint32_t                      unClientToken;    /**< Token that should be given in the ACK. This is different
                                                       than the unResponeResult and can be used to identify
                                                       who send the ACK back by the server. */
    uint32_t                      unResponseResult; /**< This is to be filled with ACK results by the client. */
    uint32_t                      sec_opcode;      /**< This is the secondary opcode indicating
                                                       the format for the rest of payload */
    elite_svc_handle_t           *svc_handle_ptr;      /**< Handle to the peer that connection
                                                       or disconnection shall be attempted with. */
} elite_msg_custom_voc_svc_connect_type;

/** Payload structure for controlling volume */
typedef struct
{
    qurt_elite_queue_t             *pBufferReturnQ;  /**< This is the queue to which this payload
                                                       buffer needs to be returned */
    qurt_elite_queue_t             *pResponseQ;      /**< This is the queue to send the ACK to.
                                                       NULL indicates no response is required*/
    uint32_t                      unClientToken;    /**< Token that should be given in the ACK. This is different
                                                       than the unResponeResult and can be used to identify
                                                       who send the ACK back by the server. */
    uint32_t                      unResponseResult; /**< This is to be filled with ACK results by the client. */
    uint32_t                      sec_opcode;      /**< This is the secondary opcode indicating
                                                       the format for the rest of payload */
    uint16_t                      volume;          /**< Volume control in Q13 */
} elite_msg_custom_voc_set_volume_type;


/** Payload structure used to configure Host PCM */
typedef struct
{
    qurt_elite_queue_t             *pBufferReturnQ;  /**< This is the queue to which this payload
                                                       buffer needs to be returned */
    qurt_elite_queue_t             *pResponseQ;      /**< This is the queue to send the ACK to.
                                                       NULL indicates no response is required*/
    uint32_t                      unClientToken;    /**< Token that should be given in the ACK. This is different
                                                       than the unResponeResult and can be used to identify
                                                       who send the ACK back by the server. */
    uint32_t                      unResponseResult; /**< This is to be filled with ACK results by the client. */
    uint32_t                      sec_opcode;      /**< This is the secondary opcode indicating
                                                       the format for the rest of payload */
    uint32_t                      apr_handle;       /**< Handle used for APR packet alloc/send */
    elite_apr_packet_t           *apr_packet_ptr;   /**< pointer to original APR packet to start/stop host pcm */
} elite_msg_custom_voc_config_host_pcm_type;


/** Payload structure used to configure timing params */
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;  /**< This is the queue to which this payload
                                         buffer needs to be returned*/
    qurt_elite_queue_t   *pResponseQ;      /**< This is the queue to send the ACK to.
                                        NULL indicates no response is required*/
    uint32_t            unClientToken;    /**< Token that should be given in the ACK. This is different
                                            than the unResponeResult and can be used to identify
                                            who send the ACK back by the server. */
    uint32_t            unResponseResult; /**< This is to be filled with ACK results by the client. */
    uint32_t            sec_opcode;      /**< This is the secondary opcode indicating
                                             the format for the rest of payload */
    int32_t             *param_data_ptr;      /**< Parameter value that are to be get/set*/
} elite_msg_custom_voc_timing_param_type;

/** Payload structure for enabling loopback mode on stream or device sessions */
typedef struct
{
    qurt_elite_queue_t       *pBufferReturnQ;  /**< This is the queue to which this payload
                                         buffer needs to be returned*/
    qurt_elite_queue_t       *pResponseQ;      /**< This is the queue to send the ACK to.
                                        NULL indicates no response is required*/
    uint32_t            unClientToken;    /**< Token that should be given in the ACK. This is different
                                            than the unResponeResult and can be used to identify
                                            who send the ACK back by the server. */
    uint32_t            unResponseResult; /**< This is to be filled with ACK results by the client. */
    uint32_t            param_id;         /**< This is the parameter ID. The rest
                                            of payload is derived based on this ID.
                                             For this type, unParamId == ELITEMSG_PARAM_ID_CAL
                                          */
    uint32_t            loop_back_enable; /**< Enable/disable loopback
                                               0 - Disable, 1- - Enable
                                            */

} voice_msg_custom_set_loopback_enable;

/** Payload structure for connecting two services */
typedef struct
{
    qurt_elite_queue_t             *pBufferReturnQ;  /**< This is the queue to which this payload
                                                       buffer needs to be returned */
    qurt_elite_queue_t             *pResponseQ;      /**< This is the queue to send the ACK to.
                                                       NULL indicates no response is required*/
    uint32_t                      unClientToken;    /**< Token that should be given in the ACK. This is different
                                                       than the unResponeResult and can be used to identify
                                                       who send the ACK back by the server. */
    uint32_t                      unResponseResult; /**< This is to be filled with ACK results by the client. */
    uint32_t                      sec_opcode;      /**< This is the secondary opcode indicating
                                                       the format for the rest of payload */
    elite_svc_handle_t           *svc_handle_ptr;      /**< Handle to the peer that connection
                                                       or disconnection shall be attempted with. */
    uint32_t record_mode;
   /**< recording mode that can be choosen by the user. Values are:
      - ::VSM_RECORD_TX_RX_STEREO -- record tx(left) and rx(right) in stereo mode   \n
      - ::VSM_RECORD_TX_RX_MIXING -- record mixed data of tx and rx paths \n
   */
    uint16_t           aud_ref_port;
    void               *afe_drift_ptr;
    /**< Pointer to drift struct, return from AFE connect in VPM. */

} elite_msg_custom_voc_svc_connect_record_type;

/** Payload structure for controlling muting */
typedef struct
{
    qurt_elite_queue_t             *pBufferReturnQ;  /**< This is the queue to which this payload
                                                       buffer needs to be returned */
    qurt_elite_queue_t             *pResponseQ;      /**< This is the queue to send the ACK to.
                                                       NULL indicates no response is required*/
    uint32_t                      unClientToken;    /**< Token that should be given in the ACK. This is different
                                                       than the unResponeResult and can be used to identify
                                                       who send the ACK back by the server. */
    uint32_t                      unResponseResult; /**< This is to be filled with ACK results by the client. */
    uint32_t                      sec_opcode;      /**< This is the secondary opcode indicating
                                                       the format for the rest of payload */
    uint16_t                      mute;
    /**< Indicates command.

       For VSM: 0 - unmute, 1 - mute, 2 - enable CNG (tx only)
       For VSM: 0 - unmute, 1 - mute
    */
    uint32_t                      ramp_duration;
    /**< duration over which mute comes into effect gradually.
         it takes values from 0 to 5000msec
    */
} elite_msg_custom_voc_set_soft_mute_type;

/** Payload structure for setting sampling rate on stream PP on tx and rx*/
typedef struct
{
    qurt_elite_queue_t             *pBufferReturnQ;  /**< This is the queue to which this payload
                                                       buffer needs to be returned */
    qurt_elite_queue_t             *pResponseQ;      /**< This is the queue to send the ACK to.
                                                       NULL indicates no response is required*/
    uint32_t                      unClientToken;    /**< Token that should be given in the ACK. This is different
                                                       than the unResponeResult and can be used to identify
                                                       who send the ACK back by the server. */
    uint32_t                      unResponseResult; /**< This is to be filled with ACK results by the client. */
    uint32_t                      sec_opcode;      /**< This is the secondary opcode indicating
                                                       the format for the rest of payload */
    uint32_t                      tx_samp_rate;
    /**<
       For VSM: valid sampling rates are : 8000, 16000, 48000 for stream PP
       and 8000, 16000, 32000, 44100, 48000 for encoder
    */
    uint32_t                      rx_samp_rate;
    /**< For VSM: valid sampling rates are : 8000, 16000, 48000 for stream PP
       and it is a dont care for decoder    */
} elite_msg_custom_voc_stream_set_sampling_type;

/** Payload structure for setting tty mode on tx and rx*/
typedef struct
{
    qurt_elite_queue_t             *pBufferReturnQ;  /**< This is the queue to which this payload
                                                       buffer needs to be returned */
    qurt_elite_queue_t             *pResponseQ;      /**< This is the queue to send the ACK to.
                                                       NULL indicates no response is required*/
    uint32_t                      unClientToken;    /**< Token that should be given in the ACK. This is different
                                                       than the unResponeResult and can be used to identify
                                                       who send the ACK back by the server. */
    uint32_t                      unResponseResult; /**< This is to be filled with ACK results by the client. */
    uint32_t                      sec_opcode;      /**< This is the secondary opcode indicating
                                                       the format for the rest of payload */
    uint32_t                      tty_mode;
    /**< Specifies the TTY mode of operation.
         @values
         - #VSM_TTY_MODE_OFF
         - #VSM_TTY_MODE_HCO
         - #VSM_TTY_MODE_VCO
         - #VSM_TTY_MODE_FULL @tablebulletend */
} elite_msg_custom_voc_stream_set_tty_mode;

/** Payload structure for getting KPPS settings from dynamic services */
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;  /**< Queue to which this payload buffer
                                           must be returned. */

    qurt_elite_queue_t   *pResponseQ;      /**< Queue to which to send the
                                           acknowledgment. NULL indicates that
                                           no response is required. */

    uint32_t             unClientToken;    /**< Token to be given in the
                                           acknowledgment. This is different
                                           from the unResponeResult and can be
                                           used to identify which service sent
                                           the response to the server. */

    uint32_t             unResponseResult; /**< Response result that the server
                                           sends back to the client. */
    uint32_t             sec_opcode;       /**< This is the secondary opcode indicating
                                           the format for the rest of payload */

    void*                pnKpps;          /**< Points to kpps structure that
                                           receiver writes to */ 
                                           
} elite_msg_custom_kpps_type;

/** Payload structure for getting delay settings from dynamic services */
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;  /**< Queue to which this payload buffer
                                           must be returned. */

    qurt_elite_queue_t   *pResponseQ;      /**< Queue to which to send the
                                           acknowledgment. NULL indicates that
                                           no response is required. */

    uint32_t             unClientToken;    /**< Token to be given in the
                                           acknowledgment. This is different
                                           from the unResponeResult and can be
                                           used to identify which service sent
                                           the response to the server. */

    uint32_t             unResponseResult; /**< Response result that the server
                                           sends back to the client. */
    uint32_t             sec_opcode;       /**< This is the secondary opcode indicating
                                           the format for the rest of payload */

    void*                delay_ptr;        /**< Points to delay structure that
                                           receiver writes to */ 
                                           
} elite_msg_custom_delay_type;

/** @brief Structure to define the fields required for Dynamnic loading
   Once dlopen makes the shared object available for ADSP execution, dlsym is exercised
   by the ADSP (AVCS) to identify the function handles for the APPI module in the
   shared object. Each shared object, compliant with APPI requires to expose
   its getsize and init routine handles
*/


/** Payload structure to send the shared object handles to the VPM */
typedef struct
{
    qurt_elite_queue_t     *pBufferReturnQ; /**< Queue to which this payload
                                            buffer must be returned. */

    qurt_elite_queue_t     *pResponseQ;     /**< Queue to which to send the
                                            acknowledgment. NULL indicates
                                            that no response is required. */

    uint32_t           unClientToken;  /**< Token to be given in the
                                            acknowledgment. This is different
                                            from the unResponseResult and can
                                            be used to identify which service
                                            sent the response to the server. */

    uint32_t           unResponseResult; /**< Response result that the server
                                              sends back to the client. */

    uint32_t           unSecOpCode;    /**< Secondary operation code that
                                            indicates the format for the rest
                                            of the payload. */
} elite_msg_custom_voice_shared_objects_type;

/** Payload structure for registering/unregistering from an event */
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;  /**< Queue to which this payload buffer
                                           must be returned. */

    qurt_elite_queue_t   *pResponseQ;      /**< Queue to which to send the
                                           acknowledgment. NULL indicates that
                                           no response is required. */

    uint32_t             unClientToken;    /**< Token to be given in the
                                           acknowledgment. This is different
                                           from the unResponeResult and can be
                                           used to identify which service sent
                                           the response to the server. */

    uint32_t             unResponseResult; /**< Response result that the server
                                           sends back to the client. */
    uint32_t             sec_opcode;       /**< This is the secondary opcode indicating
                                           the format for the rest of payload */

    uint32_t             event_id;        /**< Event ID to register/unregister */ 
                                           
} elite_msg_custom_event_reg_type;

/** Payload structure for setting VOICEPROC_SET_EP_ID
    This custom command is to set end-point ids for the dynamic services.
    Based on the end-point ids dynamic service will work either in RT
    (input sync and output deliveries are tick based) or  FTRT mode 
    (input sync and output deliveries are push based based) 
    The command will be accepted only in stop state and supported only by VPTx */

#define INVALID_PORT (0xFFFF)
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;  /**< Queue to which this payload buffer
                                           must be returned. */

    qurt_elite_queue_t   *pResponseQ;      /**< Queue to which to send the
                                           acknowledgment. NULL indicates that
                                           no response is required. */

    uint32_t             unClientToken;    /**< Token to be given in the
                                           acknowledgment. This is different
                                           from the unResponeResult and can be
                                           used to identify which service sent
                                           the response to the server. */

    uint32_t             unResponseResult; /**< Response result that the server
                                           sends back to the client. */

    uint32_t             sec_opcode;       /**< This is the secondary opcode indicating
                                           the format for the rest of payload 
                                           should be set to VOICEPROC_SET_EP_ID for this
                                            payload*/

    uint16_t             ep_id[2];        /**< EP ids for primary and secondary port. if 
                                           secondary port is not applicable should be set 
                                           to 0xFFFF (INVALID_PORT)  */ 
                                           
} elite_msg_custom_set_ep_id;

/** Payload structure for VOICE_CMD_SET_PARAM_V3 FADD message */
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;  /**< Queue to which this payload buffer
                                           must be returned. */

    qurt_elite_queue_t   *pResponseQ;      /**< Queue to which to send the
                                           acknowledgment. NULL indicates that
                                           no response is required. */

    uint32_t             unClientToken;    /**< Token to be given in the
                                           acknowledgment. This is different
                                           from the unResponeResult and can be
                                           used to identify which service sent
                                           the response to the server. */

    uint32_t             unResponseResult; /**< Response result that the server
                                           sends back to the client. */
    uint32_t             sec_opcode;       /**< This is the secondary opcode indicating
                                           the format for the rest of payload */

    uint32_t             cal_handle;      /**< Cal handle identifying cal to be applied */ 
                                           
} elite_msg_custom_set_param_v3_type;

/** Payload structure for VSS_ISTREAM_CMD_SET_VOC_PARAM FADD message */
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;  /**< Queue to which this payload buffer
                                           must be returned. */

    qurt_elite_queue_t   *pResponseQ;      /**< Queue to which to send the
                                           acknowledgment. NULL indicates that
                                           no response is required. */

    uint32_t             unClientToken;    /**< Token to be given in the
                                           acknowledgment. This is different
                                           from the unResponeResult and can be
                                           used to identify which service sent
                                           the response to the server. */

    uint32_t             unResponseResult; /**< Response result that the server
                                           sends back to the client. */
    uint32_t             sec_opcode;       /**< This is the secondary opcode indicating
                                           the format for the rest of payload */

    void                 *payload_address; /**< payload address pointing to vocoder property to be applied */

    uint16_t             payload_size;     /**< Payload size in bytes for the memory pointed by payload_address. */

} elite_msg_custom_set_voc_param_type;


/** Payload structure for VSM_CMD_ENC_SET_RATE FADD message */
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;  /**< Queue to which this payload buffer
                                           must be returned. */

    qurt_elite_queue_t   *pResponseQ;      /**< Queue to which to send the
                                           acknowledgment. NULL indicates that
                                           no response is required. */

    uint32_t             unClientToken;    /**< Token to be given in the
                                           acknowledgment. This is different
                                           from the unResponeResult and can be
                                           used to identify which service sent
                                           the response to the server. */

    uint32_t             unResponseResult; /**< Response result that the server
                                           sends back to the client. */
    uint32_t             sec_opcode;       /**< This is the secondary opcode indicating
                                           the format for the rest of payload */

    uint32_t             media_type;
    uint32_t             encoder_rate;
                                           
} elite_msg_custom_set_enc_rate_type;


#endif /* VOICEMSGS_H */
