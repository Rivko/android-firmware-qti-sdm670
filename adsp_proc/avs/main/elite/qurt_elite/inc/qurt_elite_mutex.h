/*========================================================================
Copyright (c) 2010-2011, 2013-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================== */
/**
@file qurt_elite_mutex.h

@brief This file contains mutex utilites. Recursive mutexes are always used
for thread-safe programming.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The weakgroup description below displays in the PDF.
===========================================================================*/

/** @weakgroup weakf_qurt_elite_mutex_intro
This section describes the following priority inheritance mutex functions.
Recursive mutexes are always used for thread-safe programming.
 - qurt_elite_mutex_init()
 - qurt_elite_mutex_lock()
 - qurt_elite_mutex_try_lock()
 - qurt_elite_mutex_unlock()
 - qurt_elite_mutex_destroy()
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/inc/qurt_elite_mutex.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
01/08/16   sw      (Tech Pubs) General edits in Doxygen comments.
05/11/15   sw      (Tech Pubs) Edited Doxygen comments for 8952
09/05/14   sw      (Tech Pubs) Merged Doxygen comments from 2.4.
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo     (Tech Pubs) Edited doxygen comments and markup.
03/08/10   brs      Edited for doxygen-to-latex process.
02/04/10   mwc      Created file.
========================================================================== */

#ifndef QURT_ELITE_MUTEX_H
#define QURT_ELITE_MUTEX_H

#include "qurt.h"
#include "qurt_pimutex.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup qurt_elite_mutex
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/** QuRT_Elite mutex type structure.

    @note1hang This structure is type-casted to obtain qurt_mutex_t.
    Use caution when adding new elements.
*/
typedef struct
{
    qurt_mutex_t mutex;    /**< QuRT mutex type. */
} qurt_elite_mutex_t;

/****************************************************************************
** Mutexes
*****************************************************************************/

/**
  Initializes a mutex. Recursive mutexes are always used.

  @datatypes
  #qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex.

  @return
  None.

  @dependencies
  None. @newpage
*/
void qurt_elite_mutex_init(qurt_elite_mutex_t *pqurt_elite_mutex);

/**
  Locks a mutex. Recursive mutexes are always used.

  @datatypes
  #qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
void qurt_elite_mutex_lock(qurt_elite_mutex_t *pqurt_elite_mutex);

/**
  Attempts to lock a mutex. If the mutex is already locked and unavailable,
  a failure is returned.

  @datatypes
  #qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
*/
int qurt_elite_mutex_try_lock(qurt_elite_mutex_t *pqurt_elite_mutex);

/**
  Unlocks a mutex. Recursive mutexes are always used.

  @datatypes
  #qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
void qurt_elite_mutex_unlock(qurt_elite_mutex_t *pqurt_elite_mutex);

/**
  Destroys a mutex. This function must be called for each corresponding
  qurt_elite_mutex_init() function to clean up all resources.

  @datatypes
  #qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
void qurt_elite_mutex_destroy(qurt_elite_mutex_t *pqurt_elite_mutex);

/** @} */ /* end_addtogroup qurt_elite_mutex */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_MUTEX_H

