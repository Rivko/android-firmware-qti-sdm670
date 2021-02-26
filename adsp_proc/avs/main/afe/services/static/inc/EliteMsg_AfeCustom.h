/*========================================================================
  This file contains AFE custom messaging related interface details

  Copyright (c) 2009-2012, 2017 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/inc/EliteMsg_AfeCustom.h#1 $
 ====================================================================== */
#ifndef _AFE_CUST_MSG_H_
#define _AFE_CUST_MSG_H_

/*==========================================================================
  Include Files
  ========================================================================== */
#include "EliteMsg.h"
#include "AFEInterface.h"
#include "AFEVfrHandler.h"
#include "AFECodecService.h"
#include "AFEFeedback.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*==========================================================================
  Macro\Constant definitions
  ========================================================================== */
/** @name Secondary Operation Codes

  The secondary operation code for ELITE_CUSTOM_MSG is 32 bits and
  must be globally unique. Individual services can define their globally
  unique secondary operation codes.

  The recommended method is:
  - The MSB 16 bits must be the MSB 16 bits of the service ID that defines
    this parameter ID. \n
  - The LSB 16 bits can be defined by the service itself.

  If the MSB 16 bits are all zeros, it indicates that this is a common (kernel)
  CUSTOM_MSG.

  The macros for the secondary operation codes are:
*/

/** @{ */
/** Indicates that the rest of the payload is an AFE connect. */
#define ELITEMSG_CUSTOM_AFECONNECT_REQ                 0x00200001
/** Indicates that the rest of the payload is an AFE disconnect. */
#define ELITEMSG_CUSTOM_AFEDISCONNECT_REQ              0x00200002
/** Indicates that the rest of the payload is an AFE disable. */
#define ELITEMSG_CUSTOM_AFECLIENTDISABLE               0x00200005
/** Indicates that the rest of the payload is an AFE VFR configuration. */
#define ELITEMSG_CUSTOM_VFR_SYNC                       0x00200006
/** Indicates that the rest of the payload is an AFE VFR disable. */
#define ELITEMSG_CUSTOM_VFR_DISABLE                    0x00200007
/** Indicates the terminate signal to a thread, that can be used to break the thread */
#define ELITEMSG_CUSTOM_TERMINATE_THREAD               0x0020000c
/** Indicates that the rest of the payload is an AFE riva port
 * command from client processor.*/
#define ELITEMSG_CUSTOM_RIVA_IPC_COMMAND               0x0020000d
/** Indicates to AFE real time proxy port data manager that port id in the payload has started */
#define ELITEMSG_CUSTOM_AFE_RT_PROXY_PORT_START        0x0020000e
/** Indicates to AFE real time proxy port data manager that port id in the payload has stopped */
#define ELITEMSG_CUSTOM_AFE_RT_PROXY_PORT_STOP         0x0020000f
/** a message from AFE service to any other service indicating them to read the drift_info ptr  */
#define ELITEMSG_CUSTOM_AFE_AVT_DRIFT_UPDATE           0x00200010
/** Indicates that the rest of the payload is an AFE loopback connect. */
#define ELITEMSG_CUSTOM_AFE_LOOPBACK_CONNECT_REQ       0x00200011
/** Indicates that the APR payload contains set param for AANC module */
#define ELITEMSG_CUSTOM_SET_AANC_APR_PKT               0x00200012
/** Register for codec interrupt with codec handler service */
#define ELITEMSG_CUSTOM_CDC_INT_REGISTER_REQ           0x00200013
/** De-Register for codec interrupt with codec handler service */
#define ELITEMSG_CUSTOM_CDC_INT_DEREGISTER_REQ         0x00200014
/** Codec service informs the registered clients with this
 *  messges about codec interrupt.  */
#define ELITEMSG_CUSTOM_CDC_INT_CLIENT_SIG             0x00200015
/** Clients need to provide the ACK message to codec service for
 *  the informed interrupt based on the configuration. */
