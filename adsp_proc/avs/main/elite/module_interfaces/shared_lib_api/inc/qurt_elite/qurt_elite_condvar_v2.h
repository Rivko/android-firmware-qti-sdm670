/*========================================================================
Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================= */
/**
@file qurt_elite_condvar_v2.h

@brief This file contains the ConditionVariables utilities.

v2 is implemented to make qurt_elite independent of qurt.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The weakgroup description below displays in the PDF.
===========================================================================*/

/** @weakgroup weakf_qurt_elite_cond_variablesv2_intro
Version 2 of the condition variable utilities decouples QuRT_Elite from the
underlying QuRT functions, and thus provides better portability. Inline
function and inline structure definitions are removed. New functions are added
to extend version 1 of qurt_elite_condvar.

Only the version 2 functions are documented in this section. The version 1
functions are documented in Section @xref{dox:CondvarUtilities}; they are not
repeated here.
 - qurt_elite_condvar_create() -- Version 2
 - qurt_elite_condvar_delete() -- Version 2
 - qurt_elite_condvar_signal()
 - qurt_elite_condvar_broadcast()
 - qurt_elite_condvar_wait()
*/

/*========================================================================
Edit History

$Header: //source/qcom/qct/multimedia2/Audio/elite/common/qurt_elite/main/latest/inc/qurt_elite_condvar.h#1

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/24/15   sw      (Tech Pubs) Edited Doxyen comments for 8996.
07/10/15   mh      Updated dependencies info for conditional variable wait
                   API
05/11/15   sw      (Tech Pubs) Merged some Doxyen comments from 2.6; edited for 8952.
10/30/14   rbhatnk created file.

========================================================================== */

#ifndef QURT_ELITE_CONDITIONVARIABLE_V2_H
#define QURT_ELITE_CONDITIONVARIABLE_V2_H

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

/** @addtogroup qurt_elite_condvar2
@{ */

/****************************************************************************
** ConditionVariables
*****************************************************************************/

/* Condition variable. */
typedef qurt_elite_condvar_t qurt_elite_condvar_t;

/**
  Creates and initializes a QuRT_Elite condition variable object.

  @datatypes
  qurt_elite_condvar_t

  @param[in] condition_var_ptr_ptr  Double pointer to the QuRT_Elite condition
                                    variable.

  @return
  None.

  @dependencies
  None. @newpage
 */
ADSPResult qurt_elite_condvar_create(qurt_elite_condvar_t **condition_var_ptr_ptr);

/**
  Deletes a condition variable object.

  @datatypes
  qurt_elite_condvar_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
   @newpage
*/
void qurt_elite_condvar_delete(qurt_elite_condvar_t *condition_var_ptr);

/** @} */ /* end_addtogroup qurt_elite_condvar2 */


/* NOTE - The following v1 functions are not repeated in the PDF */
/*
  Signals a condition variable object. This utility is used to awaken a single
  waiting thread.

  @datatypes
  qurt_elite_condvar_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
 */
void qurt_elite_condvar_signal(qurt_elite_condvar_t *condition_var_ptr);

/*
  Broadcasts a condition variable object. This utility is used to awaken
  multiple threads waiting for a condition variable.

  @datatypes
  qurt_elite_condvar_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
   @newpage
 */
void qurt_elite_condvar_broadcast(qurt_elite_condvar_t *condition_var_ptr);

/*
  Waits for a condition variable object. This utility suspends the current
  thread until the specified condition is true.

  @datatypes
  qurt_elite_condvar_t \n
  qurt_elite_nmutex_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.
  @param[in] nmutex             Pointer to the normal mutex associated with the
                                condition variable.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
 */
void qurt_elite_condvar_wait(qurt_elite_condvar_t *condition_var_ptr, qurt_elite_nmutex_t *nmutex);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_CONDITIONVARIABLE_V2_H

