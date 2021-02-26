/*========================================================================
Copyright (c) 2010-2011, 2013-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================= */
/**
@file qurt_elite_interrupt.h

@brief This file contains utilities for registering with interrupts.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The weakgroup description below displays in the PDF.
===========================================================================*/

/** @weakgroup weakf_qurt_elite_interrupt_intro
This section describes the qurt_elite_interrupt_register() and
qurt_elite_interrupt_unregister() utilities.
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/inc/qurt_elite_interrupt.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/17/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
01/08/16   sw      (Tech Pubs) General edits in Doxygen comments.
05/11/15   sw      (Tech Pubs) Updated Doxygen hyperlinks for 8952.
09/05/14   sw      (Tech Pubs) Merged Doxygen comments from 2.4.
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo     (Tech Pubs) Edited doxygen comments and markup.
11/04/10   psr     Created file.

========================================================================== */

#ifndef QURT_ELITE_INTERRUPT_H
#define QURT_ELITE_INTERRUPT_H

#include "qurt_elite.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/** @addtogroup qurt_elite_interrupt
@{ */

/** Stack size of the interrupt service thread (IST). */
#define QURT_ELITE_IST_STACK_SIZE 512

/*forward declaration*/
typedef struct qurt_elite_interrupt_ist_t qurt_elite_interrupt_ist_t;

/** Structure that contains information for registering an interrupt.

  The client must allocate the memory for this structure before calling
  qurt_elite_interrupt_register(). The client is not required to initialize any
  structure variables.
*/
struct qurt_elite_interrupt_ist_t
{
 char_t ist_thread_name[16];
 /**< IST thread name. */

 uint16_t intr_id;
 /**< Interrupt to register. */

 qurt_elite_thread_t thread_id;
 /**< Used to join the IST thread. */

 uint32_t ist_state;
 /**< Checks whether the IST registered the interrupt successfully. */

 void (*ist_callback)(void *);
 /**< Pointer to the thread entry function. */

 qurt_sem_t semaphore;
 /**< Semaphore used to synchronize a caller and an IST. */

 void *arg_ptr;
 /**< Pointer to the thread arguments sent by the client. @newpagetable */
 };

/**
  Registers an interrupt. The client must allocate the memory for the
  qurt_elite_interrupt_ist_t structure.

  @datatypes
  qurt_elite_interrupt_ist_t

  @param[in] ist_ptr          Pointer to the IST.
  @param[in] intr_id          Interrupt number to register.
  @param[in] callback_ptr     Pointer to the callback function when an
                              interrupt occurs.
  @param[in] arg_ptr          Pointer to the arguments sent by the client.
  @param[in] thread_name      Pointer to the IST thread name.
  @param[in] stack_size       Size of the IST stack.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  None. @newpage
*/
ADSPResult qurt_elite_interrupt_register( qurt_elite_interrupt_ist_t *ist_ptr, uint16_t intr_id,
                              void (*callback_ptr)(void *),void *arg_ptr,char_t *thread_name, uint32_t stack_size);

/**
  Deregisters an interrupt.

  @datatypes
  qurt_elite_interrupt_ist_t

  @param[in] ist_ptr  Pointer to the IST.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  Before calling this function, the interrupt object must be registered.
*/
ADSPResult qurt_elite_interrupt_unregister(qurt_elite_interrupt_ist_t *ist_ptr);

/** @} */ /* end_addtogroup qurt_elite_interrupt */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_INTERRUPT_H
