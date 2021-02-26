#ifndef ELITEMSG_H
#define ELITEMSG_H

/**
@file EliteMsg.h

@brief This file defines messages structures, IDs and payloads
 (that do not depend on secondary opcode) for ELite messages.
*/
/** @weakgroup weakf_elite_msg_struct_descr
  The Elite message structure is used internally throughout the Elite
  framework. It is an 8-uint8_t structure that optimally passes data buffers
  through the system, which is the prime consideration for steady-state
  performance.

  Hexagon can perform an 8-uint8_t copy into and out of a queue in a single
  cycle. The 8 bytes are sufficient space for an opcode followed by a payload.
  This opcode is the primary opcode. Depending on the primary opcode, the
  payload typically is a pointer to a message payload buffer, but it can be an
  immediate payload value as well.

  The primary opcodes are globally defined. The recommended way to process an
  8-uint8_t Elite message is to use a function table with each entry being
  the message handler corresponding to the primary operation code. To reduce
  the function table size, the number of primary operation codes is tightly
  limited.

  Services can define more specific messages. These service-specific messages
  are multiplexed through the payload of #ELITE_CUSTOM_MSG, which
  reserves space for a secondary opcode to distinguish the content of
  messages. The secondary opcode is globally unique.
*/

/*========================================================================
   Copyright (c) 2010, 2013-2016 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/19/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
08/25/15   sw      (Tech Pubs) General edits in Doxygen comments.
09/18/14   sw      (Tech Pubs) Merged Doxygen markup/comments from 2.4,
                   and edited Doxygen markup/comments for 2.6.
03/20/13   sw      (Tech Pubs) Updated Doxygen markup/comments for 2.0.
10/28/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
08/31/10   DG      Changed the nFlags field in elite_msg_data_buffer_t to
                   unsigned, since it is a bitmask. This removes warnings
                   when it is ORed with 0x80000000.
02/04/10   DC      Created file.
==========================================================================*/


/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/* System */
#include "qurt_elite.h"

/* Audio */
#include "EliteSvc.h"


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/

/** @addtogroup elite_msg_primary_opcodes
  These primary opcodes are the only valid values allowed in the
  unOpCode field of the elite_msg_any_t structure. They are tightly controlled
  in a centralized location to avoid potential re-use of the same opcode.
@{ */

#define    ELITE_CUSTOM_MSG            0x00000000L
    /**< Indicates a custom message supported by a given service.

         This opcode reserves space for a secondary opcode in its payload.
         The payload points to a buffer that contains a message not defined in
         the Elite common messages.

         @msgpayload{elite_msg_custom_header_t}
         @table{weak__elite__msg__custom__header__t} */

#define    ELITE_CMD_START_SERVICE     0x00000001L
    /**< Requests a static service to be started. */

#define    ELITE_CMD_DESTROY_SERVICE   0x00000002L
    /**< Requests a service to destroy itself.

         The sender of this message must know the service master thread ID and
         join that thread to await full cleanup. */

#define    ELITE_CMD_CONNECT           0x00000003L
    /**< Requests a service to connect to a second service.

         The connection consists of sharing the handle of the second service
         with the first service. Thus, the first service can send data buffers
         and commands to the second service.

         Two services can share additional proprietary handshaking at any time.

         @msgpayload{elite_msg_cmd_connect_t}
         @table{weak__elite__msg__cmd__connect__t} */

#define    ELITE_CMD_DISCONNECT        0x00000004L
    /**< Requests a service to disconnect from another service.

         The first service releases its reference to the service handle and
         sends no more data or commands to the second service.
         The second service returns all data buffers to their buffer queues.

         @msgpayload
         Uses the same payload as #ELITE_CMD_CONNECT. */

#define    ELITE_CMD_PAUSE             0x00000005L
    /**< Requests a service to pause.

         When paused, the service stops reading its input data queue and stops
         delivering data to its peers. @newpage */

#define    ELITE_CMD_RUN               0x00000006L
    /**< Requests the module to resume data flow.

         A client sends this command to  start rendering samples to the codec.

         @msgpayload{elite_msg_cmd_run_t}
         @table{weak__elite__msg__cmd__run__t} */

