/*========================================================================*/
/** @file EliteSvc_Util.h

This file contains structures and message ID's for communication between
Elite services.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      Elite_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*======================================================================
Copyright (c) 2009-2010, 2013-2016 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.  
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/19/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
08/25/15   sw      (Tech Pubs) General edits in Doxygen comments;
                   edited comments for 8996.2.7.
09/05/14   sw      (Tech Pubs) Merged Doxygen markup/comments from 2.4.
03/20/13   sw      (Tech Pubs) Updated Doxygen markup/comments for 2.0.
10/28/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
7/26/09    mwc      Created file.

========================================================================== */
#ifndef ELITESVC_UTIL_H
#define ELITESVC_UTIL_H


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "EliteMsg.h"
#include "EliteMsg_Data.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type definitions
** ----------------------------------------------------------------------- */

/** @ingroup elite_svc_datatypes
  Defines a function pointer signature for a message handler function.

  @datatypes
  elite_msg_any_t

  @param[in] pInstance   Pointer to the instance structure of the calling
                         service.
  @param[in] pMsg        Pointer to the message to process.

  @return
  Depends on the message, which is implicitly agreed upon between the caller
  and callee.

  @dependencies
  None.
 */
typedef ADSPResult (*elite_svc_msg_handler_func) (void* /*pInstance*/, elite_msg_any_t* /*pMsg*/);

/****************************************************************************
** Service utility functions.
*****************************************************************************/

/** @ingroup elitesvc_func_process_cmd_q
  Loops through the command queue and calls the handler for each function. This
  function finds the handler function via the table lookup from the message ID.

  @datatypes
  elite_svc_handle_t \n
  #elite_svc_msg_handler_func

  @param[in] pInstance   Pointer to the instance of the calling service, to
                         be passed to the handler.
  @param[in] pHandle     Pointer to the service handle of the calling service.
  @param[in] pHandler    Handler function lookup table of the calling service.
  @param[in] handler_table_size  Number of elements in the lookup table.

  @return
  None.

  @dependencies
  None.
*/
ADSPResult elite_svc_process_cmd_queue(void* pInstance, elite_svc_handle_t *pHandle,
   elite_svc_msg_handler_func pHandler[], uint32_t handler_table_size);

/** @ingroup elitesvc_unsupported
  Returns the ADSP_UNSUPPORTED error code. The handler table for a service can
  reference this function for unsupported messages.

  @datatypes
  elite_msg_any_t

  @param[in] pInstance   Pointer to the instance structure of the calling
                         service.
  @param[in] pMsg        Pointer to the message that requires handling.

  @return
  ADSP_UNSUPPORTED.

  @dependencies
  None.
*/
ADSPResult elite_svc_unsupported (void* pInstance, elite_msg_any_t* pMsg);

/** @ingroup elitesvc_func_return_success
  Referenced by the handler table for a service to return an ADSP_EOK error
  code for a message.

  @datatypes
  elite_msg_any_t

  @param[in] pInstance   Pointer to the instance structure of the calling
                         service.
  @param[in] pMsg        Pointer to the message that requires handling.

  @return
  ADSP_EOK.

  @dependencies
  None.
*/
ADSPResult elite_svc_return_success (void* pInstance, elite_msg_any_t* pMsg);

/** @ingroup elitesvc_func_destroy_data_q
  Flushes and destroys a data queue. This function is called by typical service
  destructors.

  @datatypes
  qurt_elite_queue_t (refer to @xrefcond{Q5,80-NF774-7,80-NA610-7})

  @param[in] pDataQ    Pointer to the data queue of the service.

  @return
  None.

  @dependencies
  None.
*/
void elite_svc_destroy_data_queue (qurt_elite_queue_t* pDataQ);

/** @ingroup elitesvc_func_destroy_cmd_q
  Flushes and destroys a command queue. This function is called by typical
  service destructors.

  @datatypes
  qurt_elite_queue_t (refer to @xrefcond{Q5,80-NF774-7,80-NA610-7})

  @param[in] cmdQ    Pointer to the command queue of the service.

  @return
  None.

  @dependencies
  None.
*/
void elite_svc_destroy_cmd_queue (qurt_elite_queue_t* cmdQ);

/** @ingroup elitesvc_func_free_buf_q
  Awaits the return of outstanding buffers to their queue, and then frees the
  buffers but does not destroy the buffer queue.

  This function is called when it is required to free buffers but not destroy
  the queue.

  @datatypes
  qurt_elite_queue_t (refer to @xrefcond{Q5,80-NF774-7,80-NA610-7})

  @param[in] bufQ    Pointer to the buffer queue descriptor.
  @param[in] nBufs   String identifier, which is required to unlink the queue.

  @return
  None.

  @dependencies
  None.
*/
void elite_svc_free_buffers_in_buf_queue (qurt_elite_queue_t* bufQ, int nBufs);


/** @ingroup elitesvc_func_destroy_buf_q
  Awaits the return of outstanding buffers to their queue, and then destroys
  the buffer queue, thus freeing the buffers.

  This function is to be called by typical service destructors.

  @datatypes
  qurt_elite_queue_t (refer to @xrefcond{Q5,80-NF774-7,80-NA610-7})

  @param[in] bufQ     Pointer to the buffer queue descriptor.
  @param[in] nBufs    String identifier, which is required to unlink the queue.

  @return
  None.

  @dependencies
  None.
*/
void elite_svc_destroy_buf_queue (qurt_elite_queue_t* bufQ, int nBufs);

