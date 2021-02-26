/*========================================================================
Copyright (c) 2014-2016 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================= */
/**
@file qurt_elite_queue_v2.h

@brief This file contains the queue utilities. Queues must be created and added
to a channel before they can be used. Queues are pushed from the back and can be
popped from either front(FIFO) or back(LIFO). Queues must be destroyed when
they are no longer needed.

v2 is implemented to make qurt_elite independent of qurt.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The weakgroup description below displays in the PDF.
===========================================================================*/

/** @weakgroup weakf_qurt_elite_queuev2_intro
Version 2 of the queue utilities decouples QuRT_Elite from the underlying QuRT
functions, and thus provides better portability. Inline function and inline
structure definitions are removed. New functions are added to extend version 1
of qurt_elite_queue.

Only the version 2 functions are documented in this section. The version 1
functions are documented in Section @xref{dox:QueueUtilities}; they are not
repeated here.
 - qurt_elite_queue_create()
 - qurt_elite_queue_delete() -- Version 2
 - qurt_elite_queue_disable()
 - qurt_elite_queue_push_back()
 - qurt_elite_queue_pop_front()
 - qurt_elite_queue_pop_back()
 - qurt_elite_queue_get_channel()
 - qurt_elite_queue_get_channel_bit()
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/qurt_elite/qurt_elite_queue_v2.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/16/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
01/08/16   sw       (Tech Pubs) General edits in Doxygen comments.
05/11/15   sw      (Tech Pubs) Merged Doxygen comments from 2.6; edited for 8952.
07/17/15   mh      Removing channel group support
10/30/14   rbhatnk     Created file.
========================================================================== */

#ifndef QURT_ELITE_QUEUE_V2_H
#define QURT_ELITE_QUEUE_V2_H

#include "adsp_error_codes.h"
#include "qurt_elite_types.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */
typedef qurt_elite_queue_t qurt_elite_queue_t;

/** @ingroup qurt_elite_queue2
    Maximum number of characters to use in resource names (e.g., thread names).
*/
#define QURT_ELITE_DEFAULT_NAME_LEN 16

/* V1 function not repeated in the PDF. */
/*
  Creates and initializes a queue of 64-bit unsigned integers.

  @datatypes
  qurt_elite_queue_t

  @param[in]  pzQName     Pointer to the name of the queue.
  @param[in]  nMaxItems   Maximum number of items that can be pushed into the
                          queue. This value must be a power of 2.
  @param[out] pQ          Double pointer to the created queue.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  None. @newpage
*/
ADSPResult qurt_elite_queue_create(char *pzQName, int nMaxItems, qurt_elite_queue_t** pQ);

/** @ingroup qurt_elite_queue2
  Deletes a queue.

  @datatypes
  qurt_elite_queue_t

  @param[in,out] pQ   Pointer to the queue.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
void qurt_elite_queue_delete(qurt_elite_queue_t* pQ);


/* NOTE - Following v1 functions are not repeated in the PDF */
/*
  Disables a queue.

  @datatypes
  qurt_elite_queue_t

  @param[in] pQ   Pointer to the queue.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
*/
ADSPResult qurt_elite_queue_disable(qurt_elite_queue_t* pQ);

/*
  Pushes an item onto a queue.

  @datatypes
  qurt_elite_queue_t

  @param[in] pQ        Pointer to the queue.
  @param[in] pPayload  Pointer to the address (location) of the item. \n
                       The item is pushed (copied) onto the queue, not the
                       queue address.

  @detdesc
  This function is nonblocking. The user is responsible for not overflowing
  the queue to avoid getting an unwanted assertion.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
*/
ADSPResult qurt_elite_queue_push_back(qurt_elite_queue_t* pQ, uint64_t* pPayload);

/*
  Pops an item from the front of a queue.

  @datatypes
  qurt_elite_queue_t

  @param[in] pQ        Pointer to the queue.
  @param[in] pPayload  Pointer to the target address (location) for the item
                       that is popped.

  @detdesc
  This function is nonblocking and returns ADSP_ENOMORE if it is empty.
  @par
  Typically, the client calls this function only after waiting for a channel
  and checking whether this queue contains any items.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
*/
ADSPResult qurt_elite_queue_pop_front(qurt_elite_queue_t* pQ,  uint64_t* pPayload);

/*
  Pops an item from the back of a queue.

  @datatypes
  %qurt_elite_queue_t

  @param[in] pQ        Pointer to the queue.
  @param[in] pPayload  Pointer to the target address (location) for the item
                       that is popped.

  @detdesc
  This function is for LIFO queues. It is nonblocking and returns ADSP_ENOMORE
  if it is empty.
  @par
  Typically, the client calls this function only after waiting for a channel
  and checking whether this queue contains any items.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
*/
ADSPResult qurt_elite_queue_pop_back(qurt_elite_queue_t* pQ,  uint64_t* pPayload );

/*
  Queries a queue for its channel.

  @datatypes
  qurt_elite_queue_t

  @param[in] pQ   Pointer to the queue.

  @return
  A handle to the channel containing the queue.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
qurt_elite_channel_t* qurt_elite_queue_get_channel(qurt_elite_queue_t* pQ);

/*
  Queries a queue for its channel bitfield.

  @datatypes
  qurt_elite_queue_t

  @param[in] pQ   Pointer to the queue.

  @detdesc
  This is a 32-bit value with a single bit=1.
  @par
  Use this function to find the values to be ORed together to form an enable
  bitfield for a combination of queues in a channel.

  @return
  A bitfield with this queue's bit set to 1.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
uint32_t qurt_elite_queue_get_channel_bit(qurt_elite_queue_t* pQ);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_QUEUE_V2_H