#define    ELITE_CMD_FLUSH             0x00000007L
    /**< Requests a service to flush its buffers and queues:
         - Empty its input data queue
         - Empty any partially filled input or output data buffers
         - Return all buffers to their originating buffer queues @newpage */

#define    ELITE_CMD_SET_PARAM         0x00000008L
    /**< Requests a service to set a service-defined parameter.

         @msgpayload_hdr{elite_msg_param_header_t}
         @table{weak__elite__msg__param__header__t}

         @msgpayload{elite_msg_param_cal_t}
         @table{weak__elite__msg__param__cal__t} @newpage

         @msgpayload{elite_msg_param_any_t}
         @tablens{weak__elite__msg__param__any__t} @newpage */

#define    ELITE_CMD_GET_PARAM         0x00000009L
    /**< Requests a service to get a service-defined parameter.

         @msgpayload_hdr{elite_msg_param_header_t}
         @table{weak__elite__msg__param__header__t}

         @msgpayload{elite_msg_param_cal_t}
         @table{weak__elite__msg__param__cal__t} @newpage

         @msgpayload{elite_msg_param_any_t}
         @tablens{weak__elite__msg__param__any__t} @newpage */

#define    ELITE_DATA_BUFFER           0x0000000AL
    /**< Indicates that the payload is a pointer to a data buffer that is used
         to exchange a buffer between services.

         @msgpayload{elite_msg_data_buffer_t}
         @table{weak__elite__msg__data__buffer__t} @newpage */

#define    ELITE_DATA_MEDIA_TYPE       0x0000000BL
    /**< Indicates that the payload is a pointer to the media type for the
         subsequent data buffers.

         @msgpayload_hdr{elite_msg_data_media_type_header_t}
         @tablens{weak__elite__msg__data__media__type__header__t}

         @msgpayload{elite_msg_data_media_type_apr_t}
         @tablens{weak__elite__msg__data__media__type__apr__t} @newpage */

#define    ELITE_DATA_EOS              0x0000000CL
    /**< Indicates an EOS message.
         This message can be flushed.

         @msgpayload_hdr{elite_msg_data_eos_header_t}
         @table{weak__elite__msg__data__eos__header__t}

         @msgpayload{elite_msg_data_eos_apr_t}
         @tablens{weak__elite__msg__data__eos__apr__t} @newpage

         @par EOS information (elite_msg_data_eos_info_t)
         @tablens{weak__elite__msg__data__eos__info__t} */

#define    ELITE_DATA_RAW_BUFFER       0x0000000DL
    /**< Indicates that the payload has no metadata; it is a raw buffer
         message.

         This opcode is useful when the data receiver does not need the typical
         information that accompanies a normal data buffer, such as timestamp
         and offset

         The data receiver must know where to return the buffer once it
         is consumed. */

#define    ELITE_CMD_STOP_SERVICE      0x0000000EL
    /**< Requests a service to halt its operations and return to an
         Initialized state.

         This opcode is optional for audio. */

#define    ELITE_APR_PACKET            0x0000000FL
    /**< Holds a pointer to an APR packet in case the message payload points to
         an APR packet. */

#define    ELITE_MIPS_CHANGE_EVENT     0x00000010L
    /**< Indicates that the MIPS requirement is changed for a service.
         The actual MIPS value is indicated by using a shared variable.
         @newpage */

#define   ELITE_DATA_SET_PARAM         0x00000011L
    /**< Requests a service to send any Set parameters that are synchronized
         with the data.

         @msgpayload{elite_msg_data_set_param_t}
         @table{weak__elite__msg__data__set__param__t} */

/**< 0x12 was used for DML change event*/

#define    ELITE_BW_CHANGE_EVENT     0x00000013L
    /**< Indicates that the bandwidth requirement is changed for a service.
         The actual bandwidth value is indicated by using a shared variable.

         @msgpayload{elite_msg_evt_bw_change_t}
         @table{weak__elite__msg__evt__bw__change__t} @newpage */

#define    ELITE_DATA_MARK_BUFFER      0x00000014L
    /**< Indicates the end of the stream. This message can be flushed.

         @msgpayload{elite_msg_data_mark_buffer_t}
         @tablens{weak__elite__msg__data__mark__buffer__t} */

