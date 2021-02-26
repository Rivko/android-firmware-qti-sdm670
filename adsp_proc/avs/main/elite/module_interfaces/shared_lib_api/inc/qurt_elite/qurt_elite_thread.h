/*========================================================================
Copyright (c) 2010-2011, 2013-2014, 2016 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================== */
/**
@file qurt_elite_thread.h

@brief This file contains utilities for threads. Threads must be joined to
avoid memory leaks. This file provides functions to create and destroy threads,
and to change thread priorities.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The weakgroup description below displays in the PDF.
===========================================================================*/

/** @weakgroup weakf_qurt_elite_thread_intro
Threads must be joined to avoid memory leaks. The following thread functions
are used to create and destroy threads, and to change thread priorities.
 - qurt_elite_thread_launch()
 - qurt_elite_thread_launch_lowpower()
 - qurt_elite_thread_join()
 - qurt_elite_thread_list_init()
 - qurt_elite_thread_prio_get()
 - qurt_elite_thread_set_prio()
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/qurt_elite/qurt_elite_thread.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/16/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
01/08/16    sw      (Tech Pubs) General edits in Doxygen comments.
09/08/14    sw      (Tech Pubs) Merged Doxygen comments from 2.4; edited
                     Doxygen comments/markup for 2.6.
03/25/13    sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11    leo     (Tech Pubs) Edited doxygen comments and markup.
03/08/10    brs      Edited for doxygen-to-latex process.
02/04/10    mwc      Created file.
========================================================================== */

#ifndef QURT_ELITE_THREAD_H
#define QURT_ELITE_THREAD_H

#include "qurt_elite_memory.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

/** @addtogroup qurt_elite_thread
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

//Support Q6v3 (variable HW threads) & Q6v4, Q6v5 (3 HW threads) & Q6v55, Q6v56 (4 HW threads). 
//For others issue error until supported.
//TBD: Hard-coding because 8994 build is still using v5 but need to use v55.
//Need to be modified once build migrates to v55.

/** Maximum number of hardware threads available on the platform. */
#define QURT_ELITE_MAX_HW_THREADS       4 

/** Maximum number of software threads available on the platform. */
#define QURT_ELITE_MAX_SW_THREADS       MAX_THREADS

/** Generic thread mask. */
#define QURT_ELITE_GENERIC_THREAD_MASK  0xFF

/** Thread stack fill word. */
#define QURT_ELITE_STACK_FILL_WORD             0xF8F8F8F8L

/** Thread stack fill spacing. */
#define QURT_ELITE_STACK_FILL_SPACING          16

/** Handle to a thread. */
typedef int qurt_elite_thread_t;

/** Valid low-power optimization feature flags. */
typedef enum
{
   QURT_ELITE_LP_TCB_OPT=1,  /**< Thread context buffer optimization. @newpage */
}  QURT_ELITE_LP_OPT_FLAGS;


/****************************************************************************
** Threads
*****************************************************************************/

/**
  Creates and launches a thread.

  @datatypes
  #qurt_elite_thread_t \n
  #QURT_ELITE_HEAP_ID

  @param[out] pTid           Pointer to the thread ID.
  @param[in]  pzThreadName   Pointer to the thread name.
  @param[in]  pStack         Pointer to the location of the pre-allocated stack
                             (NULL causes a new allocation). \n
                             pStack must point to the lowest address in the
                             stack.
  @param[in]  nStackSize     Size of the thread stack.
  @param[in]  nPriority      Thread priority, where 0 is the lowest priority
                             and 255 is the highest priority.
  @param[in]  pfStartRoutine Pointer to the entry function of the thread.
  @param[in]  arg            Pointer to the arguments passed to the entry
                             function. An argument can be to any pointer type.
  @param[in]  heap_id        ID of the heap to which the thread stack is
                             allocated.

  @detdesc
  The thread stack can be passed in as the pStack argument, or pStack=NULL
  indicates that QuRT_Elite allocates the stack internally. If the caller
  provides the stack, the caller is responsible for freeing the stack memory
  after joining the thread.
  @par
  Pre-allocated stacks must be freed after the dying thread is joined. The
  caller must specify the heap in which the thread stack is to be allocated.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  None. @newpage
*/
ADSPResult qurt_elite_thread_launch(qurt_elite_thread_t *pTid, char *pzThreadName, char* pStack,
   size_t nStackSize, int nPriority, int (*pfStartRoutine)(void *), void* arg, QURT_ELITE_HEAP_ID heap_id);

/**
  Creates and launches a thread for Low Power Audio (LPA).

  @datatypes
  #qurt_elite_thread_t \n
  #QURT_ELITE_HEAP_ID

  @param[out] pTid           Thread ID.
  @param[in] pzThreadName    Thread name.
  @param[in]  pStack         Location of a pre-allocated stack (NULL causes a
                             new allocation). \n
                             pStack must point to the lowest address in the
                             stack.
  @param[in] nStackSize Size (in bytes) of the thread stack.
  @param[in]  nPriority      Thread priority, where 0 is the lowest priority
                             and 255 is the highest priority.
  @param[in] pfStartRoutine  Entry function of the thread.
  @param[in]  arg            Passed to the entry function and can be a case to
                             any pointer type.
  @param[in]  heap_id        Specifies the thread stack allocated in the heap.
                             If the specified heap is full, the thread stack
                             is allocated in the default heap.
  @param[in]  lp_opt_flags   Flags that indicate the power optimization
                             features. Each bit represents different
                             optimization.
                             - Bit 0 = 1 -- Thread TCB must be created in the
                               small TCB buffer.
                             - Bit 0 = 0 -- Thread TCB must be created in the
                               normal TCB buffer.
                             - Bits 1 to 31 -- Reserved. @tablebulletend

  @detdesc
  If no specific heap ID is required, all services can use this function by
  passing #QURT_ELITE_HEAP_DEFAULT as the heap_id.
  @par
  The TCB is allocated in the small TCB buffer (which can be OCMEM or LPI). If
  the small TCB buffer is full, the TCB is allocated in the normal TCB buffer.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  None.
*/

ADSPResult qurt_elite_thread_launch_lowpower(qurt_elite_thread_t *pTid, char *pzThreadName, char* pStack,
   size_t nStackSize, int nPriority, int (*pfStartRoutine)(void *), void* arg, QURT_ELITE_HEAP_ID heap_id,
   uint32_t lp_opt_flags);

/**
  Waits for a specified thread to exit, and collects the exit status.

  @datatypes
  #qurt_elite_thread_t

  @param[in]  nTid     Thread ID to wait on.
  @param[out] nStatus  Pointer to the value returned by pfStartRoutine
                       called in qurt_elite_thread_launch().

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
void qurt_elite_thread_join(qurt_elite_thread_t nTid, int* nStatus);

/**
  Initializes the thread context structure.

  @return
  None.

  @dependencies
  None.
*/
void qurt_elite_thread_list_init(void);


/**
  Queries the thread priority of the caller.

  @return
  The thread priority of the caller, where 0 is the lowest priority and 255 is
  the highest priority.

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
*/
int qurt_elite_thread_prio_get(void);

/**
  Changes the thread priority of the caller.

  @param[in] nPrio  New priority, where 0 is the lowest priority and 255 is the
                    highest priority.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
void qurt_elite_thread_set_prio(int nPrio);

/** @} */ /* end_addtogroup qurt_elite_thread */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_THREAD_H

