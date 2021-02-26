/**========================================================================
 @file vpm_svc_apr_data_base.h
 
 @brief This header file contains varaiable and function declarations
 for APR data base manager
 
 Copyright (c) 2016 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_svc_apr_data_base.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

#ifndef _VPM_SVC_APR_DATABASE_H_
#define _VPM_SVC_APR_DATABASE_H_

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qurt_elite.h"
#include "Elite.h"

#include "vpm_internal.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct vpm_rnode
{
    qurt_elite_queue_t  *cmdQ; /* Session command Q */

    qurt_elite_queue_t  *rspQ; /* Session response Q */

#if 0
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
#endif

} vpm_rnode_t;


/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

static inline uint8_t VPM_GET_SESSION_ID(elite_apr_port_t port)
{
   return (uint8_t) (port & 0x00FF);
}

static inline elite_apr_port_t VPM_GET_APR_PORT_ID(uint16_t session_id)
{
   elite_apr_port_t port = (elite_apr_port_t) (  (((uint16_t)1) << 8) | ((uint16_t)session_id)  );

   return port;
}

void  voice_proc_mgr_apr_router_lock();

void  voice_proc_mgr_apr_router_unlock();

ADSPResult voice_proc_mgr_apr_router_init(voice_proc_mgr_t *pInst);

ADSPResult voice_proc_mgr_apr_router_reg_cmd_q( uint8_t major, qurt_elite_queue_t *pCmdQ );

ADSPResult  voice_proc_mgr_apr_router_get_session_cmd_q(elite_apr_port_t port, qurt_elite_queue_t **pCmdQ);

ADSPResult  voice_proc_mgr_apr_router_get_session_rsp_q(elite_apr_port_t port, qurt_elite_queue_t **pCmdQ);

/**
This function gets the system command Q *************************

@param pCmdQ [out] This is the system commnd Q
@return Success or failure of the instance creation.
*/
ADSPResult voice_proc_mgr_apr_router_get_svc_cmd_q(qurt_elite_queue_t **p_cmd_q);

ADSPResult voice_proc_mgr_apr_router_get_svc_rsp_q(qurt_elite_queue_t **p_rsp_q);

#endif /* #ifndef _VPM_SVC_APR_DATABASE_H_ */