#define ELITE_DATA_BUFFER_V2           0x00000015L
    /**< Indicates that the payload is a pointer to a data buffer that is used
         to exchange data between services.

         @msgpayload{elite_msg_data_buffer_v2_t}
         @table{weak__elite__msg__data__buffer__v2__t} */

#define ELITE_CUSTOM_EVENT           0x00000016L
    /**< Indicates that the payload is a pointer to a custom event to the
         service.

         @msgpayload{elite_msg_custom_event_header_t}
         @table{weak__elite__msg__custom__event__header__t} */

#define    ELITE_CMD_REGISTER_APR_EVENT               0x00000017L
    /**< APR clients can register/deregister for events from modules.
     *   static svc communicates to dynamic svc using this command

         @msgpayload{elite_msg_register_apr_event_t}
         @table{weak__elite__msg__register__apr__event__t} */

/** @} */ /* end_addtogroup elite_msg_primary_opcodes */


/** @addtogroup elite_msg_constants_macros
@{ */
/** Macro that determines the actual required buffer size if nReqBufSize bytes
    are requested.
 */
#define GET_ELITEMSG_DATABUF_REQ_SIZE( nReqBufSize ) ( (nReqBufSize) + sizeof(elite_msg_data_buffer_t) - 4 )

/** Timestamp Valid bitmask for the nFlag parameter in the
    elite_msg_data_buffer_t structure.
*/
#define ELITE_DATA_BUFFER_TIME_STAMP_BITMASK       0x80000000L

/** Speed Factor bitmask for the nFlag parameter in the
    elite_msg_data_buffer_t structure.
*/
#define ELITE_DATA_BUFFER_SPEED_FACTOR_BITS        0x0000FFFFL

/** Macro that gets the timestamp valid bit from the nFlag parameter of the
    elite_msg_data_buffer_t structure.
 */
#define GET_DATA_BUFFER_TIME_STAMP_BIT( nFlag ) ( (nFlag) & (ELITE_DATA_BUFFER_TIME_STAMP_BITMASK) )

/** Macro that sets the timestamp valid bit in the nFlag parameter of the
    elite_msg_data_buffer_t structure.
 */
#define SET_DATA_BUFFER_TIME_STAMP_BIT( nFlag ) ( (nFlag) | (ELITE_DATA_BUFFER_TIME_STAMP_BITMASK) )

/** Macro that clears the timestamp valid bit in the nFlag parameter of the
    elite_msg_data_buffer_t structure.
 */
#define CLR_DATA_BUFFER_TIME_STAMP_BIT( nFlag ) ( (nFlag) & (~(ELITE_DATA_BUFFER_TIME_STAMP_BITMASK)) )

/** Macro that gets the actual buffer pointer from the audDataBuf parameter of
    the elite_msg_data_buffer_t structure.
 */
#define GET_ELITEMSG_DATABUF_BUF_ADDR( audDataBuf ) (  &( (audDataBuf).nDataBuf )   )

/** Macro that clears the speed factor bits in the nFlag parameter of the
    elite_msg_data_buffer_t structure.
*/
#define CLR_DATA_BUFFER_SPEED_FACTOR_BITS( nFlag ) ( (nFlag) & (~(ELITE_DATA_BUFFER_SPEED_FACTOR_BITS)) )

/** Macro that sets the speed factor bits in the nFlag parameter of the
    elite_msg_data_buffer_t structure.
*/
#define SET_DATA_BUFFER_SPEED_FACTOR_BITS( nFlag, speed_factor ) ( ((nFlag) | (speed_factor)) )

/** Macro that gets the speed factor bits from the nFlag parameter of the
    elite_msg_data_buffer_t structure.
*/
#define GET_DATA_BUFFER_SPEED_FACTOR_BITS( nFlag ) ( (nFlag) & (ELITE_DATA_BUFFER_SPEED_FACTOR_BITS) )

/** @} */ /* end_addtogroup elite_msg_constants_macros */


/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

/** @addtogroup elite_msg_datatypes
@{ */

/** Message structure for all Elite messages. The structure is of size 8
  uint8_t.
 */
