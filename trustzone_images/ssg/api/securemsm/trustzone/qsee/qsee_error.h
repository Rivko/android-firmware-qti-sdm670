#ifndef QSEE_ERROR_H
#define QSEE_ERROR_H

/*===========================================================================
  Copyright (c) 2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

#define MINK_SUCCESS          0
#define MINK_GENERIC_ERROR   -1

/**
 * @brief API for synchronously signalling an application fatal error.
 *
 * Invoking this interface will result in the termination of the calling TA.
 */
void qsee_err_fatal(void);

#endif /* QSEE_ERROR_H */
