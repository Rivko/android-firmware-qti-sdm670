#ifndef QURT_ASSERT_H
#define QURT_ASSERT_H
/**
  @file qurt_assert.h   
  @brief  Prototypes of qurt_assert API  

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2014  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/



/*=============================================================================
                        CONSTANTS AND MACROS
=============================================================================*/
/** @ingroup func_qurt_assert
@{ */

void qurt_assert_error(const char *filename, int lineno) __attribute__((noreturn));

#define qurt_assert(cond) ((cond)?(void)0:qurt_assert_error(__QURTFILENAME__,__LINE__))

/** @} */ /* end_ingroup func_qurt_assert */

#endif /* QURT_ASSERT_H */