typedef struct
{
   QURT_ELITE_ALIGN(uint32_t, 8) unOpCode;
   /**< Elite primary opcode (32-bit) to help distinguish the payload.

        @values See Section @xref{dox:PrimaryOpCodes}

        This field starts at the 8-byte alignment boundary to ensure that the
        message is also 8-byte aligned. */

   void    *pPayload;
   /**< Payload buffer (32-bit) pointer or payload value. The interpretation
        depends on the unOpCode value. */

} elite_msg_any_t;

/** Common header structure for the payload of most Elite command messages.
 */
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment.

         This token is different from the unResponseResult and is used to
         identify the service that sent the response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

} elite_msg_any_payload_t;

/** Forward declaration for the payload of #ELITE_CMD_START_SERVICE, which is
    used to request the start of static services.
 */
typedef elite_msg_any_payload_t    elite_msg_cmd_start_svc_t;

/** Forward declaration for the payload of #ELITE_CMD_DESTROY_SERVICE, which
    requests a service to destroy itself. The sender of this command message
    must know the service master thread ID and join that thread to await full
    cleanup.
 */
typedef elite_msg_any_payload_t    elite_msg_cmd_destroy_svc_t;

/** Forward declaration for the payload of #ELITE_CMD_PAUSE. When paused, a
    service stops reading its data queue and stops delivering data to its
    peers.
 */
typedef elite_msg_any_payload_t    elite_msg_cmd_pause_t;

/** Forward declaration for the payload of #ELITE_CMD_FLUSH, which commands a
    service to do the following:
    - Empty its input data queue
    - Empty any partially filled input or output data buffers
    - Return all buffers to their originating buffer queues
 */
typedef elite_msg_any_payload_t    elite_msg_cmd_flush_t;

/** Forward declaration for the payload of #ELITE_CMD_STOP_SERVICE. When
    stopped, a service resets any parameters and starts delivering data to its
    peers.
 */
typedef elite_msg_any_payload_t    elite_msg_cmd_stop_t;

/** @} */ /* end_addtogroup elite_msg_datatypes */

/** @weakgroup weak_elite_msg_cmd_connect_t
@{ */
/* Payload of the following messages:
  - The #ELITE_CMD_CONNECT message, which commands one service to connect to
  another service.
  - The #ELITE_CMD_DISCONNECT message, which commands one service to disconnect
    from another service.
 */
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment.

         This token is different from the unResponseResult and is used to
         identify the service that sent the response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

    elite_svc_handle_t *pSvcHandle;
    /**< Handle to the peer to which to connect or from which to disconnect. */

} elite_msg_cmd_connect_t ;
/** @} */ /* end_weakgroup weak_elite_msg_cmd_connect_t */

/** @weakgroup weak_elite_msg_param_header_t
@{ */
/* Common fields in an ELITE_CMD_SET_PARAM or ELITE_CMD_GET_PARAM
  payload.
*/
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment.

         This token is different from the unResponseResult and is used to
         identify the service that sent the response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

    uint32_t        unParamId;
    /**< Parameter ID. The rest of the payload is derived based on this ID.

         The parameter ID is 32 bits and is globally unique. A service can
         define a globally unique parameter ID as follows:

         - The most significant 16 bits are the most significant 16 bits of the
           service ID that defines this parameter ID.
         - The service defines the least significant 16 bits.

         @note1hang If the most significant 16 bits of the parameter ID are all
                    zeros, this message is reserved for a common parameter ID. */

} elite_msg_param_header_t;
/** @} */ /* end_weakgroup weak_elite_msg_param_header_t */

/** @weakgroup weak_elite_msg_data_buffer_t
@{ */
/* Payload of the ELITE_DATA_BUFFER message, which is used to exchange a
  buffer between services.
 */
