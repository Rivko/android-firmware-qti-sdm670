/**
  file  EFIQseecom.h

  QSEE Communication Protocol for UEFI.

*/
/*=============================================================================
   Copyright (c) 2015-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/23/15   bmuthuku  Initial version
=============================================================================*/
#ifndef __EFIQSEECOM_H__
#define __EFIQSEECOM_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include <Uefi.h>

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @ingroup
  Protocol version.
*/
#define QCOM_QSEECOM_PROTOCOL_REVISION 0x0000000000010002

#define MAX_APP_NAME_LEN 32
/*  Protocol GUID definition */
/** @ingroup efi_qseecom_protocol */
#define EFI_QSEECOM_PROTOCOL_GUID \
   { 0xa74862ce, 0x680f, 0x4fe1, { 0xa3, 0x11, 0xdf, 0x41, 0xf4, 0x3, 0x3, 0x91 } }
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/** @ingroup*/
extern EFI_GUID gQcomQseecomProtocolGuid;

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/


/** @cond */
/* Protocol declaration.  */
typedef struct _QCOM_QSEECOM_PROTOCOL QCOM_QSEECOM_PROTOCOL;
/** @endcond */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/**
* Start a Secure App
*
* @param char* app_name
*   App name of the Secure App to be started
*   The app_name provided should be the same
*   name as the partition/ file and should
*   be the same name mentioned in TZ_APP_NAME
*   in the secure app.
*
* @return int
*   Success:	handle to be used for all calls to
*   			Secure app. Always greater than zero.
*   Failure:	Error code (negative only).
*/
typedef
EFI_STATUS
(EFIAPI *QCOM_QSEECOM_START_APP )
(
  IN  QCOM_QSEECOM_PROTOCOL   *This,
  IN  CHAR8                   *app_name,
  OUT UINT32                  *handle
);

/**
* Shutdown a Secure App
*
* @param int handle
*   Handle  of the Secure App to be shutdown
*
* @return int
*   Status:
*     0 - Success
*     Negative value indicates failure.
*/
typedef
EFI_STATUS
(EFIAPI *QCOM_QSEECOM_SHUTDOWN_APP )
(
  IN  QCOM_QSEECOM_PROTOCOL   *This,
  IN  UINT32                  handle
);

/**
* SendCmd to a Secure App
*
* @param int handle
*   Handle  of the Secure App to send the cmd
*
* @param void *send_buf
*   Pointer to the App request buffer
*
* @param uint32_t sbuf_len
*   Size of the request buffer
*
* @param void *resp_buf
*   Pointer to the App response buffer
*
* @param uint32_t rbuf_len
*   Size of the response buffer
*
* @return int
*   Status:
*     0 - Success
*     Negative value indicates failure.
*/
typedef
EFI_STATUS
(EFIAPI *QCOM_QSEECOM_SEND_CMD_APP )
(
  IN     QCOM_QSEECOM_PROTOCOL   *This,
  IN     UINT32                  handle,
  IN     UINT8                   *send_buf,
  IN     UINT32                  sbuf_len,
  IN OUT UINT8                   *rsp_buf,
  IN     UINT32                  rbuf_len
);

/**
* Start a Secure App by Guid
*
* @param char* app_name
*   Guid of the Secure App to be started
*
* @return int
*   Success:	handle to be used for all calls to
*   			Secure app. Always greater than zero.
*   Failure:	Error code (negative only).
*/
typedef
EFI_STATUS
(EFIAPI *QCOM_QSEECOM_START_APP_BYGUID )
(
  IN     QCOM_QSEECOM_PROTOCOL      *This, 
  IN     EFI_GUID                  * EfiGuid,
  OUT    UINT32                   *handleId
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup
  @par Summary
  Qualcomm Secure execution environment communication (QSEECOM) Protocol interface.

  @par Parameters
  @inputprotoparams
*/
struct _QCOM_QSEECOM_PROTOCOL {
  UINT64                             Revision;
  QCOM_QSEECOM_START_APP             QseecomStartApp;
  QCOM_QSEECOM_SHUTDOWN_APP          QseecomShutdownApp;
  QCOM_QSEECOM_SEND_CMD_APP          QseecomSendCmd;
  QCOM_QSEECOM_START_APP_BYGUID      QseecomStartAppByGuid;
};

#endif /* __EFIQSEECOM_H__ */


