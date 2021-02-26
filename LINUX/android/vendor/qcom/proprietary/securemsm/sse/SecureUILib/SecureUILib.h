/** @file SecureUILib//SecureUILib.h
 * @brief
 * This file contains the interfaces to use the Secure UI Library
 */

/*===========================================================================
 * Copyright(c) 2013-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

#pragma once

#include <QSEEComAPI.h>

#ifdef __cplusplus
  extern "C" {
#endif

/** @addtogroup hlos_sec_ui_lib
@{ */
/** @xreflabel{hdr:getSharedMemorySize}
 * Gets the shared memory buffer size.
 *
 * @caution1hang This function is not thread safe.
 *
 * This function returns the minimum size required for the shared memory
 * buffer. The returned size is the one required by the HLOS Secure UI Library
 * only, therefore the user must set the shared buffer size to be at least
 * as large as the returned value (that is, it must be larger if required by
 * the specific application).
 *
 * @param[out] buffer_size  Set to hold the required size (in bytes).
 *
 * @return
 * 0 on success; otherwise, errno.
 *
 * @dependencies
 * None.
 */
int32_t GetSharedMemorySize(uint32_t* buffer_size);

/**
 * Gets the secure indicator.
 *
 * This function retrieves the secure indicator and saves it ready-to-use in
 * TrustZone so that it is available for any eligible TUI application that
 * would like to display it.
 *
 * @caution1hang This function is not thread safe.
 *
 * @param[in] handle  Handle for the active connection with the secure application.
 *
 * @return
 * 0 on success; otherwise, errno or one of Secure Indicator error codes.
 *
 * @dependencies
 * None.
 */
int32_t GetSecureIndicator(struct QSEECom_handle *handle);

/**
 * Starts a Secure Touch session.
 *
 * This call blocks until the Secure Touch session is concluded. If the
 * caller wants to abort the session asynchronously, it must pass a file
 * descriptor, which is polled for data availability. When data is
 * seen as available on this FD, this is interpreted as an abort request.
 *
 * @caution1hang This function is not thread safe.
 *
 * @param[in] fd   File descriptor that is waited (polled) on to request the
 *                 service to be aborted early.
 * @param[in] handle  Handle for the active connection with the secure application.
 *
 * @return
 * 0 on success; otherwise, errno.
 *
 * @dependencies
 * None.
 */
int32_t UseSecureTouch(int fd, struct QSEECom_handle *handle);

/** @} */ /* end_addtogroup hlos_sec_ui_lib */

#ifdef __cplusplus
  }
#endif
