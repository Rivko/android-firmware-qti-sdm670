/*========================================================================
Copyright (c) 2010-2011, 2013, 2015 Qualcomm Technologies, Inc. All Rights
Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================= */
/**
@file qurt_elite_signal_v2.h

@brief This file contains signal utilities.

v2 is implemented to make qurt_elite independent of qurt.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The weakgroup description below displays in the PDF.
===========================================================================*/

/** @weakgroup weakf_qurt_elite_signalv2_intro
Version 2 of the signal utilities decouples QuRT_Elite from the underlying QuRT
functions, and thus provides better portability. Inline function and inline
structure definitions are removed. New functions are added to extend version 1
of qurt_elite_signal.

Only the version 2 functions are documented in this section. The version 1
functions are documented in Section @xref{dox:SignalUtilities}; they are not
repeated here.
 - qurt_elite_signal_create()
 - qurt_elite_signal_delete() -- Version 2
 - qurt_elite_signal_get_channel()
 - qurt_elite_signal_get_channel_bit()
 - qurt_elite_signal_send()
 - qurt_elite_signal_clear()
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/qurt_elite/qurt_elite_signal_v2.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/16/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
08/24/15   sw      (Tech Pubs) Edited Doxygen comments for 8996.
07/17/15   mh      Removing channel group support
05/11/15   sw      (Tech Pubs) Merged Doxygen comments from 2.6; edited for 8952.
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo     (Tech Pubs) Edited doxygen comments and markup.
03/08/10   brs     Edited for doxygen-to-latex process.
02/04/10   mwc     Created file.
========================================================================== */

#ifndef QURT_ELITE_SIGNAL_V2_H
#define QURT_ELITE_SIGNAL_V2_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */


/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/****************************************************************************
** Signals
*****************************************************************************/

/* V1 function not repeated in the PDF. */
/*
  Creates a signal.

  @datatypes
  qurt_elite_signal_t

  @param[out] ppSignal  Double pointer to the signal.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  None. @newpage
*/
ADSPResult qurt_elite_signal_create(qurt_elite_signal_t** ppSignal);


/** @ingroup qurt_elite_signal2
  Deletes a signal.

  @datatypes
  qurt_elite_signal_t

  @param[in] pSignal  Pointer to the signal.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
   @newpage
*/
void qurt_elite_signal_delete(qurt_elite_signal_t* pSignal);


/* NOTE - Following v1 functions are not repeated in the PDF. */
/*
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

/*
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

/*
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

/*
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


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_SIGNAL_V2_H

