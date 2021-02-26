#ifndef SECURE_INDICATOR_API_H
#define SECURE_INDICATOR_API_H

/** @file SecureIndicatorAPI.h
 * @brief
 * This file contains the definitions of the constants, data structures
 * and interfaces to the Secure Indicator Module
 */

/*===========================================================================
 Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

					EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/15    kl     (Tech Pubs) Comment updates for 80-NJ546-1 Rev. J.
02/23/15   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
02/05/15   rz      Modified GetDimensions API to support width and height
12/29/14   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
08/14/14   rz      Added GetDimensions API + updated error codes
05/20/14   rz      Initial Version

===========================================================================*/

/*----------------------------------------------------------------------------
 *Type Declarations
 *--------------------------------------------------------------------------*/

#include "comdef.h"

/** @addtogroup hlos_sec_indicator_client
@{ */
/** Secure indicator error codes. */
typedef enum {
    TUI_SI_SUCCESS                        = 0,   /**< Success. */
    TUI_SI_ERROR                          = -1,  /**< Error. */
    TUI_SI_INIT_ERR                       = -2,  /**< Initialization error. */
    TUI_SI_TEARDOWN_ERR                   = -3,  /**< Teardown error. */
    TUI_SI_QSEE_APP_NOT_INITIALIZED       = -4,  /**< QSEE application is not initialized. */
    TUI_SI_INVALID_INPUT_ERR              = -5,  /**< Invalid input. */
    TUI_SI_IS_PROVISIONED_ERR             = -6,  /**< Error occurred when checking whether the secure indicator is provisioned. */
    TUI_SI_STORE_INDICATOR_ERR            = -7,  /**< Error occurred when storing the secure indicator. */
    TUI_SI_INVALID_PNG_ERR                = -8,  /**< Invalid PNG. */
    TUI_SI_FS_FILE_NOT_REMOVED_ERR        = -9,  /**< Original image was not removed from the file system. */
    TUI_SI_PROVIDE_INDICATOR_ERR          = -10, /**< Error occurred when providing the secure indicator. */
    TUI_SI_INDICATOR_NOT_PROVISIONED_ERR  = -11, /**< Secure indicator is not provisioned. */
    TUI_SI_APP_NOT_APPROVED_ERR           = -12, /**< Application is not approved to display the secure indicator. */
    TUI_SI_GET_DIMENSIONS_ERR             = -13, /**< Error occurred when getting the secure indicator dimensions. */
    TUI_SI_REMOVE_INDICATOR_ERR           = -14, /**< Error occurred when removing the secure indicator from TrustZone. @newpage */
/** @cond */
    TUI_SI_ERR_SIZE                       = 0x7FFFFFFF
/** @endcond */
} tui_si_status_t;

/** Maximum image size. */
#define MAX_IMAGE_SIZE              400*1024  //400KB

/*----------------------------------------------------------------------------
 *Function Declarations and Documentation
 *--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
  Initializes the Secure Indicator module. This function loads the
  Secure Indicator module into TrustZone.

  @return
  #TUI_SI_SUCCESS \n
  #TUI_SI_INIT_ERR

  @dependencies
  Must be called prior to invoking any of the other Secure Indicator functions.
*/
tui_si_status_t SecureIndicator_Init();


/**
  Terminates the operation of the Secure Indicator module in TrustZone.

  @return
  #TUI_SI_SUCCESS \n
  #TUI_SI_TEARDOWN_ERR

  @dependencies
  SecureIndicator_Init() must have been called.
*/
tui_si_status_t SecureIndicator_Teardown();

/**
  Checks whether the secure indicator is already provisioned.

  @param[out]  result  Set to TRUE if the secure indicator is provisioned;
                       otherwise set to FALSE.

  @return
  #TUI_SI_SUCCESS \n
  #TUI_SI_INVALID_INPUT_ERR \n
  #TUI_SI_QSEE_APP_NOT_INITIALIZED \n
  #TUI_SI_IS_PROVISIONED_ERR

  @dependencies
  SecureIndicator_Init() must have been called.
*/
tui_si_status_t SecureIndicator_IsIndicatorProvisioned(boolean* result);


/**
  Stores the secure indicator.

  This function copies the image from the specified FS path and saves it
  securely in TrustZone. After the image is successfully saved into TrustZone,
  it is removed from the FS. The image size must not exceed MAX_IMAGE_SIZE.

  @param[in]  imagePath  Path to the image that is stored as the secure indicator.

  @return
  #TUI_SI_SUCCESS \n
  #TUI_SI_INVALID_INPUT_ERR \n
  #TUI_SI_QSEE_APP_NOT_INITIALIZED \n
  #TUI_SI_INVALID_PNG_ERR \n
  #TUI_SI_STORE_INDICATOR_ERR \n
  TUI_SI_FS_FILE_NOT_REMOVED_ERR

  @dependencies
  SecureIndicator_Init() must have been called.

  @sideeffects
  On success, the image is deleted from the FS.
*/
tui_si_status_t SecureIndicator_StoreSecureIndicator(const char* imagePath);

/**
  Provides the maximum allowed dimensions (in pixels) for the secure indicator.
  The returned parameters are the maximum width and the maximum height.

  @param[out]  maxWidth   Set to hold the maximum width in pixels.
  @param[out]  maxHeight  Set to hold the maximum height in pixels.

  @return
  #TUI_SI_SUCCESS \n
  #TUI_SI_INVALID_INPUT_ERR \n
  #TUI_SI_QSEE_APP_NOT_INITIALIZED \n
  #TUI_SI_GET_DIMENSIONS_ERR

  @dependencies
  SecureIndicator_Init() must have been called.
*/
tui_si_status_t SecureIndicator_GetSecureIndicatorDimensions(uint32* maxWidth, uint32* maxHeight);

/**
  Removes the secure indicator from TrustZone.

  @return
  #TUI_SI_SUCCESS \n
  #TUI_SI_QSEE_APP_NOT_INITIALIZED \n
  #TUI_SI_REMOVE_INDICATOR_ERR \n
  #TUI_SI_INDICATOR_NOT_PROVISIONED_ERR


  @dependencies
  SecureIndicator_Init() must have been called.

  @sideeffects
  On success, the image is deleted from TrustZone.
*/
tui_si_status_t SecureIndicator_RemoveIndicator();
/** @} */ /* end_addtogroup hlos_sec_indicator_client */

#ifdef __cplusplus
}
#endif

#endif /* SECURE_INDICATOR_API_H */
