/*========================================================================
Copyright (c) 2011,2013, 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================= */
/**
@file qurt_elite_condvar.h

@brief This file contains the ConditionVariables utilities.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The weakgroup description below displays in the PDF.
===========================================================================*/

/** @weakgroup weakf_qurt_elite_cond_variables_intro
This section describes the following utilities:
 - qurt_elite_condvar_init()
 - qurt_elite_condvar_signal()
 - qurt_elite_condvar_broadcast()
 - qurt_elite_condvar_wait()
 - qurt_elite_condvar_destroy()
*/

/*========================================================================
Edit History

$Header: //source/qcom/qct/multimedia2/Audio/elite/common/qurt_elite/main/latest/inc/qurt_elite_condvar.h#1

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/24/15   sw      (Tech Pubs) Edited Doxygen comments for 8996.
07/10/15   mh      Updated dependencies info for conditional variable wait
                   API
05/11/15   sw      (Tech Pubs) Edited Doxygen comments for 8952
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo     (Tech Pubs) Edited doxygen comments and markup.
02/03/11   sudhir     Created file.

========================================================================== */

#ifndef QURT_ELITE_CONDITIONVARIABLE_H
#define QURT_ELITE_CONDITIONVARIABLE_H

#include "qurt.h"
#include "qurt_elite_nmutex.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/** @addtogroup qurt_elite_condvar
@{ */

/****************************************************************************
** ConditionVariables
*****************************************************************************/

/** QuRT_Elite condition variable structure.

    @note1hang This structure is type-casted to obtain qurt_cond_t.
               Use caution when adding new elements.
*/
typedef struct
{
    qurt_cond_t condvar;  /**< QuRT condition variable. */
} qurt_elite_condvar_t;

/**
  Initializes a QuRT_Elite condition variable object.

  @datatypes
  #qurt_elite_condvar_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.

  @return
  None.

  @dependencies
  None. @newpage
 */
void qurt_elite_condvar_init(qurt_elite_condvar_t *condition_var_ptr);


/**
  Signals a condition variable object. This utility is used to awaken a single
  waiting thread.

  @datatypes
  #qurt_elite_condvar_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
 */
void qurt_elite_condvar_signal(qurt_elite_condvar_t *condition_var_ptr);

/**
  Broadcasts a condition variable object. This utility is used to awaken
  multiple threads waiting for a condition variable.

  @datatypes
  #qurt_elite_condvar_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
 */
void qurt_elite_condvar_broadcast(qurt_elite_condvar_t *condition_var_ptr);

/**
  Waits for a condition variable object. This utility suspends the current
  thread until the specified condition is true.

  @datatypes
  #qurt_elite_condvar_t \n
  #qurt_elite_nmutex_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.
  @param[in] nmutex             Normal mutex associated with the condition
                                variable.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
 */
void qurt_elite_condvar_wait(qurt_elite_condvar_t *condition_var_ptr, qurt_elite_nmutex_t *nmutex);

/**
  Destroys a condition variable object.

  @datatypes
  #qurt_elite_condvar_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.

  @return
  None.

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
*/
void qurt_elite_condvar_destroy(qurt_elite_condvar_t *condition_var_ptr);

/** @} */ /* end_addtogroup qurt_elite_condvar */


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_CONDITIONVARIABLE_H