#define ELITEMSG_CUSTOM_CDC_INT_CLIENT_ACK             0x00200016
/** Indicates that bg thread data is to be init-ed */
#define ELITEMSG_CUSTOM_AFEBGT_INIT                    0x00200017
/** Indicates that bg thread data is to be de-inited */
#define ELITEMSG_CUSTOM_AFEBGT_DEINIT                  0x00200018
/** Indicates that bg thread is to be destroyed */
#define ELITEMSG_CUSTOM_AFEBGT_DESTROY                 0x00200019
/** Custom cmd for enabling the afe clients */
#define ELITEMSG_CUSTOM_AFECLIENTENABLE                0x0020001A
/** Custom cmd for resetting the afe sw mad clients */
#define ELITEMSG_CUSTOM_AFECLIENT_DC_RESET             0x0020001B
/** Indicates that the rest of the payload is an AFE connect. */
#define ELITEMSG_CUSTOM_AFE_CUST_CLIENT_CONNECT_REQ    0x0020001C
/** Indicates that the rest of the payload is an AFE connect. */
#define ELITEMSG_CUSTOM_AFE_CUST_CLIENT_DISCONNECT_REQ 0x0020001D
/*Custom cmd to get AANC parameters */
#define ELITEMSG_CUSTOM_GET_AANC_APR_PKT               0x0020001E
/* Custom cmd to inform disconnect inform of a marker  */
#define ELITEMSG_CUSTOM_AFECLIENT_DISCONNECT_MARKER    0x0020001F
/*Custom cmd for client to register with AFE with some criterion */
#define ELITEMSG_CUSTOM_AFE_CLIENT_CRITERIA_REGISTER   0x00200020
/** Indicates that bg thread is to be destroyed */
#define ELITEMSG_CUSTOM_BGT_DESTROY                    0x00200021

/** Indicates AFE client's media format update */
#define ELITEMSG_CUSTOM_AFECLIENT_MEDIA_FMT_UPDATE     0x00200022

/** Custom message to initaite device recovery from error
 *  state (e.g. stall due to signal miss) */
#define ELITEMSG_CUSTOM_DEV_ERR_RECOVERY               0x00200023

/** Indicates hdmi-over-dp update */
#define ELITEMSG_CUSTOM_HDMI_OVER_DP                   0x00200024

/** Indicates avtimer scheduled DTMF config */
#define ELITEMSG_CUSTOM_TIMED_DTMF_CONFIG              0x00200025
/** @} */ /* end_name_group Secondary Operation Code */

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/
/** @brief Payload structure for ELITE_CUSTOM_MSG and
  sec_op_code == ELITEMSG_CUSTOM_AFECONNECT/STARTDMA Header.
 */
typedef struct elite_msg_custom_afe_header
{
    qurt_elite_queue_t            *buffer_returnq_ptr;
                                  /**< Queue to which this payload buffer must
                                       be returned. */
    qurt_elite_queue_t            *responseq_ptr;
                                  /**< Queue to which to send the
                                       acknowledgement. NULL indicates that
                                       no response is required. */
    uint32_t                      client_token;
                                  /**< Token that is to be given in the
                                       acknowledgement. This is different than
                                       the response_result and can be used to
                                       identify who sent the acknowledgement
                                       back by the server. */
    uint32_t                      response_result;
                                  /**< This is to be filled with the
                                       acknowledgement results by the client.*/
    uint32_t                      sec_op_code;
                                  /**< Secondary operation code indicating
                                       the format for the rest of the
                                       payload. */
    uint16_t                      afe_id;
                                  /**< AFE port ID and direction. */
    uint16_t                      payload_size;
                                  /**< payload size. */
    void						  *payload_ptr;
                                  /**< payload pointer. */
} elite_msg_custom_afe_header_t;

/** @brief Payload structure for ELITE_CUSTOM_MSG and
  sec_op_code == ELITEMSG_CUSTOM_AFECONNECT_REQ /
  ELITEMSG_CUSTOM_AFEDISCONNECT_REQ.
 */
typedef struct elite_msg_custom_afe_connect
{
    qurt_elite_queue_t         			*buffer_returnq_ptr;
										/**< Queue to which this payload buffer must
											be returned. */
    qurt_elite_queue_t          		*responseq_ptr;
										/**< Queue to which to send the
											acknowledgement. NULL indicates that
											no response is required. */
    uint32_t                    		client_token;
										/**< Token that is to be given in the
											acknowledgement. This is different than
											the response_result and can be used to
											identify who sent the acknowledgement. */
    uint32_t                    		response_result;
										/**< Result of the AfeConnect operation filled
											by the AFE and used by the AFE clients
											upon acknowledgement. */
    uint32_t                    		sec_op_code;
										/**< Secondary operation code indicating
											the format for the rest of the payload. */
    uint16_t                    		afe_id;
										/**< AFE port ID and direction. */
    uint16_t                    		reserved;
										/**< Reserved for 32-bit alignment. */
    elite_svc_handle_t          		*svc_handle_ptr;
										/**< Handle to the peer with which the
											connection or disconnection is to be
											attempted. */
    afe_client_t                		afe_client;
										/**< Client's information to the AFE. */
    volatile const afe_drift_info_t	  	*afe_drift_ptr;
										/**< read-only drift ptr for the clients */
} elite_msg_custom_afe_connect_t;


