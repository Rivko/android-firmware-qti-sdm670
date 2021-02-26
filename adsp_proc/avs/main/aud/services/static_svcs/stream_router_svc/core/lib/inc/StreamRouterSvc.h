/*========================================================================*/
/**
@file StreamRouterSvc.h

@brief Stream Router service public header.

This file contains Stream Router service belonging to the Elite framework.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
in the PDF.

The elite_audio_mainpage.dox file contains all file/group descriptions
that are in the output PDF generated using Doxygen and Latex. To edit or
update any of the file/group text in the PDF, edit the
elite_audio_mainpage.dox file or contact Tech Pubs.

The above description for this file is part of the "audio_decoder" group
description in the Elite_Core_mainpage.dox file.
===========================================================================*/

/*========================================================================
Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/stream_router_svc/core/lib/inc/StreamRouterSvc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
5/19/11    RP      Created file.
========================================================================== */
#ifndef _STREAMROUTERSVC_H_
#define _STREAMROUTERSVC_H_

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "adsp_adm_api.h"
#include "AudioStreamMgr.h"
#include "AFEAvtDrift.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup stream_router
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */
#define NUM_STRTRS                  2
#define RX_STRTR_ID                 0
#define TX_STRTR_ID                 1

#define STRTR_MAX_PORTS             2
#define STRTR_INVALID_PORT_ID       0xFFFFFFFF
/**
@brief Data structure for initializing the Stream Router service on an open
command.
*/
typedef struct
{
   uint32_t      stream_router_id;   /**< stream router ID. (Tx or Rx). */

} streamRouterSvcInitParams_t;

/**
 * Creates an instance of the Elite Stream Router service.

@param [out] handle      Service entry handle returned to the caller.

@return
Success or failure of the instance creation.

@dependencies
None.
*/
ADSPResult stream_router_svc_create (streamRouterSvcInitParams_t *inputParam, void **handle);

/** Operation code for Get Session Time message to stream
*  router.
*/
#define ELITEMSG_CUSTOM_STREAM_ROUTER_GET_SESSION_TIME     (0x002A0001UL)

/** Payload format for the Get Session Time message.
*/
typedef struct EliteMsg_CustomStreamRouterGetSessionTime EliteMsg_CustomStreamRouterGetSessionTime;

/**
@brief Payload structure for the StreamRouter Get Session Time
message and Get Session Time V2 message.

This custom message is sent to the stream router to retrieve
information about an audio stream's session time (how much
content has been rendered to the device). This information can
be used by the client for Audio-Video Synchronization
(AV-Sync) or progress bar updates.
*/
struct EliteMsg_CustomStreamRouterGetSessionTime
{
   qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                           must be returned. */

   qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                           acknowledgment. NULL indicates
                                           that no response is required. */

   uint32_t         unClientToken;    /**< Token to be given in the
                                      acknowledgment. This is different
                                      from the unResponseResult and can
                                      be used to identify which service
                                      sent the response to the server. */

   uint32_t         unResponseResult; /**< Response result that the server
                                      sends back to the client. */

   uint32_t         unSecOpCode;      /**< Secondary operation code that
                                      indicates the format for the rest
                                      of the payload. */
   uint32_t         port_id;          /**< Secondary operation code that
                                         indicates the format for the rest
                                         of the payload. */

   uint64_t         ullSessionTime;   /**< Session time (in microseconds). */

   uint64_t         ullAbsoluteTime;  /**< Absolute time (in microseconds). */
};


/** Operation code for message to connect stream router port.
*/
#define ELITEMSG_CUSTOM_STREAM_ROUTER_CONNECT     (0x002A0002UL)

/** Operation code for message to disconnect stream router port.
*/
#define ELITEMSG_CUSTOM_STREAM_ROUTER_DISCONNECT     (0x002A0003UL)

#define STRTR_PORT_DIR_INPUT               0

#define STRTR_PORT_DIR_OUTPUT              1


/** Payload format for the Stream Router config Port message.
*/
typedef struct EliteMsg_CustomStreamRouterConnect EliteMsg_CustomStreamRouterConnect;

