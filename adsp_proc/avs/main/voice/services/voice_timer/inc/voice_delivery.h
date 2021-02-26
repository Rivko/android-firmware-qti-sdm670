/*========================================================================

*//** @file voice_delivery.h

Copyright (c) 2012-2014, 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Header file for voice delivery service
*//*====================================================================== */

/*========================================================================
  Edit History

  $Header:

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  08/23/2012 ka      Created file
  ========================================================================== */


#ifndef VOICEDELIVERY_H
#define VOICEDELIVERY_H

#include "Elite.h"

/* Macro to create VDS token using client type and session number */
#define VDS_CREATE_TOKEN(session, type) (((session) << 16) | (type))

/** @brief Enumerations for VDS buffer id's.
*/
enum VDS_BUFFER_TYPE
{
   VDS_UNTIMED_BUFFER,
   /**< Buffer ID to signify that data is ready for delivery and can be pushed downstream */

   VDS_TIMED_BUFFER,
   /**< Buffer ID to signify that data is ready and should be delivered at offset tick */

   VDS_INACTIVE_BUFFER,
   /**< Buffer ID to signify that this data should not be delivered */

   VDS_BUFFER_TYPES_END
   /**< Enum to indicate end of buffer ids */
};

/** @brief Enumerations for VDS msg id's.
*/
enum VDS_MSGS_TYPE
{
   VDS_SUBSCRIBE,
   /**< Msg ID to subscribe to Voice Delivery Service */

   VDS_UNSUBSCRIBE,
   /**< Msg ID to unsubscribe from Voice Delivery Service */

   VDS_DELIVER_ELITE_BUFFER,
   /**< Msg ID to schedule delivery of an elite buffer */

   VDS_DELIVER_APR_PACKET,
   /**< Msg ID to schedule delivery of an APR packet */

   VDS_RESYNC,
   /**< Msg ID to command resync for the client. In this context
        resync means that VDS drops either a request it is currently
	holding or the next incoming request */

   VDS_DROP_NOW,
   /**< Msg ID to inform VDS to drop existing buffer.  */


   VDS_MSGS_END
   /**< Enum to indicate end of msgs */
};

enum VDS_CLIENTS_TYPE
{
   VDS_CLIENT_VPTX_DELIVERY,
   VDS_CLIENT_VPRX_DELIVERY,
   VDS_CLIENT_VENC_DELIVERY,
   VDS_CLIENT_VDEC_PKT_REQUEST
};

/** @brief Structure of secondary payload used in Subscribe and Unsubscribe msgs.
        This payload acts as a communication channel between VDS and
        the client.
*/
typedef struct vds_sub_t
{
   uint32_t                 client_token;
   /**< ID to identify the client type, i.e which
         service/type of delivery etc. */
   uint32_t                 client_id;
   /**< Client ID returned by VDS when subscribing
        Client must provide this value when unsubscribing
	and sending delivery requests */
   qurt_elite_signal_t      *signal_ptr;
   /**< Signal to assert when delivery is to be made.
        VDS creates this signal and returns it
         */
   qurt_elite_signal_t      *error_signal_ptr;
   /**< Signal that VDS will assert when there is a
        delivery error. If it is set to NULL, VDS will
	not send the signal */

   qurt_elite_signal_t      *resync_response_signal_ptr;
   /**< Signal that VDS will assert when there is a
        resync response to the client. If it is set to NULL, VDS will
	not send the signal */

   bool_t                   is_delivery_tick_reqd;
   /**< Tells VDS whether it needs to wait for client delivery tick
        or deliver immidiately after queuing */
} vds_sub_t;

typedef struct vds_deliver_elite_msg_t
{
   uint32_t                 client_token;
   /**< ID to identify the client type, i.e which
         service/type of delivery etc. */
   uint32_t                client_id;
   /**< Unique client ID sent during subscribe */
   qurt_elite_queue_t      *peer_ptr;
   /**< Queue to pus buffer to */
   elite_msg_any_t         msg;
   /**< Elite message */
   uint32_t                buffer_id;
   /**< Buffer ID */
} vds_deliver_elite_msg_t;