/** @brief Payload structure for ELITE_CUSTOM_MSG and
   sec_op_code == ELITEMSG_CUSTOM_VFR_SYNC / ELITEMSG_CUSTOM_VFR_DISABLE.
 */
typedef struct elite_msg_custom_afe_vfr_config_info
{
    qurt_elite_queue_t         *buffer_returnq_ptr;
                               /**< Queue to which this payload buffer must
							        be returned. */
    qurt_elite_queue_t         *responseq_ptr;
							   /**< Queue to which to send the
							        acknowledgement. NULL indicates that
							        no response is required. */
    uint32_t                   client_token;
							   /**< Token that is to be given in the
							        acknowledgement. This is different than
							        the response_result and can be used to
							        identify who sent the acknowledgement
							        back by the server. */
    uint32_t                   response_result;
							   /**< This is to be filled with the
							        acknowledgement results by the client. */
    uint32_t                   sec_op_code;
							   /**< Secondary operation code indicating
							        the format for the rest of the payload. */
    vfr_client_info_t         client_vfr_info;
                               /**< Structure containing the VFR queue
                                    information from voice. */
} elite_msg_custom_afe_vfr_config_info_t;

/** @brief Payload structure for the VFR response with drift or resync
  information.
 */
typedef struct elite_msg_custom_vfr_info
{
    qurt_elite_queue_t         *buffer_returnq_ptr;
                               /**< Queue to which this payload buffer must
							        be returned. */
    qurt_elite_queue_t         *responseq_ptr;
							   /**< Queue to which to send the
							        acknowledgement. NULL indicates that
							        no response is required. */
    uint32_t                   client_token;
							   /**< Token that is to be given in the
							        acknowledgement. This is different than
							        the response_result and can be used to
							        identify who sent the acknowledgement
							        back by the server. */
    uint32_t                   response_result;
							   /**< This is to be filled with the
							        acknowledgement results by the client. */
    uint32_t                   sec_op_code;
							   /**< Secondary operation code indicating
							        the format for the rest of the payload. */
    vfr_info_t                 vfr_info;
                               /**< Response for VFR  with drift/resync
                                    information. */
} elite_msg_custom_vfr_info_t;

/** @brief Payload structure for the riva IPC command,
    opcode with ELITEMSG_CUSTOM_RIVA_IPC_COMMAND.
 */
typedef struct elite_msg_custom_riva_ipc
{
    qurt_elite_queue_t         *buffer_returnq_ptr;
                               /**< Queue to which this payload buffer must
							        be returned. */
    qurt_elite_queue_t         *responseq_ptr;
							   /**< Queue to which to send the
							        acknowledgement. NULL indicates that
							        no response is required. */
    uint32_t                   client_token;
							   /**< Token that is to be given in the
							        acknowledgement. This is different than
							        the unResponseResult and can be used to
							        identify who sent the acknowledgement
							        back by the server. */
    uint32_t                   response_result;
							   /**< This is to be filled with the
							        acknowledgement results by the client. */
    uint32_t                   sec_op_code;
							   /**< Secondary operation code indicating
							        the format for the rest of the payload. */
} elite_msg_custom_riva_ipc_t;


/** @brief Payload structure for the avt drift update message,
    opcode with ELITEMSG_CUSTOM_AVT_DRIFT_UPDATE.
 */
typedef struct elite_msg_custom_avt_drift_update
{
    qurt_elite_queue_t      *buffer_returnq_ptr;
							/**< Queue to which this payload buffer must
                               be returned. */
    qurt_elite_queue_t      *responseq_ptr;
							/**< Queue to which to send the
                             acknowledgement. NULL indicates that
                             no response is required. */
    uint32_t                client_token;
							/**< Token that is to be given in the
                             acknowledgement. This is different than
                             the unResponseResult and can be used to
                             identify who sent the acknowledgement
                             back by the server. */
    uint32_t                response_result;
							/**< This is to be filled with the
                             acknowledgement results by the client. */
    uint32_t                sec_op_code;
							/**< Secondary operation code indicating
                             the format for the rest of the payload. */
    uint16_t                port_id;
							/**< port id from which the message originates */
} elite_msg_custom_avt_drift_update_t;