/** @ingroup elitesvc_func_deinit_cmd_q
  Flushes and destroys a command queue. This function is called by typical
  service destructors.

  @datatypes
  qurt_elite_queue_t (refer to @xrefcond{Q5,80-NF774-7,80-NA610-7})

  @param[in] cmdQ    Pointer to the queue descriptor.

  @return
  None.

  @dependencies
  Before calling this function, qurt_elite_queue_init() was called
  to create the command queue. (For information on qurt_elite_queue_init(),
  refer to @xrefcond{Q5,80-NF774-7,80-NA610-7}).
*/
void elite_svc_deinit_cmd_queue (qurt_elite_queue_t* cmdQ);

/** @ingroup elitesvc_func_deinit_buf_q
  Awaits the return of outstanding buffers to their queue, and then destroys
  the buffer queue, freeing the buffers.
  This function is called by typical service destructors.

  @datatypes
  qurt_elite_queue_t (refer to @xrefcond{Q5,80-NF774-7,80-NA610-7})

  @param[in] bufQ     Pointer to the buffer queue descriptor.
  @param[in] nBufs    String identifier, which is required to unlink the queue.

  @return
  None.

  @dependencies
  Before calling this function, qurt_elite_queue_init() was called
  to create the buffer queue. (For information on qurt_elite_queue_init(),
  refer to @xrefcond{Q5,80-NF774-7,80-NA610-7}).
*/
void elite_svc_deinit_buf_queue (qurt_elite_queue_t* bufQ, int nBufs);


/** @ingroup elitesvc_func_deinit_data_q
  Flushes and destroys a data queue.
  This function is called by typical service destructors.

  @datatypes
  qurt_elite_queue_t (refer to @xrefcond{Q5,80-NF774-7,80-NA610-7})

  @param[in] pDataQ     Pointer to the data queue descriptor.

  @return
  None.

  @dependencies
  Before calling this function, qurt_elite_queue_init() was called
  to create the data queue. (For information on qurt_elite_queue_init(),
  refer to @xrefcond{Q5,80-NF774-7,80-NA610-7}).
*/
void elite_svc_deinit_data_queue (qurt_elite_queue_t* pDataQ);

/** @ingroup elitesvc_func_send_ack
  Forms and sends an acknowledgment to a command if the command requested
  an acknowledgment.

  If the command did not request an acknowledgment, this function does nothing.
  The function can be called without checking whether an acknowledgment is
  required.

  @datatypes
  elite_msg_any_t

  @param[in] pMsg     Pointer to the message that might need an acknowledgment.
  @param[in] result   Error status to report in the acknowledgment.

  @return
  None.

  @dependencies
  None.
*/
void elite_svc_send_ack (elite_msg_any_t *pMsg, ADSPResult result);

/** @ingroup elitesvc_func_wait_ack
  Called after sending a message to await an acknowledgment.

  @datatypes
  elite_msg_any_t

  @param[in] pMsg    Pointer to the message that might need an acknowledgment.

  @return
  Error status is returned in the acknowledgment.

  @dependencies
  None.
*/
ADSPResult elite_svc_wait_for_ack(elite_msg_any_t *pMsg);

/** @ingroup elitesvc_func_wait_ack_get_msg
  Called after sending a message to await an acknowledgment. On a successful
  return, the response contains the new message, and it might contain a
  payload.

  @datatypes
  elite_msg_any_t

  @param[in]  pMsg        Pointer to the message that might need an
                          acknowledgment.
  @param[out] pResponse   Pointer to the response message that is sent as an
                          acknowledgment.

  @return
  Error status is returned in the acknowledgment.

  @dependencies
  None.
*/
ADSPResult elite_svc_wait_for_ack_and_get_msg(elite_msg_any_t *pMsg, elite_msg_any_t *pResponse);

/** @ingroup elitesvc_func_get_frame_size
  Called by the services to determine the operative frame size in samples based
  on the input sampling rate. The service must validate the sample rate before
  calling this function.

  @param[in] sample_rate      Sampling rate for which the frame size in samples
                              must be calculated.
  @param[in] num_samples_ptr  Pointer to the frame size in samples to report
                              in an acknowledgement.

  @return
  Frame size is returned in an acknowledgement.

  @dependencies
  None.
*/
static inline void elite_svc_get_frame_size(uint32_t sample_rate, uint32_t *num_samples_ptr)
{
    //Returns 1 sample as a minimum value
    if(sample_rate < 1000)
    {
        *num_samples_ptr = 1;
        return;
    }
    *num_samples_ptr = (sample_rate/1000);
}

/** @ingroup elitesvc_func_raise_eosv2_evt
  Called by the services to push a rendered EOS version 2 event to the ASM.

  @datatypes
  elite_msg_data_eos_info_t

  @param[in] pEoSInfo      EoS Information structure.

  @return
  Result of the function call.

  @dependencies
  None.
*/
ADSPResult elite_svc_raise_rendered_eos_v2_event (elite_msg_data_eos_info_t *pEoSInfo);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITESVC_UTIL_H

