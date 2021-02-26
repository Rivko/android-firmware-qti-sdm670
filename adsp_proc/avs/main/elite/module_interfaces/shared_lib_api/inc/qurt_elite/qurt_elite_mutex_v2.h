/*========================================================================
Copyright (c) 2014-2016 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================== */
/**
@file qurt_elite_mutex_v2.h

@brief This file contains mutex utilites. Recursive mutexes are always used
for thread-safe programming.

v2 is implemented to make qurt_elite independent of qurt.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The weakgroup description below displays in the PDF.
===========================================================================*/

/** @weakgroup weakf_qurt_elite_mutexv2_intro
Version 2 of the mutex utilities decouples QuRT_Elite from the underlying QuRT
functions, and thus provides better portability. Inline function and inline
structure definitions are removed. New functions are added to extend version 1
of qurt_elite_mutex.

Only the version 2 functions are documented in this section. The version 1
functions are documented in Section @xref{dox:MutexUtilities}; they are not
repeated here.
 - qurt_elite_mutex_create() -- Version 2
 - qurt_elite_mutex_delete() -- Version 2
 - qurt_elite_mutex_lock()
 - qurt_elite_mutex_try_lock()
 - qurt_elite_mutex_unlock()
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/qurt_elite/qurt_elite_mutex_v2.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/16/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
01/08/16   sw       (Tech Pubs) General edits in Doxygen comments.
05/11/15   sw      (Tech Pubs) Merged Doxygen comments from 2.6; edited for 8952.
10/30/14   rbhatnk      Created file.
========================================================================== */

#ifndef QURT_ELITE_MUTEX_V2_H
#define QURT_ELITE_MUTEX_V2_H

#include "adsp_error_codes.h"
#include "qurt_elite_types.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup qurt_elite_mutex2
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

typedef qurt_elite_mutex_t qurt_elite_mutex_t;

/****************************************************************************
** Mutexes
*****************************************************************************/

/**
  Creates and initializes a mutex. Recursive mutexes are always used.

  @datatypes
  qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Double pointer to the mutex.

  @return
  0 -- Success
  @par
  Nonzero -- Failure (for information on ADSPResult error codes, refer to
  @xrefcond{Q1,80-NF774-4,80-NA610-4})

  @dependencies
  None. @newpage
*/
ADSPResult qurt_elite_mutex_create(qurt_elite_mutex_t **pqurt_elite_mutex);

/**
  Deletes a mutex. This function must be called for each corresponding
  qurt_elite_mutex_create() function to clean up all resources.

  @datatypes
  qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
*/
void qurt_elite_mutex_delete(qurt_elite_mutex_t *pqurt_elite_mutex);

/** @} */ /* end_addtogroup qurt_elite_mutex2 */


/* NOTE - The following v1 functions are not repeated in the PDF */
/*
  Locks a mutex. Recursive mutexes are always used.

  @datatypes
  qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
*/
void qurt_elite_mutex_lock(qurt_elite_mutex_t *pqurt_elite_mutex);

/*
  Attempts to lock a mutex. If the mutex is already locked and unavailable,
  a failure is returned.

  @datatypes
  qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  Before calling this function, the object must be created and initialized.
   @newpage
*/
int qurt_elite_mutex_try_lock(qurt_elite_mutex_t *pqurt_elite_mutex);

/*
  Unlocks a mutex. Recursive mutexes are always used.

  @datatypes
  qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
void qurt_elite_mutex_unlock(qurt_elite_mutex_t *pqurt_elite_mutex);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_MUTEX_V2_H

