/*========================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================== */
/**
@file qurt_elite_nmutex.h

@brief This file contains normal mutex utilities.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The weakgroup description below displays in the PDF.
===========================================================================*/

/** @weakgroup weakf_qurt_elite_nmutex_intro
This section describes the following regular/non-priority-inheriting mutex
functions:
 - qurt_elite_nmutex_init()
 - qurt_elite_nmutex_lock()
 - qurt_elite_nmutex_try_lock()
 - qurt_elite_nmutex_unlock()
 - qurt_elite_nmutex_destroy()
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/inc/qurt_elite_nmutex.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/24/15    sw     (Tech Pubs) Edited Doxygen comments for 8996.
07/08/15   mh      Created file.
========================================================================== */

#ifndef QURT_ELITE_NMUTEX_H
#define QURT_ELITE_NMUTEX_H

#include "qurt.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup qurt_elite_nmutex
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/** QuRT_Elite normal mutex type structure.

    @note1hang This structure is type-casted to obtain qurt_mutex_t.
               Use caution when adding new elements.
*/
typedef struct
{
    qurt_mutex_t mutex;    /**< QuRT normal mutex type. */
} qurt_elite_nmutex_t;

/****************************************************************************
** Normal Mutex APIs
*****************************************************************************/

/**
  Initializes a normal mutex.

  @datatypes
  #qurt_elite_nmutex_t

  @param[in] pqurt_elite_nmutex   Pointer to the normal mutex.

  @return
  None.

  @dependencies
  None. @newpage
*/
void qurt_elite_nmutex_init(qurt_elite_nmutex_t *pqurt_elite_nmutex);

/**
  Locks a normal mutex.

  @datatypes
  #qurt_elite_nmutex_t

  @param[in] pqurt_elite_nmutex   Pointer to the normal mutex.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
void qurt_elite_nmutex_lock(qurt_elite_nmutex_t *pqurt_elite_nmutex);

/**
  Attempts to lock a normal mutex. If the lock is already locked and
  unavailable, a failure is returned.

  @datatypes
  #qurt_elite_nmutex_t

  @param[in] pqurt_elite_nmutex   Pointer to the normal mutex.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
*/
int qurt_elite_nmutex_try_lock(qurt_elite_nmutex_t *pqurt_elite_nmutex);

/**
  Unlocks a normal mutex.

  @datatypes
  #qurt_elite_nmutex_t

  @param[in] pqurt_elite_nmutex   Pointer to the normal mutex.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
void qurt_elite_nmutex_unlock(qurt_elite_nmutex_t *pqurt_elite_nmutex);

/**
  Destroys a normal mutex. This function must be called for each corresponding
  qurt_elite_nmutex_init() function to clean up all resources.

  @datatypes
  #qurt_elite_nmutex_t

  @param[in] pqurt_elite_nmutex   Pointer to the normal mutex.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
*/
void qurt_elite_nmutex_destroy(qurt_elite_nmutex_t *pqurt_elite_nmutex);


/** @} */ /* end_addtogroup qurt_elite_nmutex */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_NMUTEX_H