/**
@brief Payload structure for the StreamRouter connect/disconnect message.

This custom message is sent to the stream router to retrieve
information about an audio stream's session time (how much
content has been rendered to the device). This information can
be used by the client for Audio-Video Synchronization
(AV-Sync) or progress bar updates.
*/
struct EliteMsg_CustomStreamRouterConnect
{
   qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                           must be returned. */

   qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                           acknowledgment. NULL indicates
                                           that no response is required. */

   uint32_t         unClientToken;    /**< Token to be given in the
                                      acknowledgment. This is different
                                      from the unResponseResult and can
                                      be used to identify which service
                                      sent the response to the server. */

   uint32_t         unResponseResult; /**< Response result that the server
                                      sends back to the client. */

   uint32_t         unSecOpCode;      /**< Secondary operation code that
                                      indicates the format for the rest
                                      of the payload. */

   uint32_t         port_dir;         /**< Is the port to be connected to input/output port
                                      Use STRTR_PORT_DIR_INPUT for input and
                                      STRTR_PORT_DIR_OUTPUT for output. */
   elite_svc_handle_t *svc_handle_ptr; /**< For output port connection this field indicates
                                       the downstream module's svc handle to connect to.
                                       For input port connection this field will contain the
                                       stream router port's service handle in the response. */
   uint32_t         port_id;          /**< port id of the port to which connection is established
                                         For device connect - ADM sends the port id of the stream side port to connect to
                                         The port Id of the new port is returned to ADM for both stream and device connect*/
   uint32_t         format_id;        /**<  Format ID of the compressed stream. Required for connection
                                      of input port on Rx path */
   uint32_t         num_channels;     /**<  number of channels: 2 for Audio Stream packet, 8 for HBR */
   uint32_t         sampling_rate;    /**<  sampling rate of the stream */
   uint32_t         bits_per_sample;  /**<  bit width of the steram   */
   uint32_t         data_log_id;      /**<  unique data logging Id for this stream/device session */
   bool_t           is_stream_to_be_connected;   /**< Indicates if the stream side is to be connected to the given device port
                                                if yes, port_id above is connected to the device port. If not (DEVICE_OPEN command
                                                is used, then wait for the MAP_ROUTING command to establish this connection */

   uint32_t             buffering_ms;     /**< this buffering in ms decides the number of buffers &
                                    may be length of the buffers. Dont care for Rx st rtr.*/

   volatile uint32_t    *punAFEDelay;      /**<ADM passes the address of the AFE delay. AFE is supposed to fill this in */
   volatile uint32_t    *punCoppBufDelay;     /**<ADM passes the address of the compressed COPP buffering delay. Compressed COPP is supposed to fill this in */
   volatile uint32_t    *punCoppAlgDelay;     /**<ADM passes the address of the compressed COPP algorithmic delay. Compressed COPP is supposed to fill this in */
   volatile uint32_t    *punStrRtrDelay;   /**<ADM passes the address of the AFE delay. StrRtr is supposed to fill this in  */
   Elite_CbType          *pEliteCb;        /**<Pointer to the callback structure needed for stream router to raise event*/
      volatile const afe_drift_info_t  *pAfeDriftPtr;

};


/** Operation code for message to disconnect stream router port.
*/
#define ELITEMSG_CUSTOM_STREAM_ROUTER_DISABLE_PORT     (0x002A0004UL)

/** Payload format for the custom stream router disable port command
*/
typedef struct EliteMsg_CustomStreamRouterDisablePort EliteMsg_CustomStreamRouterDisablePort;

/**
@brief Payload structure for the StreamRouter disable port message.

This custom message is sent to the stream router to stop it sending down more
data on its output port

*/
struct EliteMsg_CustomStreamRouterDisablePort
{
   qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                         must be returned. */

   qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                         acknowledgment. NULL indicates
                                         that no response is required. */

   uint32_t         unClientToken;    /**< Token to be given in the
                                         acknowledgment. This is different
                                         from the unResponseResult and can
                                         be used to identify which service
                                         sent the response to the server. */

   uint32_t         unResponseResult; /**< Response result that the server
                                         sends back to the client. */