/** @brief Payload structure for loopback connect message.
 *  This message is sent from TX port to RX port during loopback set-up.
 */
typedef struct elite_msg_custom_afe_loopback_connect
{
   elite_msg_custom_afe_connect_t   connect_msg;
									/**< connect msg payload. note this should be on the top 
									  because the payload is casted to connect msg */
   bool_t                           is_rate_match;
                                    /**< whether this tx port client needs any rate matching */
   volatile uint64_t                *tx_port_acc_avt_drift_info;
                                    /**< drift info of the tx port */
} elite_msg_custom_afe_loopback_connect_t;

/** @brief Payload structure for ELITE_CUSTOM_MSG and
   sec_op_code == ELITEMSG_CUSTOM_REGISTER_CDC_INTERRUPT, ELITEMSG_CUSTOM_DEREGISTER_CDC_INTERRUPT
   ELITEMSG_CUSTOM_CDC_INT_CLIENT_SIG, ELITEMSG_CUSTOM_CDC_INT_CLIENT_ACK
 */
typedef struct elite_msg_custom_afe_cdc_int_info
{
    qurt_elite_queue_t         *buffer_returnq_ptr;
                               /**< Queue to which this payload buffer must
							        be returned. */
    qurt_elite_queue_t         *responseq_ptr;
							   /**< Queue to which to send the
							        acknowledgement. NULL indicates that
							        no response is required. */
    uint32_t                   client_token;
							   /**< Token that is to be given in the
							        acknowledgement. This is different than
							        the response_result and can be used to
							        identify who sent the acknowledgement
							        back by the server. */
    uint32_t                   response_result;
							   /**< This is to be filled with the
							        acknowledgement results by the client. */
    uint32_t                   sec_op_code;
							   /**< Secondary operation code indicating
							        the format for the rest of the payload. */
    cdc_client_info_t         client_info;
                               /**< Structure containing the client information for codec interrupt handler. */
} elite_msg_custom_afe_cdc_int_info_t;

/**
 * Connect command between VI processing and Rx processing 
 */
typedef struct elite_msg_custom_afe_cust_client_connect_t
{
    qurt_elite_queue_t         			*buffer_returnq_ptr;
										/**< Queue to which this payload buffer must
											be returned. */
    qurt_elite_queue_t          		*responseq_ptr;
										/**< Queue to which to send the
											acknowledgement. NULL indicates that
											no response is required. */
    uint32_t                    		client_token;
										/**< Token that is to be given in the
											acknowledgement. This is different than
											the response_result and can be used to
											identify who sent the acknowledgement. */
    uint32_t                    		response_result;
										/**< Result of the AfeConnect operation filled
											by the AFE and used by the AFE clients
											upon acknowledgement. */
    uint32_t                    		sec_op_code;
										/**< Secondary operation code indicating
											the format for the rest of the payload. */
    uint16_t                    		afe_id;
										/**< AFE port ID and direction. */
    uint16_t                    		reserved;
										/**< Reserved for 32-bit alignment. */
    elite_svc_handle_t          		*svc_handle_ptr;
										/**< Handle to the peer with which the
											connection or disconnection is to be
											attempted. */
    afe_custom_client_t						afe_client;
										/**< Client's information to the AFE. */
   
} elite_msg_custom_afe_cust_client_connect_t;

/**
 * disconnect marker command from client to AFE, indicating disconnect about to come
 */

