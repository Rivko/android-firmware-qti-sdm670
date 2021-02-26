/*========================================================================
Copyright (c) 2010-2011, 2013-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================= */
/**
@file qurt_elite_channel.h
@brief This file contains a utility to form a channel of a combination
of up to 32 signals, queues, and timers. The client can wait on
any combination of signals, queues, and timers and be awakened
when any desired element is active.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The weakgroup description below displays in the PDF.
===========================================================================*/

/** @weakgroup weakf_qurt_elite_channel_utils_intro
The following channel utilities form a channel with a combination of up to
32 signals, queues, and timers. The client can wait on any combination of
signals, queues, and timers and be awakened when any preferred element is
active.
 - qurt_elite_channel_init()
 - qurt_elite_channel_destroy()
 - qurt_elite_channel_addq()
 - qurt_elite_channel_add_signal()
 - qurt_elite_channel_wait()
 - qurt_elite_channel_poll()
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/inc/qurt_elite_channel.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/16/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
01/08/16   sw       (Tech Pubs) General edits in Doxygen comments.
07/17/15   mh       Removing channel group support
09/05/14   sw       (Tech Pubs) Merged Doxygen comments from 2.4.
03/25/13   sw       (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo      (Tech Pubs) Edited doxygen comments and markup.
02/04/10   mwc      Created file.
03/09/10   brs      Edited for doxygen-to-latex process.
========================================================================== */

#ifndef QURT_ELITE_CHANNEL_H
#define QURT_ELITE_CHANNEL_H


#include "qurt.h"
#include <stdio.h>
#include "qurt_elite_types.h"
#include "adsp_error_codes.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup qurt_elite_channel
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */
/* Forward declaration*/
typedef struct qurt_elite_channel_t qurt_elite_channel_t;

/* Forward declaration*/
typedef struct qurt_elite_signal_t qurt_elite_signal_t;

/** Mask of all channel bits. */
#define QURT_ELITE_CHANNEL_ALL_BITS 0xFFFFFFFFL

/****************************************************************************
** Channels
*****************************************************************************/

/** Structure that combines up to 32 queues and signals. Each queue and
    signal is assigned a bit in the 32-bit field.
*/
struct qurt_elite_channel_t
{
   qurt_signal2_t    anysig;
   /**< Any 32-bit signal channel. */

   uint32_t               unBitsUsedMask;
   /**< Mask bookkeeping for used bits.
   
        @values
         - 1 -- Used
         - 0 -- Available @tablebulletend @newpagetable */
};

/**
  Initializes a channel.

  @note1hang There can be only one receiver thread in this channel
             implementation.

  @datatypes
  qurt_elite_channel_t

  @param[in] pChannel  Pointer to the channel.

  @detdesc
  A channel is a combination of up to 32 signals, queues, and timers. Any mask
  of the associated elements can be set up to block until one of the nonmasked
  queues receives an element. Typically, the receiver blocks on a channel,
  checks which queue or queues have data, and pops the data off at least one
  of the queues.
  @par
  @note1hang Channel operations are not thread-protected. Only the
             owner-receiver thread is to touch them.

  @return
  None.

  @dependencies
  None. @newpage
*/
void qurt_elite_channel_init(qurt_elite_channel_t* pChannel);

/**
  Destroys a channel. This function must be called to clean up the
  resources from a corresponding call to qurt_elite_channel_init().

  @datatypes
  qurt_elite_channel_t

  @param[in] pChannel  Pointer to the channel.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
void qurt_elite_channel_destroy(qurt_elite_channel_t* pChannel);

/**
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
  @newpage
*/
ADSPResult qurt_elite_channel_addq(qurt_elite_channel_t* pChannel, qurt_elite_queue_t* pQ, uint32_t unBitMask);

/**
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

/**
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
  Before calling this function, the object must be created and initialized. @newpage
*/
uint32_t qurt_elite_channel_wait(qurt_elite_channel_t* pChannel, uint32_t unEnableBitfield);

/**
  Checks which channel queues and signals received active triggers.

  @datatypes
  qurt_elite_channel_t

  @param[in] pChannel          Pointer to the channel.
  @param[in] unEnableBitfield  Mask that indicates the queues and signals.

  @detdesc
  A bit setting of 1 indicates that there is something to receive. This
  operation is like a poll for a specific set of bits in cases where
  blocking from qurt_elite_channel_wait() is not wanted.

  @return
  A 32-bit field indicating which of the queues and signals received
  active triggers.

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
*/
uint32_t qurt_elite_channel_poll(qurt_elite_channel_t* pChannel, uint32_t unEnableBitfield);

/** @} */ /* end_addtogroup qurt_elite_channel */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_CHANNEL_H