   uint32_t         unSecOpCode;      /**< Secondary operation code that
                                         indicates the format for the rest
                                         of the payload. */

   uint32_t         port_id;         /**< port id of the output port that
                                        needs to be diabled */
};


/** Operation code for the Stream Router Run message.
*/
#define ELITEMSG_CUSTOM_STREAM_ROUTER_RUN              (0x002A0005UL)

/** Payload format for the Stream Router Run message.
*/
typedef struct EliteMsg_CustomStreamRouterRun EliteMsg_CustomStreamRouterRun;

/**
@brief Payload structure for Stream Router Run message.

This custom message is used to kick start the rendering process of a
compressed audio stream connected to the stream router. The command can instruct the
stream router to begin the rendering process immediately, at a specified absolute
time or with a given amount of delay
*/
struct EliteMsg_CustomStreamRouterRun
{
   qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                           must be returned. */

   qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                           acknowledgment. NULL indicates
                                           that no response is required. */

   uint32_t        unClientToken;    /**< Token to be given in the
                                     acknowledgment. This is different
                                     from the unResponseResult and can
                                     be used to identify which service
                                     sent the response to the server. */

   uint32_t        unResponseResult; /**< Response result that the server
                                     sends back to the client. */

   uint32_t        unSecOpCode;      /**< Secondary operation code that
                                     indicates the format for the rest
                                     of the payload. */
   uint32_t        port_id;         /**< port id of the stream side port
                                       that needs to be set to run*/

   int64_t         llStartTime;     /**< Time (in microseconds) to which to
                                    align the session origin time
                                    (timestamp=0). */

   uint32_t        unStartFlag;      /**< Indicates whether to run immediately
                                     or at a specific rendering time. */
};

/** Operation code for message to set stream router specific parameter.
*/
#define ELITEMSG_CUSTOM_STREAM_ROUTER_SET_PARAM     (0x002A0006UL)


/** Operation code for message to get stream router specific parameter.
*/
#define ELITEMSG_CUSTOM_STREAM_ROUTER_GET_PARAM     (0x002A0007UL)

/**
@brief Payload structure for Stream Router Set / Get Param message.

This custom message is used to set a particular stream router configuration
parameter or to get the required parameter value
*/

struct EliteMsg_CustomStreamRouterGetSetParam
{
   qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                         must be returned. */

   qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                         acknowledgment. NULL indicates
                                         that no response is required. */

   uint32_t        unClientToken;    /**< Token to be given in the
                                         acknowledgment. This is different
                                         from the unResponseResult and can
                                         be used to identify which service
                                         sent the response to the server. */

   uint32_t        unResponseResult; /**< Response result that the server
                                         sends back to the client. */

   uint32_t        unSecOpCode;      /**< Secondary operation code that
                                         indicates the format for the rest
                                         of the payload. */

   uint32_t        unPortID;         /**< ID of the port where the get/set
                                         param is requested. */

   uint32_t        unPortDirection;  /**< Indicates whether the port is an
                                       input or output port. */

   uint32_t        unParamId;        /**< Elite parameter ID; supports only
                                        ELITEMSG_PARAM_ID_CAL. The payload
                                        is derived based on this ID and the
                                        message. */

   int32_t         *pnParamData;     /**< Points to *_param_data_t
                                        (e.g:asm_session_param_data_t) */

   uint16_t        unSize;          /**< Payload size in bytes for the memory
                                        pointed by pnParamData. For GetParams,  this will be the max size of the
                                        expected data to be returned.*/

};


/** Operation code for message to pause stream router port
*/
#define ELITEMSG_CUSTOM_STREAM_ROUTER_PAUSE     (0x002A0008UL)

/** Operation code for message to flush stream router port
*/
#define ELITEMSG_CUSTOM_STREAM_ROUTER_FLUSH     (0x002A0009UL)

/** Payload format for the Stream Router Pause/Flush message.
*/
typedef struct EliteMsg_CustomStreamRouterPauseFlush EliteMsg_CustomStreamRouterPauseFlush;

/**
@brief Payload structure for Stream Router Pause/Flush message.

Custom message to pause/flush the desired port of the stream router.
The pause is for the stream side port only

*/
struct EliteMsg_CustomStreamRouterPauseFlush
{
   qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                         must be returned. */

   qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                         acknowledgment. NULL indicates
                                         that no response is required. */

   uint32_t        unClientToken;    /**< Token to be given in the
                                         acknowledgment. This is different
                                         from the unResponseResult and can
                                         be used to identify which service
                                         sent the response to the server. */

   uint32_t        unResponseResult; /**< Response result that the server
                                         sends back to the client. */

   uint32_t        unSecOpCode;      /**< Secondary operation code that
                                         indicates the format for the rest
                                         of the payload. */

   uint32_t        port_id;          /**< Port id of the stream router that needs
                                        to be paused*/
};

/** Operation code for message to route stream to device side
*/
#define ELITEMSG_CUSTOM_STREAM_ROUTER_MAP_ROUTINGS     (0x002A000AUL)

/** Payload format for the Stream Router map routings message.
 */
typedef struct EliteMsg_CustomStreamRouterMapRoutings EliteMsg_CustomStreamRouterMapRoutings;

/**
@brief Payload structure for Stream Router Map Routings command.

Custom message to route the given stream port to the desired device port

 */
struct EliteMsg_CustomStreamRouterMapRoutings
{
   qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                         must be returned. */

   qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                         acknowledgment. NULL indicates
                                         that no response is required. */

   uint32_t        unClientToken;    /**< Token to be given in the
                                         acknowledgment. This is different
                                         from the unResponseResult and can
                                         be used to identify which service
                                         sent the response to the server. */

   uint32_t        unResponseResult; /**< Response result that the server
                                         sends back to the client. */

   uint32_t        unSecOpCode;      /**< Secondary operation code that
                                         indicates the format for the rest
                                         of the payload. */
   adm_cmd_stream_device_map_routings_v5_t *pRoutingPayload;
                                    /**< payload of the map routing command from the client. */

};

/** Operation code for message to unmap the routing between stream and device side ports
 */
#define ELITEMSG_CUSTOM_STREAM_ROUTER_UNMAP_ROUTINGS     (0x002A000BUL)

/** Payload format for the Stream Router map routings message.
 */
typedef struct EliteMsg_CustomStreamRouterUnMapRoutings EliteMsg_CustomStreamRouterUnMapRoutings;

/**
@brief Payload structure for Stream Router UnMap Routings command.

Custom message to route the given stream port to the desired device port

 */
struct EliteMsg_CustomStreamRouterUnMapRoutings
{
   qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                         must be returned. */

   qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                         acknowledgment. NULL indicates
                                         that no response is required. */

   uint32_t        unClientToken;    /**< Token to be given in the
                                         acknowledgment. This is different
                                         from the unResponseResult and can
                                         be used to identify which service
                                         sent the response to the server. */

   uint32_t        unResponseResult; /**< Response result that the server
                                         sends back to the client. */

   uint32_t        unSecOpCode;      /**< Secondary operation code that
                                         indicates the format for the rest
                                         of the payload. */
   adm_cmd_stream_device_unmap_routings_v5_t *pRoutingPayload;
                                    /**< payload of the unmap routing command from the client. */

};

#define ELITEMSG_CUSTOM_STREAM_ROUTER_REG_COMP_FMT_CHG     (0x002A000CUL)

        typedef struct EliteMsg_CustomCompressedFmtReg EliteMsg_CustomCompressedFmtReg;

        /**
     @brief Payload structure for the StreamRouter compressed media fmt change message.

         */
        struct EliteMsg_CustomCompressedFmtReg
        {
           qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                                must be returned. */

           qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                                acknowledgment. NULL indicates
                                                that no response is required. */

           uint32_t         unClientToken;    /**< Token to be given in the
                                           acknowledgment. This is different
                                           from the unResponseResult and can
                                           be used to identify which service
                                           sent the response to the server. */

           uint32_t         unResponseResult; /**< Response result that the server
                                           sends back to the client. */

           uint32_t         unSecOpCode;      /**< Secondary operation code that
                                           indicates the format for the rest
                                           of the payload. */

           uint16_t         port_id;         /**< Stream Router Port id*/

           uint16_t         token;           /**< Token used as the call_back*/

           uint16_t         enable; 

           uint16_t         dest_port;

           uint16_t         dest_addr;

           uint16_t         src_port;

           uint16_t         src_addr;
        };

   /** Operation code for setting primary output port of input ports. This is applicable
            only for RX Stream router, the opcode is the same to couple the similar command for matrix.
    */
