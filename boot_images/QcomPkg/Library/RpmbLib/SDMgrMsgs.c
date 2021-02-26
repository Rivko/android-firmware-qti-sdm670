/** @file SDMgrMsgs.c
 *
 * This file provides routines for communication between
 * HLOS environment and SD_MGR in TZ's error code.
 *
 * Copyright (c) 2012 - 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 **/

/*=============================================================================
                              EDIT HISTORY


  when         who     what, where, why
  ----------   ---     ---------------------------------------------------------
  2015-04-17   jt/rm   Add support for ufs rpmb
  2014-03-27   bn      Fixed 64-bit pointer size
  2012-11-02   bb      Initial Version

 =============================================================================*/

#include "SDMgrMsgs.h"

/* Get TZ Error code from non-secure error code */
INT32 TZSDMgrGetErrorCode(EFI_STATUS ErrorCode)
{
  switch (ErrorCode)
  {
    case EFI_SUCCESS:
      return SD_MGR_MSG_SUCCESS;

    case EFI_INVALID_PARAMETER:
      return SD_MGR_MSG_INVALID_PARAM_ERROR;

    case EFI_NO_MEDIA:
      return SD_MGR_MSG_DEVICE_NOT_FOUND_ERROR;

    case EFI_OUT_OF_RESOURCES:
      return SD_MGR_MSG_OUT_OF_RESOURCES;

    case EFI_UNSUPPORTED:
      return SD_MGR_MSG_NOT_SUPPORTED;
  }

  return SD_MGR_MSG_ERROR;
}

/* Check RPMB Listener version with TZ
   Return 1 if both TZ and listener use same version
   Return 0 if there is a diff. between TZ and listener */
INT32 TZSDMgrRpmbVersionCheck(UINT32 Version)
{
  return ((Version == SD_MGR_RPMB_MSGS_VERSION)? 1:0);
}

/* Check GPT Listener version with TZ
   Return 1 if both TZ and listener use same version
   Return 0 if there is a diff. between TZ and listener */
INT32 TZSDMgrGptVersionCheck(UINT32 Version)
{
  return ((Version == SD_MGR_GPT_MSGS_VERSION)? 1:0);
}