typedef struct elite_msg_custom_client_disconnect_marker
{
    qurt_elite_queue_t                 *buffer_returnq_ptr;
                              /**< Queue to which this payload buffer must
                                 be returned. */
    qurt_elite_queue_t              *responseq_ptr;
                              /**< Queue to which to send the
                                 acknowledgement. NULL indicates that
                                 no response is required. */
    uint32_t                        client_token;
                              /**< Token that is to be given in the
                                 acknowledgement. This is different than
                                 the response_result and can be used to
                                 identify who sent the acknowledgement. */
    uint32_t                        response_result;
                              /**< Result of the AfeConnect operation filled
                                 by the AFE and used by the AFE clients
                                 upon acknowledgement. */
    uint32_t                        sec_op_code;
                              /**< Secondary operation code indicating
                                 the format for the rest of the payload. */
    uint16_t                            afe_id;
                                 /**< AFE port ID and direction. */
    uint16_t                        reserved;
                              /**< Reserved for 32-bit alignment. */
    elite_svc_handle_t              *svc_handle_ptr;
                              /**< Handle to the peer with which the
                                 connection or disconnection is to be
                                 attempted. */

}elite_msg_custom_afeclient_disconnect_marker_t;

/** @brief Payload structure for ELITE_CUSTOM_MSG and
  sec_op_code == ELITEMSG_CUSTOM_AFE_REGISTER_CLIENT
 */
typedef struct elite_msg_custom_afe_register_client
{
    qurt_elite_queue_t              *buffer_returnq_ptr;
                    /**< Queue to which this payload buffer must
                      be returned. */
    qurt_elite_queue_t              *responseq_ptr;
                    /**< Queue to which to send the
                      acknowledgement. NULL indicates that
                      no response is required. */
    uint32_t                        client_token;
                    /**< Token that is to be given in the
                      acknowledgement. This is different than
                      the response_result and can be used to
                      identify who sent the acknowledgement. */
    uint32_t                        response_result;
                    /**< Result of the Aferegister operation filled
                      by the AFE and used by the AFE clients
                      upon acknowledgement. */
    uint32_t                        sec_op_code;
                    /**< Secondary operation code indicating
                      the format for the rest of the payload. */
    uint16_t                        afe_port_id;
                    /**< AFE port ID and direction. */
    uint16_t                        reserved;
                    /**< Reserved for 32-bit alignment. */
    elite_svc_handle_t              *svc_handle_ptr;
                    /**< Handle to the peer with which
                      is registering. */
    afe_client_register_criteria_type   criteria;
                    /**< Criterion for which Client wants to register with AFE*/
    bool_t                         criteria_enable;
                    /**< TRUE = Enable ; FALSE = Disable*/
} elite_msg_custom_afe_client_criteria_register_t;

/** @brief Payload structure for ELITE_CUSTOM_MSG and
  sec_op_code == ELITEMSG_CUSTOM_SET_AANC_APR_PKT and ELITEMSG_CUSTOM_GET_AANC_APR_PKT
 */
typedef struct elite_msg_custom_aanc_info
{
  qurt_elite_queue_t            *buffer_returnq_ptr;
  /**< Queue to which this payload buffer must
                                     be returned. */
  qurt_elite_queue_t            *responseq_ptr;
  /**< Queue to which to send the
                                     acknowledgement. NULL indicates that
                                     no response is required. */
  uint32_t                      client_token;
  /**< Token that is to be given in the
                                     acknowledgement. This is different than
                                     the response_result and can be used to
                                     identify who sent the acknowledgement
                                     back by the server. */
  uint32_t                      response_result;
  /**< This is to be filled with the
                                     acknowledgement results by the client.*/
  uint32_t                      sec_op_code;
  /**< Secondary operation code indicating
                                     the format for the rest of the
                                     payload. */
  uint16_t                      afe_id;
  /**< AFE port ID and direction. */
  uint16_t                      reserved;
  /**< Reserved for 32-bit alignment. */
  void                         *tx_port_ptr;
  /**< The port on which the set param command has been sent */
  elite_apr_packet_t           *apr_pkt_ptr;
  /**< This is pointer to the APR packet. */
  uint32_t                     payloadSize;
  /**< This is size of the APR payload. */
  uint32_t                     apr_payload_start_addr;
} elite_msg_custom_aanc_info_t;


typedef struct elite_msg_custom_client_media_format_update
{
    qurt_elite_queue_t                 *buffer_returnq_ptr;
                              /**< Queue to which this payload buffer must
                                 be returned. */
    qurt_elite_queue_t              *responseq_ptr;
                              /**< Queue to which to send the
                                 acknowledgement. NULL indicates that
                                 no response is required. */
    uint32_t                        client_token;
                              /**< Token that is to be given in the
                                 acknowledgement. This is different than
                                 the response_result and can be used to
                                 identify who sent the acknowledgement. */
    uint32_t                        response_result;
                              /**< Result of the AfeConnect operation filled
                                 by the AFE and used by the AFE clients
                                 upon acknowledgement. */
    uint32_t                        sec_op_code;
                              /**< Secondary operation code indicating
                                 the format for the rest of the payload. */
    uint16_t                            afe_id;
                                 /**< AFE port ID and direction. */
    uint16_t                        reserved;
                              /**< Reserved for 32-bit alignment. */
    elite_svc_handle_t              *svc_handle_ptr;
                              /**< Handle to the peer with which the
                                 connection or disconnection is to be
                                 attempted. */

}elite_msg_custom_client_media_format_update_t;

