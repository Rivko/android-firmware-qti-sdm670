/**
@file AudioStreamMgr_Type.h
@brief This file declares some common data types used by AudioStreamMgr.

*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_Type.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
04/01/2010 DC      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef AUDIO_STREAM_MGR_TYPE_H
#define AUDIO_STREAM_MGR_TYPE_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/// System
#include "qurt_elite.h"
#include <stringl/stringl.h>
#include "adsp_private_api_ext.h"
#include "adsp_private_api.h"

/// Audio
#include "Elite.h"

#include "adsp_asm_api.h"
#include "adsp_error_codes.h"
#include "AudDynaPPSvc.h"
#include "AudioStreamMgr.h"
#include "AudioStreamMgr_PrivateDefs.h"
#include "qurt_elite_adsppm_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/
/*Max number of channel bits.*/
#define AUDIO_STREAM_MGR_MAX_NUM_CHANNEL_BITS      32

/* Number of maximum dynamic service per stream.
   Gapless: Dec->Switch->POPP->RxMtx (4)
   Tunneled Playback: Dec->POPP->RxMtx (3)
   Tunneled Record: TxMtxNode->POPP->Enc (3)
   Non-tunneled: Dec->POPP->Enc (3)
   Loopback: TxMtx->POPP->RxMtx (3)
   compr->compr Loopback: TxRtr->Dec->PP->Enc->RxRtr*/
#define  AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM     6

#define ASM_MAX_CLIENT_TO_DEVICE_STREAMS     1

/*  Max number of sessions supported. */
#define  ASM_MAX_STREAMS_GAPLESS_SESSION        2

/** maximum number of streams per session
 * We need one more than max gapless streams to handle below scenario:
 * stream-id 1 -> first gapless
 * stream-id 2 -> second stream
 * stream-id 1 closed
 * stream-id 3 opened.
 * to hold stream-id 3 we cannot reuse 1's slot as 1 is the firstgaplessStream who's nodes are shared with
 * matrix/pp etc. We need a 3rd to allot memory for stream-id 3.*/
#define ASM_MAX_SUPPORTED_STREAMS_PER_SESSION   (ASM_MAX_STREAMS_GAPLESS_SESSION+1)

/* Enumeration of stream modes */
enum asm_session_mode_t
{
   ASM_RX_MODE = 0,    /*Tunneled playback mode*/
   ASM_TX_MODE,        /*Tunneled record mode*/
   ASM_NT_MODE,        /*Non-tunneled mode*/
   ASM_LOOPBACK_PCM_IN_PCM_OUT_MODE,          /*Loopback mode PCM -> PCM */
   ASM_LOOPBACK_PCM_IN_COMPRESSED_OUT_MODE,   /*Loopback mode PCM -> compressed*/
   ASM_LOOPBACK_COMPRESSED_IN_PCM_OUT_MODE,          /*Loopback mode compressed -> PCM */
   ASM_LOOPBACK_COMPRESSED_IN_COMPRESSED_OUT_MODE,   /*Loopback mode compressed -> compressed*/
   ASM_LOOPBACK_COMPRESSED_IN_MULTI_OUT_MODE,          /*Loopback mode compressed -> multiple outputs */
   ASM_COMPRESSED_PLAYBACK_MODE, /*Compressed playback mode*/
   ASM_COMPRESSED_INPUT_MODE, /*Compressed input mode*/
   ASM_RX_PULL_MODE, /* pull mode rx*/
   ASM_TX_PUSH_MODE, /* push mode tx */
   ASM_UNKNOWN_MODE  /** mode is unknown */
};

/* This is the state of each session */
enum asm_stream_state_t
{
   /* Stable State*/
   ASM_STREAM_CLOSE = 0,    /**< In close state, the STREAM is no longer active
                                             and is ready to be OPEN again. */
   ASM_STREAM_RUN ,      /**< Svcs in the STREAM is running and processing data.

                           Run, pause, suspend are session states. Not removing just in case we go away from this approach &
                           have run/pause/suspend for streams.*/

