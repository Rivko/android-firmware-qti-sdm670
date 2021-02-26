/***********************************************************************
 * osal_qurt_uimage.c
 * Description: This file contains the implementation of the OS abstraction
 * layer for QURT which goes into uimage.
 * Copyright (C) 2017 QUALCOMM Technologies, Incorporated.
 *
 *
 ***********************************************************************/

/*===========================================================================

                         EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/services/osal/qurt/src/osal_qurt_uimage.c#1 $ $DateTime: 2017/10/12 04:14:53 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-10-04    is   Pulled in functions for uimage

===========================================================================*/

#include "osal_qurt_types.h"
#include "atomic_ops_plat.h"


/*===========================================================================
FUNCTION    OSAL_ATOMIC_COMPARE_AND_SET */

/*!
  @brief
    Compares the atomic word value to old_value and if it matches
    then sets new value.

  @param
    [in] target    Pointer to the atomic word.
    [in] old_val   old value to compare
    [in] new_val   new value to be set.

  @dependencies 
    None

  @return One of the following
  <ul>
  <li> FALSE
  <li> TRUE
  </ul>

  @sideeffects 
    None 
*/
/*===========================================================================*/
int osal_atomic_compare_and_set(osal_atomic_word_t *target,
                       osal_atomic_plain_word_t old_val,
                       osal_atomic_plain_word_t new_val )
{
   return atomic_compare_and_set(target,old_val,new_val);

}
