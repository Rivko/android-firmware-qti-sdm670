/*========================================================================
Copyright (c) 2010-2011, 2013-2015 Qualcomm Technologies, Inc. All Rights
Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================= */
/**
@file qurt_elite_signal.h

@brief This file contains signal utilities.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The weakgroup description below displays in the PDF.
===========================================================================*/

/** @weakgroup weakf_qurt_elite_signal_intro
This section describes the following signal functions:
 - qurt_elite_signal_create()
 - qurt_elite_signal_destroy()
 - qurt_elite_signal_init()
 - qurt_elite_signal_deinit()
 - qurt_elite_signal_get_channel()
 - qurt_elite_signal_get_channel_bit()
 - qurt_elite_signal_send()
 - qurt_elite_signal_clear()
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/inc/qurt_elite_signal.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/16/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
08/24/15   sw      (Tech Pubs) Edited Doxygen comments for 8996.
05/11/15   sw      (Tech Pubs) Edited Doxygen hyperlinks for 8952.
09/08/14   sw      (Tech Pubs) Merged Doxygen comments from 2.4.
07/17/15   mh      Removing channel group support
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo     (Tech Pubs) Edited doxygen comments and markup.
03/08/10   brs     Edited for doxygen-to-latex process.
02/04/10   mwc     Created file.
========================================================================== */

#ifndef QURT_ELITE_SIGNAL_H
#define QURT_ELITE_SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

/** @addtogroup qurt_elite_signal
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/****************************************************************************
** Signals
*****************************************************************************/

/** Signal to be triggered by events, or used to trigger events.

  The signal coalesces on a channel bit. The only way to receive a signal is
  through its associated channel.
 */
struct qurt_elite_signal_t
{
   qurt_elite_channel_t         *pChannel;
   /**< Pointer to the associated channel. */

   uint32_t                    unMyChannelBit;
   /**< Channel bitfield of this signal. @newpagetable */
};

/**
  Creates a signal.

  @note1hang This function will be deprecated in a future release, at which
             time qurt_elite_signal_init() must be used instead.

  @datatypes
  qurt_elite_signal_t

  @param[out] ppSignal  Double pointer to the signal.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  None.
*/
ADSPResult qurt_elite_signal_create(qurt_elite_signal_t** ppSignal);

/**
  Destroys a signal

  @note1hang This function will be deprecated in a future release, at which
             time qurt_elite_signal_deinit() must be used instead.

  @datatypes
  qurt_elite_signal_t

  @param[in] pSignal  Pointer to the signal.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
*/
void qurt_elite_signal_destroy(qurt_elite_signal_t* pSignal);


/**
  Initializes a signal. The client must allocate the memory for the signal and
  pass the pointer to this function.

  @datatypes
  qurt_elite_signal_t

  @param[out] pSignal  Pointer to the signal.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  None.
*/
ADSPResult qurt_elite_signal_init(qurt_elite_signal_t* pSignal);

/**
  De-initialize a signal.

  @datatypes
  qurt_elite_signal_t

  @param[in] pSignal  Pointer to the signal.

  @return
  None.

  @dependencies
  Before calling this function, the signal group must be created and
  initialized. @newpage
*/
void qurt_elite_signal_deinit(qurt_elite_signal_t* pSignal);

/**
  Queries a signal for its channel.

  @datatypes
  qurt_elite_signal_t

  @param[in] pSignal  Pointer to the signal.

  @return
  A handle to the channel containing the signal.

  @dependencies
  Before calling this function, the signal group must be created and
  initialized.
*/
qurt_elite_channel_t* qurt_elite_signal_get_channel(qurt_elite_signal_t* pSignal);

/**
  Queries a signal for its channel bitfield.

  @datatypes
  qurt_elite_signal_t

  @param[in] pSignal  Pointer to the signal to query.

  @detdesc
  Use this function to find the values to be ORed together to form an enable
  bitfield for a combination of signals in a channel.

  @return
  A 32-bit value with a single bit=1.

  @dependencies
  Before calling this function, the signal group must be created and
  initialized. @newpage
*/
uint32_t qurt_elite_signal_get_channel_bit(qurt_elite_signal_t* pSignal);

/**
  Sends a signal.

  @datatypes
  qurt_elite_signal_t

  @param[in] pSignal  Pointer to the signal.

  @return
  None.

  @dependencies
  Before calling this function, the signal group must be created and
  initialized.
*/
void qurt_elite_signal_send(qurt_elite_signal_t* pSignal);

/**
  Clears a signal that is active in a channel.

  @datatypes
  qurt_elite_signal_t

  @param[in] pSignal   Pointer to the signal.

  @return
  None.

  @dependencies
  Before calling this function, the signal group must be created and
  initialized.
*/
void qurt_elite_signal_clear(qurt_elite_signal_t* pSignal);

/** @} */ /* end_addtogroup qurt_elite_signal */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_SIGNAL_H