   ASM_STREAM_PAUSE ,    /**< Data in the STREAM is gated at some svc level and
                                             not to be rendered. */
   ASM_STREAM_SUSPEND ,        /**< STREAM is in pause state and hardware modules are
                                                in power off mode, and deregistered by adsppm */
   /*Transitional States */
   ASM_STREAM_DISCONNECTING, /**< This transitional state indicates STREAM is
                                                in the process of tear down the graph. This
                                                step is required before destroy svcs in the graph.
                                                */
   ASM_STREAM_CONNECTING,    /**< This transitional state indicates STREAM is
                                                in the process of connecting the graph.
                                                */
   ASM_STREAM_BAD_STATE,      /**< Transitional state which is set if more
                                               responses are expected before calling panic handler */
   ASM_STREAM_GAPLESS_CLOSE /**< in gapless playback first a stream is opened and then its nodes are shared with PP, MtMx svcs (kips, BW).
                                  Later if a second stream is opened and first stream is closed, then first stream is put to this state.
                                  It's different from close in that (a) its nodes are used in close state (b) if reopened, its nodes don't need reset.
                                  only first stream can go to this state. once last gapless stream is closed CLOSE state is regained.*/
};

/* This is the state of each session */
enum asm_session_state_t
{
   /* Stable State*/
   AUDIO_STREAM_MGR_SESSION_CLOSE = 0,    /**< In close state, the session is no longer active
                                             and is ready to be OPEN again. */
   AUDIO_STREAM_MGR_SESSION_RUN ,      /**< Svcs in the session is running and processing data*/
   AUDIO_STREAM_MGR_SESSION_PAUSE ,    /**< Data in the session is gated at some svc level and
                                             not to be rendered. */
   AUDIO_STREAM_MGR_SESSION_SUSPEND ,        /**< Session is in pause state and hardware modules are
                                                in power off mode, and deregistered by adsppm */
   AUDIO_STREAM_MGR_SESSION_BAD_STATE      /**< Transitional state which is set if more
                                               responses are expected before calling panic handler */
};

#define AUDIO_STREAM_MGR_MAX_SYS_CMD_Q_ELEMENTS  32
#define AUDIO_STREAM_MGR_MAX_SYS_EVT_Q_ELEMENTS  16

/*For non gapless streams there is only one stream
which is indexed as 0 by default*/
#define ASM_DEFAULT_STREAM           0

//Ttwo steps in matrix disconnection
#define  DISCONNECT_MATRIX    true
#define  DISABLE_MATRIX       false

/** TX matrix output buffers help in delaying the data. This is necessary for AV sync in loopback.
 * Currently, max delay is 150 ms. Other buffers are standard 10 ms (Two 5 ms buffers).
 * Note: the numbers should be multiple of 5. The Rx matrix value is not honored since buffers are not created by matrix. */
#define ASM_TX_MATRIX_OUTPUT_BUFFERING_LOOPBACK   (150)
#define ASM_TX_MATRIX_OUTPUT_BUFFERING_LOWLATENCY_LOOPBACK   (50)
#define ASM_TX_MATRIX_OUTPUT_BUFFERING_REGULAR    (10)
#define ASM_RX_MATRIX_INPUT_BUFFERING_REGULAR    (10)
#define ASM_TX_MATRIX_OUTPUT_BUFFERING_LOWLATENCY  (2)
#define ASM_RX_MATRIX_INPUT_BUFFERING_LOWLATENCY  (2)

#define ASM_INVALID_INDEX (0xFF)

// Check for stream id, needed for KW
#define ASM_CHECK_STREAM_ID(stream_id) 			                                 \
       if ( stream_id > ASM_MAX_SUPPORTED_STREAM_ID)                             \
	{                                                                             \
	    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"incorrect stream id %lu",           \
	          (uint32_t) stream_id);                                              \
		 return ADSP_EHANDLE; 	                                                   \
	}

#define ASM_CHECK_SESSION_NULLNESS(pSession, pPkt)                                     \
      if (NULL == pSession)                                                      \
      {                                                                          \
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Session instance cannot be null");  \
         AudioStreamMgr_GenerateAck( pPkt, ADSP_EFAILED, NULL, 0, 0 );               \
         return ADSP_EFAILED;                                                    \
      }


/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

typedef struct ASM_RunPauseFlushFTableEntry ASM_RunPauseFlushFTableEntry;
typedef struct ASM_CreateDynaSvcFTableEntry ASM_CreateDynaSvcFTableEntry;

/** order is important because the Ftables like ASM_Run/Pause/SuspendFTable etc have this order.
 * *** Remember to modify those tables when any change is made here*** */
enum ASM_GraphNodeIndex
{
   ASM_NODE_DEC=0,
   ASM_NODE_PP,
   ASM_NODE_RX_MTX,
   ASM_NODE_TX_MTX,
   ASM_NODE_SWITCH,
   ASM_NODE_ENC,
   ASM_NODE_RX_ST_RTR,
   ASM_NODE_TX_ST_RTR,
   ASM_NODE_RX_GENERIC,  // A Generic downstream node that is either AFE for ULL/LLNP Rx or COPP for ULLPP modes.
                         // In LLNP/ULL Rx, dummy handle is returned by ADM for stream connect and stored in stream->afe_dummy_handle. This node stores actual handle.
                         // Whenever AFE node has to be used, be very careful because the handle to be used may be  afe_dummy_handle or the handle in node struct. this AFE is always Rx.
   ASM_NODE_TX_GENERIC,  // A generic upstream node used by LLNP Tx use cases.
   ASM_NODE_MAX
};

#define ASM_INVALID_NODE ASM_NODE_MAX

#define ASM_INVALID_NODE_ID   0

/* This structure represent a node (dynamic service) in the graph under ASM control. */
struct ASM_GraphNode {
   ASM_GraphNodeIndex   index;      /** index of this node (whether DEC, PP etc). used only for debugging. assigned at creation of ASM.*/
   elite_svc_handle_t   *pSvcHandle; /**< service handle also have service ID. In case of AFE & ULL, this is the real AFE handle. the dummy handle returned by ADM for streamconnect is stored in afe_handle. */
   qurt_elite_thread_t  threadId; /**< Need to save service thread ID so that ASM can
                                       thread join the destroyed dynamic service thread.
                                       */
   volatile uint32_t    buf_delay;  /** The delay introduced by the module. This includes
                                    the buffering delay and the algoritmic delay.
                                    The dynamic service is expected to initialize this
                                    during service creation time. It is also required
                                    to update this variable whenever the delay changes. unit microseconds. */
   volatile uint32_t    alg_delay;  /** The delay introduced by the module. This includes
                                    the buffering delay and the algoritmic delay.
                                    The dynamic service is expected to initialize this
                                    during service creation time. It is also required
                                    to update this variable whenever the delay changes. unit microseconds. */
} ;

/*
This is the IF that matrix will provide to ASM for any future communication after
connection.
Stream connect and disconnect is sent to ADM. On ACK of Stream connect,
ADM provide a matrix handle.  ASM communicate with matrix using that handle
directly except with stream disconnect.

This param is also applicable to stream router.
*/
struct AudioStreamMgr_MtMxPortParam {
    uint32_t unPortID;
    uint32_t unPortDirection;
}  ;

/*
* This structure stores the callback and context required for sending messages directly to PP.
*/
struct AudioStreamMgr_PPCallbackType {
   AudPP_MsgCbType function;
   void *context;
} ;

/* the order in this struct cannot be changed as ASM_TOPO_TABLE relies on it. */
struct ASM_StrmSvcList
{
   ASM_GraphNodeIndex             svc_nodes[AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM];
                                 //node indices.
                                 //service handles. Starts from the first stream service and
                                 //ends with the terminal service.
                                 //The terminal service could be Rx mtx port
} ;

/* the order in this struct cannot be changed as ASM_TOPO_TABLE relies on it. */
struct ASM_SvcList
{
   ASM_GraphNodeIndex              svc_nodes[AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM];
} ;

enum asm_mem_map_array_index_push_pull
{
   ASM_POS_BUF_INDEX = 0,
   ASM_CIRC_BUF_INDEX,
   ASM_MAX_MEM_MAP_ELEMENTS
};

/* order of enums is important as ASM_TOPO_TABLE relies on it. */
enum ASM_TopoId
{
   ASM_TOPO_NT_MODE = 0,               //0
   ASM_TOPO_RX_REGULAR,
   ASM_TOPO_RX_ULL,                    //2
   ASM_TOPO_RX_GAPLESS,                // although gapless is a session property, since topo is stream prop, gapless is listed here.
   ASM_TOPO_TX_REGULAR,
   ASM_TOPO_PCM2PCM_LOOPBACK,          //5
   ASM_TOPO_PCM2COMPR_LOOPBACK,
   ASM_TOPO_RX_COMPR,
   ASM_TOPO_TX_COMPR,
   ASM_TOPO_RX_PULL,                   //9
   ASM_TOPO_TX_PUSH,

   //special loopbacks
   ASM_TOPO_COMPR2COMPR_GEN_LOOPBACK,
   ASM_TOPO_COMPR2COMPR_CONV_LOOPBACK, //12
   ASM_TOPO_COMPR2COMPR_DIRECT_LOOPBACK,
   ASM_TOPO_COMPR2PCM_LOOPBACK,

   ASM_TOPO_RX_PULL_ULL, //15

   ASM_TOPO_TX_LLNP, //16
   ASM_TOPO_TX_PUSH_LLNP,

   ASM_TOPO_RX_ULLPP,   // 18
   ASM_TOPO_RX_PULL_ULLPP,

   ASM_TOPO_MAX                        //20
};

#define ASM_INVALID_TOPO ASM_TOPO_MAX

enum ASM_topo_type
{
   ASM_TOPO_TYPE_FLUSH_QUERY_DELAY_TOPO=0, //this node is also used for BW aggregation.
   ASM_TOPO_TYPE_SUSPEND_TOPO,
   ASM_TOPO_TYPE_PAUSE1_TOPO, //first step of pause : first step in the pause which is sent only to PPsvc to initiate soft pause. PP responds back after soft-pause is completed.
   ASM_TOPO_TYPE_PAUSE2_TOPO, //second step of pause : once soft-pause is completed ASM receives ACK from PP svc and then ASM sends pause2 to all required services. this cmd won't block in PP.
   	   	   	   	   	   	   	  //two-step pause was required for splitter use case in MPQ.
   ASM_TOPO_TYPE_RUN_TOPO,
   ASM_TOPO_TYPE_MAX
};

/* the order in this struct cannot be changed as ASM_TOPO_TABLE relies on it. */
struct ASM_topo_t
{
   ASM_TopoId        TopoId;
   ASM_StrmSvcList   StrmSvcInfo;
   ASM_SvcList       CmdTopo[ASM_TOPO_TYPE_MAX];
   /**
    * CmdTopo contains what previously was:
            ASM_SvcList       FlushAndQueryDelayTopo; //this node is also used for BW aggregation.
            ASM_SvcList       SuspendTopo;
            ASM_SvcList       PauseTopo1; //first step of pause
            ASM_SvcList       PauseTopo2; //second step of pause
            ASM_SvcList       RunTopo;
   */
};

enum ASM_STREAM_SORT_METHOD
{
   ASM_STREAM_SORT_SRC_TO_SINK = 0,
   ASM_STREAM_SORT_SINK_TO_SRC

};

enum ASM_direction
{
   ASM_RX_DIR = 0,
   ASM_TX_DIR,
   ASM_DONT_CARE_DIR,
   ASM_MAX_DIR
};

enum
{
   ASM_DEC_SVC = 0,
   ASM_ENC_SVC,
};

enum ASM_MtxStRtrType
{
   ASM_DEV_IF_NONE = 0,
   ASM_DEV_IF_MATRIX,
   ASM_DEV_IF_STREAM_ROUTER,
};

/** */
enum ASM_IoFormatConvType
{
   ASM_IO_FORMAT_CONV_AUTO = 0,           //decode raw to PCM OR encode PCM to raw.
   ASM_IO_FORMAT_CONV_61937_TO_PCM,       //depacketizer->decodes
   ASM_IO_FORMAT_CONV_61937_TO_61937,     //compressed pass through
   ASM_IO_FORMAT_CONV_61937_CONV_61937,   //depacketizer->conv->packetize
   ASM_IO_FORMAT_CONV_61937_TO_RAW,       //depacketize
   ASM_IO_FORMAT_CONV_RAW_TO_61937,       //packetize
   ASM_IO_FORMAT_CONV_PCM_TO_61937,       //encode->packetize.
   ASM_IO_FORMAT_CONV_61937_TO_MULTI      //input to MIMO decoder is 61937, output is not known.
};

/**
 * maximum number of pre-allocated sessions.
 * If more than below num of sessions are needed, then dynamic allocation takes place.
 */
#define ASM_MAX_PREALLOCED_SESSIONS 2

/* Invalid internal session id */
#define ASM_INVALID_INT_SESSION_ID (-1)

struct AudioStreamMgr_StreamStateType
{
   uint8_t              ucStreamId;  /**< ID of the stream: 1 to ASM_MAX_SUPPORTED_STREAM_ID*/

   ASM_GraphNode        nodes[ASM_NODE_MAX];  //indexed by ASM_NodeIndex

   const ASM_topo_t     *pTopo;

   asm_stream_state_t   state;
   uint32_t             bits_per_sample;
                                        /**< Used to determine the Tx matrix output bit width */
   uint32_t             unDataLogId;    /**< id for the PCM/bitstream logs: every session-stream has new id. <12 bit int. apr port id ><12 bit seq num><8 bit for flush etc> */
   uint32_t             src_fmt_id;     /**< hold dec_fmt_id of the streams in the session. can be zero for compressed passthru*/
   uint32_t             sink_fmt_id;    /**< hold the enc_cfg_id of the streams in the session.can be zero for compressed passthru*/

   uint32_t             pathFTRTBufDelayUs;  /**< buffering delay of the modules with faster than real time input in the path defined by the topology of the session/stream in microseconds. Indexed through portId.*/
   uint32_t             pathRTBufDelayUs;  /**< buffering delay of the modules with real time input in the path defined by the topology of the session/stream in microseconds. Indexed through portId.*/
   uint32_t             pathAlgDelayUs;  /**< algorithmic delay in the path defined by the topology of the session/stream in microseconds. Indexed through portId.*/
   uint16_t             stream_perf_mode;
                                        /**< indicates which performance mode this stream is opened.
                                             Supported values are only ASM_LEGACY_STREAM_SESSION,
                                             ASM_LOW_LATENCY_STREAM_SESSION &
                                             ASM_ULTRA_LOW_LATENCY_STREAM_SESSION .
                                             Currently all the streams in asession need to have same perf mode.
                                             In that case this becomes a session property, but kept here for future expansions */

   uint32_t             mem_map_handle[ASM_MAX_MEM_MAP_ELEMENTS]; /** memory map handle for push mode record or pull mode playback.
                                                first element for position buf, second for shared circ buf */

   AudioStreamMgr_MtMxPortParam        mtMxPortParam[ASM_MAX_DIR]; /**< also applicable to stream router.*/

   AudioStreamMgr_PPCallbackType       PPCallback;
                                        /**< Used to send data-path like message such as spa buffer
                                             to PP from AudioStreamMgr_AprCallback */
   AudioStreamMgr_CallBackHandleType   AsmCbData;
                                        /**< Used by Matrix to raise events */

   elite_svc_handle_t   *generic_rx_dummy_handle;
                                       /**<In case of AFE & LLNP/ULL Rx this is the dummy AFE handle. The real handle returned by AFE during map routing is stored in node. */
                     /**
                      * In ULL mode we actually need 2 handles
                      * 1. to store handle obtained from ADM for streamConnect(RequestRxDeviceHandle) so that it can be returned in streamDisconnect(ReturnDeviceHandle)
                      * 2. to store handle sent by AFE (through ADM) in UllAfeConnect so that we can use it to disconnect dec svc if stream is closed before AFE is closed (in AudioStreamMgr_StreamDisConnectGraph).
                      *
                      *  in ASM, we check handle's NULLness for sending returnDeviceHandle command. -- (A)
                      *
                      *  To satisfy both #2 and (A), we need to store dummy handle. dummy handle is used for purpose#1.
                      */
   bool_t             wait_for_eos_v2_ack;   /** After Dec sends EoS downstream it sends an event to ASM to wait for acks. in the mean time if close/flush come the wait resets.
                                                 else the event directly comes from matrix/stream router or AFE to ASM. then ASM sends rendered_eos_v2 to client.*/
};

struct ASM_StreamIdNodeIndex_t
{
   uint8_t              stream_id;
   ASM_GraphNodeIndex   node_ind;
};

struct ASM_StreamIdNodeIndices_t
{
   uint8_t  stream_id;
   ASM_GraphNodeIndex  cmd_node_ind[AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM];
      //unlike cmdTopo where nodes are not contiguous, here they are contiguous.
      //contiguous means valid nodes are not separated by invalid ones.
};

/**
 * session state created at boot-up
 */
struct AudioStreamMgr_StaticSessionStateType {
   qurt_elite_queue_t*     pSessionCmdQ;         /**< Client to AudioStreamMgr cmd Q */
   qurt_elite_queue_t*     pSessionRespQ;        /**< Other svcs to AudioStreamMgr response Q */
   uint32_t                unSessionCmdQMask;    /**< Mask for Session Cmd Q */
   elite_msg_any_t         cmdMsg;               /**< Current IPC command or custom cmd under processing*/
};
/*
The is the session state structure. It contains the necessary information
related with control a session
*/
struct AudioStreamMgr_SessionStateType {

   uint8_t                 ucInternalSessionId;/**< which group is this session. This is internal
                                             session ID starting from 0,
                                             which is different from ISOD session ID
                                             starting from 1.
                                             A utility function
                                             MAP_INTERNAL_SESSION_ID_TO_EXTERNAL can be
                                             used to map internal v.s. external session ID.
                                             invalid value: ASM_INVALID_INT_SESSION_ID*/

   AudioStreamMgr_StreamStateType aStreams[ASM_MAX_SUPPORTED_STREAMS_PER_SESSION];
                                        /**<Array of streams supported in this session. */

   ASM_StreamIdNodeIndices_t      aTempStreamIdNodeIndexList[ASM_MAX_SUPPORTED_STREAMS_PER_SESSION]; /**<array of streams & their nodes organized from source to sink or sink to source.
                                                                this is a temp array created for run,pause,suspend. */

   uint8_t                 ucStreamId2Index[ASM_MAX_SUPPORTED_STREAM_ID + 1];
                                        /**< indexed by stream id 1 to 16, gives index from 0 to ASM_MAX_SUPPORTED_STREAMS_PER_SESSION.*/

   elite_msg_any_t         msgIPC;              /**< Current IPC command under processing*/
   elite_msg_any_t         msg;                 /**< Current message from respQ under processing*/
   asm_session_mode_t 		unMode;             /**<  Type of the session */
   asm_session_state_t  	unState;            /**< State of this session.  */

   AudioStreamMgr_StreamStateType   *pFirstGaplessStrm;
                                          /**< Generally session = group of streams with a stream for each leg. however, in gapless we have only 2 stream but 3 legs.
                                               Hence, in order to handle the 3rd and common leg, we need info stored in the first stream. It's done by using this var.*/

   uint8_t                 oldestOpenStreamId; /**< stream id of the oldest open stream.
                                                Usually the opened stream, but in gapless stream it would be oldest. if 2 streams are open,
                                                when one is closed, this is assigned to the other open stream.
                                                **Potential problem**: in gapless if old ones is not closed after new is opened,
                                                then queries using this stream_id will not be correct.
                                                Fix for this involves event from switch svc & is complicated.

                                                0 is invalid*/

   uint32_t                unReqResps;         /**< Bitmask to track outstanding response that ASM expect.
                                             Set bit i of this number for internal commands
                                             ASM sent to module i and clear bit i of this number after
                                             ASM get a ACK from module i. When this
                                             becomes zero we know we have to ack the IPC command.*/
   int32_t                 nOverallResult;     /**< Overall result of the internal commands. This would be an OR
                                             of the individual results. Should be initialized to zero.
                                             If this is non-zero at the end of all responses, we know
                                             we should ack back the IPC command with error. */

   uint32_t                prev_total_kips;
   uint32_t                prev_floor_kips;

   asm_session_cmd_run_v2_t      prev_run_cmd;     /** cached run command to support opening of streams at run time.*/
};

/**
 * MMPM/ADSPPM related struct
 */
struct ASM_adsppm_t{
   uint32_t adsp_core_id;
   qurt_elite_adsppm_client_t *adsppm_wrapper;

   uint32_t prev_latency_vote;
   bool_t prev_latency_req_rel;
} ;

/*
The is the AudioStreamMgr structure.
*/
struct AudioStreamMgr_t
{


   elite_svc_handle_t       serviceHandle;       /**< handle to give out to others.
                                               The command queueue of this handle
                                               is the system queue that
                                               1. receives open command from Scorpion.
                                               2. recevies Elite system level message
                                               such as start_svc, destroy_svc.
                                               Currently, processing of command of this queue
                                               does not require a state machine.
                                               */

   elite_msg_any_t         msg;                 // Message from system cmdQ that is under processing

   qurt_elite_queue_t*     pDeviceSvcCmdQ;       //DeviceMgr cmd Q.
   qurt_elite_queue_t*     pAFESvcCmdQ;          //AFE svc cmd Q.

   qurt_elite_channel_t    channel;           //mask for MQ's owned by this obj
   uint32_t                unCurrentBitfield;      //contains the current channel mask or signals to act upon
   uint32_t                unChannelStatus;        //current signal received
   //Self Command Q buffer aligned to 8 bytest
   QURT_ELITE_ALIGN(char asmCmdQBuf[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(AUDIO_STREAM_MGR_MAX_SYS_CMD_Q_ELEMENTS)], 8);

   //event queue for events from dynamic services
   QURT_ELITE_ALIGN(char asmEvtQBuf[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(AUDIO_STREAM_MGR_MAX_SYS_EVT_Q_ELEMENTS)], 8);

   /* Once Q is added, then it cannot
      be removed. Hence, no point in dynamically creating
      session states. Just keep an array and recycle.
      */

   AudioStreamMgr_StaticSessionStateType aStSessions[ASM_MAX_SUPPORTED_SESSION_ID]; //static part of the session state struct (needed before session alloc)

   AudioStreamMgr_SessionStateType  aSessionsBuf[ASM_MAX_PREALLOCED_SESSIONS]; //sessions buffer, preallocated

   AudioStreamMgr_SessionStateType  *pSessions[ASM_MAX_SUPPORTED_SESSION_ID]; //array of session pointers.

   uint16_t                         unDataLogSeqNum;    //num for the PCM/bitstream logs: every session has new id.

   ASM_adsppm_t          adsppm;              //ADSPPM struct.
};

struct ASM_OpenStreamParams_t
{
   bool_t                     gapless_mode;
   bool_t                     sr_cm_event_needed;
   bool_t                     meta_info_flag;
   bool_t                     no_apr_enc_dec;
   bool_t                     is_rt_dec;  //flag to indicate that the decoder is real time decoder
   uint16_t                   stream_perf_mode;
   uint16_t                   sink_endpoint_type;
   uint16_t                   src_endpoint_type;
   uint16_t                   bits_per_sample;
   uint16_t                   stream_type;
   uint32_t                   decoder_conv_mode;
   uint32_t                   pp_topo_id;
   uint32_t                   src_fmt_id;  //previously, dec_fmt_id
   uint32_t                   sink_fmt_id;  //previously, enc_cfg_id
   uint32_t                   default_pp_sample_rate;
   uint32_t                   enc_frames_per_buffer;
   uint32_t                   timestamp_type;

   ASM_IoFormatConvType           dec_io_fmt_conv;
   ASM_IoFormatConvType           enc_io_fmt_conv;
};


/* AudioStreamMgr queue handler type*/
typedef int (*AudioStreamMgr_QueueHandler_f) (AudioStreamMgr_t* pMe, int32_t nChannelBit);

/* AudioStreamMgr SysCmdQ handler type*/
typedef int (*AudioStreamMgr_MsgHandler_f) (AudioStreamMgr_t* pMe);

/* AudioStreamMgr session resp/msg queue handler type*/
typedef int (*AudioStreamMgr_SessionMsgHandler_f) (AudioStreamMgr_t* pMe, AudioStreamMgr_SessionStateType *pSession);

/* AudioStreamMgr session cmd queue handler type*/
typedef int (*AudioStreamMgr_SessionCmdHandler_f) (AudioStreamMgr_t* pMe, uint8_t intSessionId);

/*Generic function pointer to send run/pause/flush  to svc */
typedef ADSPResult (*AudioStreamMgr_RunPauseFlushSvc_f) ( AudioStreamMgr_t* pMe,
                                                     AudioStreamMgr_SessionStateType *pSession,
                                                     AudioStreamMgr_StreamStateType *pStream,
                                                     uint32_t opcode,
                                                     uint8_t  unDir);

/*Generic function pointer to call create  svc */
typedef ADSPResult (*AudioStreamMgr_CreateDynaSvc_f) (AudioStreamMgr_t *pMe,
                                                   AudioStreamMgr_SessionStateType *pSession,
                                                   AudioStreamMgr_StreamStateType *pStream,
                                                   ASM_OpenStreamParams_t *pOpenStream);
/*Generic function pointer to send close  to svc */
typedef ADSPResult (*AudioStreamMgr_CloseDynaSvc_f) (AudioStreamMgr_t *pMe,
                                                   AudioStreamMgr_SessionStateType *pSession,
                                                   AudioStreamMgr_StreamStateType *pStream);

/*Generic function pointer to send close  to svc */
typedef ADSPResult (*AudioStreamMgr_ConnectDynaSvc_f) (AudioStreamMgr_t *pMe,
                                                   AudioStreamMgr_SessionStateType *pSession,
                                                   AudioStreamMgr_StreamStateType *pStream,
                                                   ASM_GraphNodeIndex upstream_node_ind,
                                                   ASM_GraphNodeIndex downstream_node_ind,
                                                   bool_t is_custom);

typedef ADSPResult (*ASM_ExecRunPauseSuspendFn) ( ASM_GraphNodeIndex node_index,
      AudioStreamMgr_t* pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream);;

/*Run/Pause/Flush function table entry with a function pointer and associated opcode */
struct ASM_RunPauseFlushFTableEntry
{
    AudioStreamMgr_RunPauseFlushSvc_f    func_ptr;
    uint32_t                             unOpcode;
    uint8_t                              unDir;
} ;

/*Run/Pause/Flush function table entry with a function pointer and associated opcode */
struct ASM_CreateDynaSvcFTableEntry
{
    AudioStreamMgr_CreateDynaSvc_f  func_ptr;
} ;

/*close */
struct ASM_CloseDynaSvcFTableEntry
{
    AudioStreamMgr_CloseDynaSvc_f  func_ptr;
} ;

/*Connect */
struct ASM_ConnectDynaSvcFTableEntry
{
   AudioStreamMgr_ConnectDynaSvc_f  func_ptr;
} ;

/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/
ADSPResult ASM_init_stream_topo(ASM_TopoId topoId, AudioStreamMgr_StreamStateType *s);
ASM_TopoId ASM_GetRxTopoId(uint16_t src, uint16_t sink);
ASM_TopoId ASM_GetLoopbackTopoIdAndMode(uint16_t src, uint16_t sink, uint32_t src_fmt, uint32_t sink_fmt, uint32_t dec_conv_mode,asm_session_mode_t *mode );
#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDIO_STREAM_MGR_TYPE_H