/** @brief Payload structure for ELITE_CUSTOM_MSG and
   sec_op_code == ELITEMSG_CUSTOM_HDMI_OVER_DP
 */
typedef struct elite_msg_custom_afe_hdmi_over_dp_info
{
    qurt_elite_queue_t         *buffer_returnq_ptr;
                               /**< Queue to which this payload buffer must
							        be returned. */
    qurt_elite_queue_t         *responseq_ptr;
							   /**< Queue to which to send the
							        acknowledgement. NULL indicates that
							        no response is required. */
    uint32_t                   client_token;
							   /**< Token that is to be given in the
							        acknowledgement. This is different than
							        the response_result and can be used to
							        identify who sent the acknowledgement
							        back by the server. */
    uint32_t                   response_result;
							   /**< This is to be filled with the
							        acknowledgement results by the client. */
    uint32_t                   sec_op_code;
                              /**< Secondary operation code indicating
                                   the format for the rest of the payload. */
   void                       *afe_port_ptr;
                              /**< AFE port pointer. */

   uint32_t                    event_mask;
                              /**< Mask contains DP event info  */

} elite_msg_custom_afe_hdmi_over_dp_info_t;

/** @brief Payload structure for ELITE_CUSTOM_MSG and
   sec_op_code == ELITEMSG_CUSTOM_TIMED_DTMF_CONFIG
 */
typedef struct elite_msg_custom_afe_tone_generate_cfg
{
    qurt_elite_queue_t         *buffer_returnq_ptr;
                               /**< Queue to which this payload buffer must
                                    be returned. */
    qurt_elite_queue_t         *responseq_ptr;
                               /**< Queue to which to send the
                                    acknowledgement. NULL indicates that
                                    no response is required. */
    uint32_t                   client_token;
                               /**< Token that is to be given in the
                                    acknowledgement. This is different than
                                    the response_result and can be used to
                                    identify who sent the acknowledgement
                                    back by the server. */
    uint32_t                   response_result;
                               /**< This is to be filled with the
                                    acknowledgement results by the client. */
    uint32_t                   sec_op_code;
                               /**< Secondary operation code indicating
                                    the format for the rest of the payload. */
    uint32_t                   avtimer_timestamp_lsw;
                               /**< LSW AVTimer timestamp in microseconds. */

    uint32_t                   avtimer_timestamp_msw;
                               /**< MSW AVTimer timestamp in microseconds. */

    int32_t                    dtmf_duration_in_ms;
                               /**< Duration of the DTMF tone in milliseconds. 
                               Duration includes both ramp up and ramp down.

                                @values
                                - -1 -- Infinite tone
                                - 0 -- Disable or stop infinite tone
                                - > 0 -- Finite duration @tablebulletend */

    uint16_t                    dtmf_high_freq;
                                /**< DTMF high tone frequency.

                                @values 1209, 1336, 1477, 1633 Hz */

    uint16_t                    dtmf_low_freq;
                                /**< DTMF low tone frequency.

                                 @values 697, 770, 852, 941 Hz */

    uint16_t                    dtmf_gain;
                                /**< Gain applied to a DTMF tone.

                                 @values Any number (in Q13 format) */

    uint16_t                    mix_enable;
                                /**< flag to enable DTMF tone mixing with
                                other data on the same port. */


    uint16_t                    num_ports;
                                /**< Number of ports following this structure in memory.

                                 @values @ge 1 */

    uint16_t                    port_ids;
                                /**< Destination ports (array).

                                 For DTMF on multiple ports, port_ids is populated num_ports times, and
                                 they follow this element. */

    uint16_t                    reserved;
                                /**< This field must be set to 0. */
}elite_msg_custom_afe_tone_generate_cfg_t;
#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef _AFE_CUST_MSG_H_
