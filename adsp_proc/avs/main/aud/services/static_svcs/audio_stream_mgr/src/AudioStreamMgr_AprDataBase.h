/**
@file AudioStreamMgr_Type.h
@brief This file declares some common data types used by AudioStreamMgr.

*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_AprDataBase.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/24/2010 DG      Added support for handling the callback function for PP.
04/01/2010 DC      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef AUDIO_STREAM_MGR_APR_DATABASE_H
#define AUDIO_STREAM_MGR_APR_DATABASE_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qurt_elite.h"
#include "Elite.h"
#include "AudioStreamMgr_Type.h"
#include "AudioStreamMgr_PrivateDefs.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

enum {
   AUDIO_STREAM_MGR_APR_ROUTER_RX_DIR = 0,
   AUDIO_STREAM_MGR_APR_ROUTER_TX_DIR
};

typedef struct {
    qurt_elite_queue_t  *cmdQ;                          // Only one command q per group
    qurt_elite_queue_t  *dataQ[ASM_MAX_SUPPORTED_STREAM_ID+1][2];   /* Array of dataQ pointers. each
                                                                handle can be associated with
                                                                one read buffer and one write
                                                                buffer.
                                                                At IPC level, Stream ID 1- ASM_MAX_SUPPORTED_STREAM_ID.
                                                                For easy of implementation, add empty space ahead.
                                                                */
    AudioStreamMgr_PPCallbackType PPCallback[ASM_MAX_SUPPORTED_STREAM_ID + 1]; /* Callbacks for sending messages directly
                                                                              to PP, without going through the
                                                                              ASM thread. */
} AudioStreamMgr_RNodeType;


/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

static inline uint8_t ASM_GET_STREAM_ID(elite_apr_port_t port)
{
   return (uint8_t) (port & 0x00FF );
}

static inline uint8_t ASM_GET_GROUP_ID(elite_apr_port_t port)
{
   return (uint8_t) (port >> 8);
}

static inline elite_apr_port_t ASM_GET_PORT_ID(uint8_t group_id, uint8_t stream_id)
{
   elite_apr_port_t port = (elite_apr_port_t) (  (((uint16_t)group_id)<<8) | ((uint16_t)stream_id)  );

   return port;
}

void  AudioStreamMgr_AprRouterLock();

void  AudioStreamMgr_AprRouterUnLock();

ADSPResult AudioStreamMgr_AprRouterInit(AudioStreamMgr_t *pInst);

ADSPResult AudioStreamMgr_AprRouterRegisterCmdQ( uint8_t major, qurt_elite_queue_t *pCmdQ );

ADSPResult AudioStreamMgr_AprRouterRegisterDatQ( elite_apr_port_t port,
                                                 uint32_t   unDirection, /**< 0 for register Rx data queue
                                                                         1 for register Tx data queue
                                                                         */
                                                 qurt_elite_queue_t *pDatQ

                                                   );

ADSPResult AudioStreamMgr_AprRouterDeRegisterDatQ( elite_apr_port_t port,
                                                  uint32_t   unDirection /**< 0 for register Rx data queue
                                                                         1 for register Tx data queue
                                                                         */
                                                   );

/**
 * This function adds a PP callback function and context to the
 * APR database. This function can be used to send messages
 * directly to PP. It is meant to be used for data path messages
 * such as Spectrum Analyzer data buffers.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in] port
 *  The port for which this PP callback will be used.
 * @param[in] callback
 *  The PP callback function and context, which will be added to
 *  the database.
 *
 * @return ADSPResult
 *  Indicates whether the callback was added successfully or
 *  not.
 */
ADSPResult AudioStreamMgr_AprRouterRegisterPPCb(elite_apr_port_t port,
                                                AudioStreamMgr_PPCallbackType callback);

/**
 * This function removes the PP callback data for a particular
 * port. The corresponding entry is cleared, so no more messages
 * will be sent to that PP.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in] port
 *  The port for which the PP callback entry is to be cleared.
 *
 * @return ADSPResult
 *  Indicates whether the callback was removed successfully or
 *  not.
 */
ADSPResult AudioStreamMgr_AprRouterDeRegisterPPCb( elite_apr_port_t port );

//ADSPResult AudioStreamMgr_AprRouterRegistercCmdQ( uint8_t major, qurt_elite_queue_t *pCmdQ )
ADSPResult  AudioStreamMgr_AprRouterGetDataQ( elite_apr_port_t port,
                                             uint32_t unDirection, /**< 0 for register Rx data queue
                                                                         1 for register Tx data queue
                                                                         */
                                                    qurt_elite_queue_t **pDataQ
                                             );

ADSPResult  AudioStreamMgr_AprRouterGetCmdQ(elite_apr_port_t port, qurt_elite_queue_t **pCmdQ);

/**
 * This function calls the PP callback for a port, in order to
 * directly send a message to PP. This will bypass the ASM
 * thread, so it can be used for low-latency requirements.
 *   - Dependencies: The APR router mutex must be locked prior
 *     to calling this function, using
 *     AudioStreamMgr_AprRouterLock.
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in] port
 *  The port whose PP callback function is to be called.
 * @param[in] msg
 *  The message to be sent to PP.
 *
 * @return ADSPResult
 *  Indicates whether the message was successfully queued. If a
 *  failure code is returned, the message was not sent to PP and
 *  it must be acked with failure by the caller.
 */
ADSPResult  AudioStreamMgr_CallPPCb(elite_apr_port_t port,
                                    elite_msg_any_t msg);

/**
This function gets the system command Q *************************

@param pCmdQ [out] This is the system commnd Q
@return Success or failure of the instance creation.
*/
ADSPResult AudioStreamMgr_GetSysCmdQ(qurt_elite_queue_t **pCmdQ);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif
