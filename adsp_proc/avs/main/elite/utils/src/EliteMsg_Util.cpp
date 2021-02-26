/**
@file EliteMsg_Util.cpp

@brief This file contains the implementation for Elite
message utility functions.

*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/04/10   DC      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/




/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                          Function Definitions
** ======================================================================= */

ADSPResult elite_msg_create_msg( elite_msg_any_t       *msg_ptr,
                                 uint32_t              *buf_size_ptr,
                                 uint32_t              opcode,
                                 qurt_elite_queue_t    *resp_q_ptr,
                                 uint32_t              client_token,
                                 uint32_t              resp_result
                                  )

{
    ADSPResult nRes;
    qurt_elite_bufmgr_node_t bufNode;
    int nActualSize;
    if ( ADSP_FAILED( nRes= elite_mem_get_buffer( *buf_size_ptr,
                                       &bufNode,  &nActualSize) ) ) {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Fail to get command buffer");
        return nRes;
    }

    /** Fill payload buffer  */
    elite_msg_any_payload_t *payload_ptr = (elite_msg_any_payload_t *) bufNode.pBuffer;

    payload_ptr->pBufferReturnQ     = bufNode.pReturnQ;
    payload_ptr->pResponseQ         = resp_q_ptr;
    payload_ptr->unClientToken      = client_token;
    payload_ptr->unResponseResult   = resp_result;

    /** Fill message */
    msg_ptr->unOpCode                  = opcode;
    msg_ptr->pPayload                  = payload_ptr;

    return ADSP_EOK;
}

ADSPResult elite_msg_push_payload_to_returnq(qurt_elite_queue_t* return_q_ptr, elite_msg_any_payload_t* payload_ptr)
{
   //check if the buffer is from buffer manager
   if (qurt_elite_is_bufmgr_node(elite_mem_global_pool_global.pEliteMemBufMgr,
                                 return_q_ptr, 
                                 payload_ptr)) 
   {
      //return to buffer manager
      return qurt_elite_bufmgr_return_buf(payload_ptr);
   }
   else
   {
      qurt_elite_bufmgr_node_t bufMgrNode;
      bufMgrNode.pReturnQ = return_q_ptr;
      bufMgrNode.pBuffer  = payload_ptr;
      return qurt_elite_queue_push_back( return_q_ptr, (uint64_t*)(&bufMgrNode) );
   }
}

ADSPResult elite_msg_ack_msg(const elite_msg_any_t *msg_ptr, uint32_t resp_result)
{
   elite_msg_any_payload_t *payload_ptr = (elite_msg_any_payload_t*) msg_ptr->pPayload;
   payload_ptr->unResponseResult = resp_result;
   return qurt_elite_queue_push_back( payload_ptr->pResponseQ, (uint64_t*)(msg_ptr) );
}

ADSPResult elite_msg_release_msg(const elite_msg_any_t *msg_ptr)
{
   elite_msg_any_payload_t *payload_ptr = (elite_msg_any_payload_t *)msg_ptr->pPayload;
   qurt_elite_queue_t *return_q_ptr = payload_ptr->pBufferReturnQ;
   return elite_msg_push_payload_to_returnq(return_q_ptr, payload_ptr);
}

ADSPResult elite_msg_return_payload_buffer(elite_msg_any_t *msg_ptr)
{
   return elite_msg_release_msg(msg_ptr);
}

ADSPResult elite_msg_finish_msg( const elite_msg_any_t *msg_ptr, uint32_t resp_result )
{
   /* Send response if response q is available,
      otherwise return the payload to return q. */
   elite_msg_any_payload_t *payload_ptr = (elite_msg_any_payload_t*) msg_ptr->pPayload;
   if ( payload_ptr->pResponseQ ) 
   {
      return elite_msg_ack_msg(msg_ptr, resp_result);
   } 

   return elite_msg_release_msg(msg_ptr);
}

elite_msg_any_t* elite_msg_convt_buf_node_to_msg(
                    qurt_elite_bufmgr_node_t* node_ptr,    
                    uint32_t                  opcode,      
                    qurt_elite_queue_t*       resp_q_ptr,  
                    uint32_t                  client_token,
                    uint32_t                  resp_result)
{
    if (!node_ptr) return NULL;
    
    elite_msg_any_payload_t *payload_ptr = (elite_msg_any_payload_t *) node_ptr->pBuffer;

    payload_ptr->pResponseQ         = resp_q_ptr;
    payload_ptr->unClientToken      = client_token;
    payload_ptr->pBufferReturnQ     = node_ptr->pReturnQ;
    payload_ptr->unResponseResult   =  resp_result;

    elite_msg_any_t *msg_ptr = (elite_msg_any_t *)node_ptr;
    msg_ptr->unOpCode = opcode;
    
    return msg_ptr;
}