typedef struct vds_deliver_apr_packet_t
{
   uint32_t                 client_token;
   /**< ID to identify the client type, i.e which
         service/type of delivery etc. */
   uint32_t                client_id;
   /**< Unique client ID sent during subscribe */
   elite_apr_handle_t*     apr_handle_ptr;
   /**< Queue to push buffer to */
   elite_apr_packet_t*     apr_packet_ptr;
   /**< Pointer to elite message */
   uint64_t                delivery_timestamp;
   /**< Expected delivery time stamp for this apr packet */
} vds_deliver_apr_packet_t;

typedef struct vds_resync_t
{
   uint32_t                 client_token;
   /**< ID to identify the client type, i.e which
         service/type of delivery etc. */
   uint32_t                client_id;
   /**< Unique client ID sent during subscribe */
} vds_resync_t;

/** @brief Structure of primary payload used in Subscribe and Unsubscribe custom msgs.
*/
typedef struct
{
    qurt_elite_queue_t      *pBufferReturnQ;
    /**< This is the queue to which this payload buffer needs to be returned */

    qurt_elite_queue_t      *pResponseQ;
    /**< This is the queue to send the ACK back. NULL indicates no response is required */

    uint32_t                unClientToken;
    /**< Token that should be given in the ACK. This is different than the
         unResponeResult and can be used to identify who send the ACK back by
         the server. */

    uint32_t                unResponseResult;
    /**< This is to be filled with ACK result by the client. */

    uint32_t                unSecOpCode;
    /**< This is the secondary opcode indicating the format for the rest of payload */

    vds_sub_t               *sub_req_ptr;
    /**< Pointer to subscribe or unsubscribe struct. */

} elite_msg_custom_vds_sub_type;

/** @brief Structure of primary payload used with VDS_DELIVER_ELITE_BUFFER
*/
typedef struct
{
    qurt_elite_queue_t      *pBufferReturnQ;
    /**< This is the queue to which this payload buffer needs to be returned */

    qurt_elite_queue_t      *pResponseQ;
    /**< This is the queue to send the ACK back. NULL indicates no response is required */

    uint32_t                unClientToken;
    /**< Token that should be given in the ACK. This is different than the
         unResponeResult and can be used to identify who send the ACK back by
         the server. */

    uint32_t                unResponseResult;
    /**< This is to be filled with ACK result by the client. */

    uint32_t                unSecOpCode;
    /**< This is the secondary opcode indicating the format for the rest of payload */
    vds_deliver_elite_msg_t delivery_request;
    /**< Pointer to subscribe or unsubscribe struct. */

} elite_msg_custom_vds_deliver_elite_type;

/** @brief Structure of primary payload used with VDS_DELIVER_APR_PACKET
*/
typedef struct
{
    qurt_elite_queue_t      *pBufferReturnQ;
    /**< This is the queue to which this payload buffer needs to be returned */

    qurt_elite_queue_t      *pResponseQ;
    /**< This is the queue to send the ACK back. NULL indicates no response is required */

    uint32_t                unClientToken;
    /**< Token that should be given in the ACK. This is different than the
         unResponeResult and can be used to identify who send the ACK back by
         the server. */

    uint32_t                unResponseResult;
    /**< This is to be filled with ACK result by the client. */

    uint32_t                unSecOpCode;
    /**< This is the secondary opcode indicating the format for the rest of payload */
    vds_deliver_apr_packet_t delivery_request;
    /**< Pointer to subscribe or unsubscribe struct. */

} elite_msg_custom_vds_deliver_apr_type;

/** @brief Structure of primary payload used with VDS_RESYNC
*/
typedef struct
{
    qurt_elite_queue_t      *pBufferReturnQ;
    /**< This is the queue to which this payload buffer needs to be returned */

    qurt_elite_queue_t      *pResponseQ;
    /**< This is the queue to send the ACK back. NULL indicates no response is required */

    uint32_t                unClientToken;
    /**< Token that should be given in the ACK. This is different than the
         unResponeResult and can be used to identify who send the ACK back by
         the server. */

    uint32_t                unResponseResult;
    /**< This is to be filled with ACK result by the client. */

    uint32_t                unSecOpCode;
    /**< This is the secondary opcode indicating the format for the rest of payload */

    vds_resync_t            resync_params;
    /**< Pointer to subscribe or unsubscribe struct. */

} elite_msg_custom_vds_resync_type;


ADSPResult vds_create(elite_svc_handle_t** vds_handle, uint32_t instance_id);

void vds_update_histogram( uint32_t client_token, uint32_t time);



#endif /* VOICETIMERMSGS_H */