typedef struct
{
   /* Must start with 8 uint8_t aligned boundary */
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment.

         This token is different from the unResponseResult and is used to
         identify the service that sent the response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

    /*Must be 8 uint8_t aligned here. */
    uint64_t        ullTimeStamp;
    /**< Timestamp of the buffer in microseconds. */

    uint32_t        nFlag;
    /**< Flag associated with the buffer.

         Bit 31 specifies whether the timestamp is valid:
         - 1 -- Valid timestamp
         - 0 -- Invalid timestamp
         - The bitmask is #ELITE_DATA_BUFFER_TIME_STAMP_BITMASK

         Bits 0 to 15 indicate the speed factor:
         - Speed factor -- Q12 value
         - Range of the speed factor -- 1024 to 16384
         - Default value for normal speed -- 4096
         - The bitmask is #ELITE_DATA_BUFFER_SPEED_FACTOR_BITS

         All other bits are reserved. */

    int32_t         nOffset;
    /**< Offset into the data buffer where the valid data begins. */

    int32_t         nActualSize;
    /**< Number of valid bytes in the buffer. */

    int32_t         nMaxSize;
    /**< Total number of bytes allocated in the data region. */

    /* 8 uint8_t aligned here. Assuming malloc always return 8 uint8_t aligned boundary.  */
    /* Data is in band. */
    int32_t         nDataBuf;
    /**< Beginning of the data buffer. The caller uses the
         GET_AUDDATABUF_REQ_SIZE macro to allocate enough space. */
                                       /* compiler might insert memory holes here!!!!!! */
} elite_msg_data_buffer_t ;
/** @} */ /* end_weakgroup weak_elite_msg_data_buffer_t */


/** @weakgroup weak_elite_msg_data_buffer_v2_t
@{ */
/* Payload of the ELITE_DATA_BUFFER_V2 message, which is used
  to exchange a buffer between services.
 */
typedef struct
{
   /* Must start with 8 uint8_t aligned boundary */
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t             unClientToken;
    /**< Token to be given in the acknowledgment.

         This token is different from the unResponseResult and is used to
         identify the service that sent the response to the server. */

    uint32_t             unResponseResult;
    /**< Response result that the server sends back to the client. */

    /*Must be 8 uint8_t aligned here. */
    uint64_t             ullTimeStamp;
    /**< Timestamp (in microseconds) of the buffer. */

    uint32_t             nFlag;
    /**< Flag associated with the buffer.

         Bit 31 specifies whether the timestamp is valid:
         - 1 -- Valid timestamp
         - 0 -- Invalid timestamp
         - The bitmask is #ELITE_DATA_BUFFER_TIME_STAMP_BITMASK

         Bits 0 to 15 indicate the speed factor:
         - Speed factor -- q12 value
         - Range of the speed factor -- 1024 to 16384
         - Default value for normal speed -- 4096
         - The bitmask is #ELITE_DATA_BUFFER_SPEED_FACTOR_BITS

         All other bits are reserved. */

    int32_t              nOffset;
    /**< Offset into the data buffer where the valid data begins. */

    int32_t              nActualSize;
    /**< Number of valid bytes in the buffer. */

    int32_t              nMaxSize;
    /**< Total number of bytes allocated in the data region. */

    void*                data_ptr;
    /**< Pointer to the data. @newpagetable */

} elite_msg_data_buffer_v2_t ;
/** @} */ /* end_weakgroup weak_elite_msg_data_buffer_v2_t */


/** @weakgroup weak_elite_msg_custom_header_t
@{ */
/* Common fields in any ELITE_CUSTOM_MSG message payload.
*/
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment.

         This token is different from the unResponseResult and is used to
         identify the service that sent the response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

    uint32_t        unSecOpCode;
    /**< Secondary opcode that indicates the format for the rest of
         the payload.

         The secondary opcode for #ELITE_CUSTOM_MSG is 32 bits and
         is globally unique. A service can define a globally unique secondary
         operation code as follows:

         - The most significant 16 bits are the most significant 16 bits of
           the service ID that defines this secondary opcode.
         - The service defines the least significant 16 bits.

         @note1hang If the most significant 16 bits of the secondary operation
                    code are all zeros, this message is reserved for a common
                    custom message. */

} elite_msg_custom_header_t;
/** @} */ /* end_weakgroup weak_elite_msg_custom_header_t */