#define ELITEMSG_CUSTOM_STREAM_ROUTER_SET_PRIMARY_PORT   (0x002A000DUL)

   /** payload for the primary output port set message.
    */
   typedef struct EliteMsg_CustomSetStrtrPrimaryPortType EliteMsg_CustomSetStrtrPrimaryPortType;

   /**
          @brief Payload structure for the primary output port set message.

          This custom message is used to indicate the primary output port of all
          active input ports. This custom message will be applicable only for RX matrix.
          Output ports that are connected to pullable PLLs will become primary ports.

          The payload for this event comprises one instance of
          EliteMsg_CustomSetPrimaryPortType, followed immediately by
          unNumSessions number of instances of EliteMsg_CustomPrimaryPortMappingInfoType
    */
   struct EliteMsg_CustomSetStrtrPrimaryPortType
   {
      qurt_elite_queue_t                 *pBufferReturnQ;
      /**< Queue to which this payload buffer
                                                    must be returned. */

      qurt_elite_queue_t                 *pResponseQ;
      /**< Queue to which to send the
                                                    acknowledgment. NULL indicates
                                                    that no response is required. */

      uint32_t                           unClientToken;
      /**< Token to be given in the
                                                    acknowledgment. This is different
                                                    from the unResponseResult and can
                                                    be used to identify which service
                                                    sent the response to the server. */

      uint32_t                           unResponseResult;
      /**< Response result that the server
                                                    sends back to the client. */

      uint32_t                           unSecOpCode;
      /**< Secondary operation code that
                                                    indicates the format for the rest
                                                    of the payload. */

      uint32_t                           unNumSessions;
      /**< Number of active sessions.
                                                    For each session mapping information of
                                                    session (input port) to output port
                                                    will be followed after this.*/
   };

   typedef struct EliteMsg_CustomComprPrimaryPortMappingInfoType EliteMsg_CustomComprPrimaryPortMappingInfoType;

   /**
          @brief Mapping information of session (input port) to primary output port
    */

   struct EliteMsg_CustomComprPrimaryPortMappingInfoType
   {
      uint16_t                           unInputPort;
      /**< Input port. */

      uint16_t                           unPrimaryOutputPort;
      /**< Primary output port of above
                                                    input port. */
   };


/** Operation code for message to flush stream router port
 */
#define ELITEMSG_CUSTOM_STREAM_ROUTER_SUSPEND     (0x002A000EUL)

/** Payload format for the Stream Router Pause/Flush message.
 */
typedef struct EliteMsg_CustomStreamRouterSuspend EliteMsg_CustomStreamRouterSuspend;

/**
@brief Payload structure for Stream Router Pause/Flush message.

Custom message to pause/flush the desired port of the stream router.
The pause is for the stream side port only

 */
struct EliteMsg_CustomStreamRouterSuspend
{
   qurt_elite_queue_t    *pBufferReturnQ;  /**< Queue to which this payload buffer
                                         must be returned. */

   qurt_elite_queue_t    *pResponseQ;      /**< Queue to which to send the
                                         acknowledgment. NULL indicates
                                         that no response is required. */

   uint32_t        unClientToken;    /**< Token to be given in the
                                         acknowledgment. This is different
                                         from the unResponseResult and can
                                         be used to identify which service
                                         sent the response to the server. */

   uint32_t        unResponseResult; /**< Response result that the server
                                         sends back to the client. */

   uint32_t        unSecOpCode;      /**< Secondary operation code that
                                         indicates the format for the rest
                                         of the payload. */

   uint32_t        port_id;          /**< Port id of the stream router that needs
                                        to be paused*/
};

   /** @} */ /* end_addtogroup stream_router */
#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef _STREAMROUTERSVC_H_

