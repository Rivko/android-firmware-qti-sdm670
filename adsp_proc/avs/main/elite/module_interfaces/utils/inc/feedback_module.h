/*========================================================================
 $Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/utils/inc/feedback_module.h#1 $

 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 7/22/2014   rv       Created

 ==========================================================================*/

/*-----------------------------------------------------------------------
 This file contains Feedback implementation to be used by CAPI V2 modules for
 communicating between them.  

 Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
 -----------------------------------------------------------------------*/

#ifndef _FEEDBACK_MODULE_H_
#define _FEEDBACK_MODULE_H_

/*==========================================================================
 Include files
 ========================================================================== */
#include "qurt_elite.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

/*==========================================================================
 Function Declarations
 ========================================================================== */
/**
 * This function is used to return the buffer to the return Q.
 */
ADSPResult feedback_push_buf_to_buf_q(void *client_info_ptr, void *pPayLoad);

/**
 * This function is used to pop a buffer from the data Q.
 */
ADSPResult feedback_pop_buf_from_data_q(void *client_info_ptr, void **pPayLoad);

/**
 * This function is used to pop a buffer from the buffer Q.
 */
ADSPResult feedback_pop_buf_from_buf_q(qurt_elite_queue_t *buf_q_ptr, void **payload_pptr);

/**
 * This function is used to send the buffer to the data Q.
 */
ADSPResult feedback_send_buf_to_data_q(qurt_elite_queue_t *data_q_ptr, void *payload_ptr);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _FEEDBACK_MODULE_H_ */