/** @weakgroup weak_elite_msg_data_media_type_header_t
@{ */
/* Common fields in the ELITE_DATA_MEDIA_TYPE message payload. */
/** Indicates the media type for all subsequent data buffers.

  When FLUSH completes, each service precedes any subsequent data
  buffers with ELITE_DATA_MEDIA_TYPE in case there is confusion about a media
  type after FLUSH.
 */
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment.

         This token is different from the unResponseResult and is used to
         identify the service that sent the response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

    uint32_t        unMediaTypeFormat;
    /**< Format of the rest of the payload. */

} elite_msg_data_media_type_header_t;
/** @} */ /* end_weakgroup weak_elite_msg_data_media_type_header_t */

/** @weakgroup weak_elite_msg_data_eos_header_t
@{ */
/* Common fields in the ELITE_DATA_EOS message payload.
 */
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment.

         This token is different from the unResponseResult and is used to
         identify the service that sent the response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

    uint32_t        unEosFormat;
    /**< Format of the rest of the payload. */

} elite_msg_data_eos_header_t ;
/** @} */ /* end_weakgroup weak_elite_msg_data_eos_header_t */

/** @weakgroup weak_elite_msg_cmd_run_t
@{ */
/* Payload of the ELITE_CMD_RUN message. A client sends this command to 
  start rendering samples to the codec.
*/
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t        unClientToken;
    /**< Token to be provided in the acknowledgment.

         This token is different from the unResponseResult and is used to
         identify the service that sent the response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

} elite_msg_cmd_run_t ;
/** @} */ /* end_weakgroup weak_elite_msg_cmd_run_t */


/** @ingroup elite_msg_datatypes
  Forward declaration for the payload of the #ELITE_MIPS_CHANGE_EVENT message,
  which is used to indicate a change in the MIPS requirement of the service.
*/
typedef elite_msg_any_payload_t    elite_msg_evt_mips_change_t;

/** @weakgroup weak_elite_msg_evt_bw_change_t
@{ */
/* Payload of the ELITE_BW_CHANGE_EVENT message, which is used to
  indicate a change in the BW requirement of the service.
 */
typedef struct /** @cond */ _elite_msg_evt_bw_change_t /** @endcond */
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that
         no response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment.

         This token is different from the unResponseResult. It is used to
         identify the service that sent the response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

} elite_msg_evt_bw_change_t;
/** @} */ /* end_weakgroup weak_elite_msg_evt_bw_change_t */

/** @weakgroup weak_elite_msg_custom_event_header_t
@{ */
/* Common fields in any ELITE_CUSTOM_EVENT message payload.
*/
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t        unClientToken;
    /**< Token to be provided in the acknowledgment.

         This token is different from the unResponseResult. It is used to
         identify the service that sent the response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

    uint32_t        unSecOpCode;
    /**< Secondary opcode that indicates the format for the rest of
         the payload.

         The secondary opcode for #ELITE_CUSTOM_EVENT is 32 bits and
         is globally unique. A service can define a globally unique secondary
         operation code as follows:
         - The most significant 16 bits are the most significant 16 bits of
           the service ID that defines this secondary opcode.
         - The service defines the least significant 16 bits.

         @note1hang If the most significant 16 bits of the secondary operation
                    code are all zeros, this message is reserved for a common
                    custom message. */

} elite_msg_custom_event_header_t;
/** @} */ /* end_weakgroup weak_elite_msg_custom_event_header_t */

/** @weakgroup weak_elite_msg_register_apr_event_t
 *
@{ */
/* ELITE_CMD_REGISTER_APR_EVENT message payload.
*/
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t        unClientToken;
    /**< Token to be provided in the acknowledgment.

         This token is different from the unResponseResult. It is used to
         identify the service that sent the response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

    uint16_t         src_port;
    /**< src port of the apr packet */

    uint16_t         src_addr;
    /**< src addr of the apr packet */

    uint16_t         dest_port;
    /**< dest port of the apr packet */

    uint16_t         dest_addr;
    /**< dest addr of the apr packet */

    uint32_t         apr_token;
    /**< APR token of the command */

     int32_t          *pnParamData;
    /**< Points to module info structure. different svc may have different structs */

     uint32_t          unSize;
     /**< Payload size in bytes for the memory pointed by pnParamData and the list of modules */

} elite_msg_register_apr_event_t;
/** @} */ /* end_weakgroup weak_elite_msg_register_apr_event_t */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITEMSG_H

