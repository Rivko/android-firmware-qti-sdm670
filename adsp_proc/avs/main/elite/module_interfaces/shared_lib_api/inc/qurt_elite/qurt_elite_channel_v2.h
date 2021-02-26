/*========================================================================
Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================= */
/**
@file qurt_elite_channel_v2.h
@brief This file contains a utility to form a channel of a combination
of up to 32 signals, queues, and timers. The client can wait on
any combination of signals, queues, and timers and be awakened
when any desired element is active.

v2 is implemented to make qurt_elite independent of qurt.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      The weakgroup description below displays in the PDF.
===========================================================================*/
/** @weakgroup weakf_qurt_elite_channel_utilsv2_intro
Version 2 of the channel utilities decouples QuRT_Elite from the underlying
QuRT functions, and thus provides better portability. Inline function and
inline structure definitions are removed. New functions are added to extend
version 1 of qurt_elite_channel.

Only the version 2 functions are documented in this section. The version 1
functions are documented in Section @xref{dox:ChannelUtilities}; they are not
repeated here.
 - qurt_elite_channel_create() -- Version 2
 - qurt_elite_channel_delete() -- Version 2
 - qurt_elite_channel_addq()
 - qurt_elite_channel_add_signal()
 - qurt_elite_channel_wait()
 - qurt_elite_channel_poll()
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/qurt_elite/qurt_elite_channel_v2.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/16/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
08/24/15   sw      (Tech Pubs) Edited Doxygen comments for 8996.
07/17/15   mh      Removing channel group support
05/11/15   sw       (Tech Pubs) Merged Doxygen comments from 2.6; edited for 8952.
10/30/14   rbhatnk	added file.
========================================================================== */

#ifndef QURT_ELITE_CHANNEL_V2_H
#define QURT_ELITE_CHANNEL_V2_H

#include "qurt_elite_types.h"
#include "adsp_error_codes.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/** @addtogroup qurt_elite_channel2
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */
typedef struct qurt_elite_queue_t qurt_elite_queue_t;

/* Forward declaration*/
typedef struct qurt_elite_channel_t qurt_elite_channel_t;

/* Forward declaration*/
typedef struct qurt_elite_signal_t qurt_elite_signal_t;

/** Mask of all channel bits. */
#define QURT_ELITE_CHANNEL_ALL_BITS 0xFFFFFFFFL


/****************************************************************************
** Channels
*****************************************************************************/

/**
  Creates and initializes a channel.

  @datatypes
  qurt_elite_channel_t

  @param[in] pChannel  Double pointer to the channel.

  @detdesc
  There can be only one receiver thread in this channel implementation. A
  channel is a combination of up to 32 signals, queues, and timers.
  @par
  Any mask of the associated elements can be set up to block until one of the
  nonmasked queues receives an element. Typically, the receiver blocks on a
  channel, checks which queues have data, and pops the data off at least one
  queue.
  @par
  @note1hang Channel operations are not thread-protected. Only the
             owner-receiver thread is to touch them. @newpage

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  None.
*/
ADSPResult qurt_elite_channel_create(qurt_elite_channel_t** pChannel);

/**
  Deletes a channel. This function must be called to clean up the
  resources from a corresponding call to qurt_elite_channel_create().

  @datatypes
  qurt_elite_channel_t

  @param[in] pChannel  Pointer to the channel.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
void qurt_elite_channel_delete(qurt_elite_channel_t* pChannel);

/** @} */ /* end_addtogroup qurt_elite_channel2 */


/* NOTE - The following v1 functions are not repeated in the PDF. */
/*
  Adds a queue to a channel. Each queue can belong to only one channel.

  @datatypes
  qurt_elite_channel_t \n
  qurt_elite_queue_t

  @param[in] pChannel   Pointer to the channel.
  @param[in] pQ         Pointer to the queue.
  @param[in] unBitMask  Indicates the bit used to add the queue.

  @return
  Handle to the channel containing the queue.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
ADSPResult qurt_elite_channel_addq(qurt_elite_channel_t* pChannel, qurt_elite_queue_t* pQ, uint32_t unBitMask);

/*
  Adds a signal to a channel.

  @datatypes
  qurt_elite_channel_t \n
  qurt_elite_signal_t

  @param[in] pChannel   Pointer to the channel.
  @param[in] pSignal    Pointer to the signal.
  @param[in] unBitMask  Indicates the bit used to add the signal.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
ADSPResult qurt_elite_channel_add_signal(qurt_elite_channel_t* pChannel, qurt_elite_signal_t* pSignal, uint32_t unBitMask);

/*
  Waits for a combination of queues, signals, and timers in a channel to
  receive an item.

  @note1hang Only one thread can wait on a channel at a time.

  @datatypes
  qurt_elite_channel_t

  @param[in] pChannel          Pointer to the channel.
  @param[in] unEnableBitfield  Mask that indicates the queues and signals.

  @return
  A bitfield indicating which queues have items.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
uint32_t qurt_elite_channel_wait(qurt_elite_channel_t* pChannel, uint32_t unEnableBitfield);

/*
  Checks which channel queues and signals received active triggers.

  @datatypes
  qurt_elite_channel_t

  @param[in] pChannel          Pointer to the channel.
  @param[in] unEnableBitfield  Mask that indicates the queues and signals.

  @detdesc
  A bit setting of 1 indicates that there is something to receive. This
  operation is like a poll for a specific set of bits in cases where the
  blocking from qurt_elite_channel_wait() is not wanted.

  @return
  A 32-bit field indicating which of the queues and signals received
  active triggers.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
uint32_t qurt_elite_channel_poll(qurt_elite_channel_t* pChannel, uint32_t unEnableBitfield);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_CHANNEL_V2_H

