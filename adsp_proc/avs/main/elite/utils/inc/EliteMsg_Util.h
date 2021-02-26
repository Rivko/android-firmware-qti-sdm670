#ifndef ELITEMSG_UTIL_H
#define ELITEMSG_UTIL_H

/**
@file EliteMsg_Util.h

@brief This file defines common utility function for Elite
messages.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. 
      The description that displays in the PDF is located in the
      Elite_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*========================================================================
   Copyright (c) 2010, 2013-2015 Qualcomm Technologies, Inc.  All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/25/15   sw      (Tech Pubs) General edits in Doxygen comments.
09/05/14   sw      (Tech Pubs) Merged Doxygen markup/comments from 2.4.
03/20/13   sw      (Tech Pubs) Updated Doxygen comments and markup for 2.0.
10/28/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
02/04/10   DC      Created file.
==========================================================================*/


/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/
/* System */
#include "qurt_elite.h"

/* Audio */
#include "EliteMsg.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/

/** @ingroup elitemsg_func_create
  Creates a payload buffer based on the size requested, and fills the buffer
  return queue, response queue, client token, and response result fields in
  the payload buffer. Then it fills the operation code and payload into an
  Elite message.

  The caller is responsible for filling any fields in the payload structure
  that come after the unResponseResult field.

  @datatypes
  elite_msg_any_t \n
  qurt_elite_queue_t (refer to @xrefcond{Q5,80-NF774-7,80-NA610-7})

  @param[in,out] pAnyMsg            Pointer to the message to be filled in.
  @param[in,out] punPayLoadBufSize  On input, pointer to the payload buffer
                                    size requested.\n
                                    On output, pointer to the actual size of
                                    the buffer created. Only the caller knows
                                    the requested size because it knows the
                                    secondary operation code or parameter ID.
  @param[in]     unOpCode           Primary operation code to fill in the
                                    message.
  @param[in]     pResponseQ         Pointer to the response of this message.
  @param[in]     unClientToken      Client token of this message.
  @param[in]     unResponseResult   Response result of this message.

  @return
  Indication of success or failure.

  @dependencies
  None.
 */
ADSPResult elite_msg_create_msg( elite_msg_any_t       *pAnyMsg,
                                     uint32_t             *punPayLoadBufSize,
                                     uint32_t                unOpCode,
                                     qurt_elite_queue_t       *pResponseQ,
                                     uint32_t                unClientToken,
                                     uint32_t                unResponseResult
                                  );
/** @ingroup elitemsg_func_release
  Returns the payload buffer associated with the message back to its buffer
  queue.

  @datatypes
  elite_msg_any_t

  @param[in] pAnyMsg    Pointer to the message whose payload buffer is to be
                        returned.

  @return
  Indication of success or failure.

  @dependencies
  None.
 */
ADSPResult elite_msg_release_msg( const elite_msg_any_t       *pAnyMsg );

/** @ingroup elitemsg_func_ack
  Sends an acknowledgment of a message.

  The owner of the message also owns the associated payload buffer. Hence,
  elite_msg_release_msg() and elite_msg_ack_msg() are mutually exclusive in
  the same message.

  @datatypes
  elite_msg_any_t

  @param[in] pAnyMsg            Pointer to the message to be acknowledged.
  @param[in] unResponseResult   Response result indicator.

  @return
  Indication of success or failure.

  @dependencies
  None.
 */
ADSPResult elite_msg_ack_msg( const elite_msg_any_t *pAnyMsg, uint32_t unResponseResult ) ;

/** @ingroup elitemsg_func_finish
  Called after a message was processed and now it is ready to be released or
  acknowledged.

  @datatypes
  elite_msg_any_t

  @param[in] pAnyMsg            Pointer to the message that was processed.
  @param[in] unResponseResult   Fills the response result if the message
                                sends back an acknowledgment.

  @return
  Indication of success or failure.

  @dependencies
  None.
 */
ADSPResult elite_msg_finish_msg( const elite_msg_any_t *pAnyMsg, uint32_t unResponseResult ) ;

/** @ingroup elitemsg_func_conv_buf_node
  Fills the operation code and payload into an Elite message payload buffer.

  @param[in] pBufMgrNode        Pointer to the message payload buffer.
  @param[in] unOpCode           Primary operation code.
  @param[in] pResponseQ         Pointer to the response to fill in the message.
  @param[in] unClientToken      Token to fill in the message.
  @param[in] unResponseResult   Response result.

  @return
  elite_msg_any_t* -- Message with the payload.

  @dependencies
  None.
 */
elite_msg_any_t* elite_msg_convt_buf_node_to_msg(
                                      qurt_elite_bufmgr_node_t  *pBufMgrNode,
                                      uint32_t                unOpCode,
                                      qurt_elite_queue_t       *pResponseQ,
                                      uint32_t                unClientToken,
                                      uint32_t                unResponseResult
                                    );

/** @ingroup elitemsg_func_return_payld_buf
  Returns the payload buffer to the designated buffer queue when given a
  message with a pointer to the payload buffer. For efficiency, the caller
  ensures that the BufferReturnQ always exists in the payload header.

  @datatypes
  elite_msg_any_t

  @param[in] pEliteMsg    Pointer to the message to be released.

  @return
  ADSPResult -- Success or failure code.

  @dependencies
  None.
 */
ADSPResult elite_msg_return_payload_buffer( elite_msg_any_t       *pEliteMsg );


/** @ingroup elitemsg_func_push_payld
  Directly pushes a payload to its return queue.

  This function directly pushes any payloads to its return queue. It is
  used when a service creates its data buffers and wants to push them into
  its own buffer queue.

  @datatypes
  qurt_elite_queue_t (refer to @xrefcond{Q5,80-NF774-7,80-NA610-7}) \n
  elite_msg_any_payload_t

  @param[in]  pReturnQ      Pointer to the return queue. The caller must
                            guarantee that pReturnQ is not NULL.
  @param[in]  pAnyPayload   Pointer to the payload to be released.

  @return
  ADSPResult -- Success or failure code.

  @dependencies
  None.
  */
ADSPResult elite_msg_push_payload_to_returnq(qurt_elite_queue_t* pReturnQ, elite_msg_any_payload_t* pAnyPayload);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITEMSG_UTIL_H

