/** @file  
                        scm_util.c

  Scm utility function needed for verified boot
  
  Copyright (c) 2016 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                EDIT HISTORY


 when       who    what, where, why
 --------   ---     ----------------------------------------------------------
 08/26/16   SA     Fix check for secure device
 07/28/16   SA     Initial version

=============================================================================*/

#include <Protocol/EFIScm.h>                     // for QCOM_SCM_PROTOCOL
#include <Include/scm_sip_interface.h>           // for TZ Structs
#include <Library/DebugLib.h>                    // for Debug(())
#include <Library/UncachedMemoryAllocationLib.h> // for UncachedAllocatePool
#include <Library/UefiBootServicesTableLib.h>    // for gBS
#include <Guid/EventGroup.h>
#include "scm_util.h"

#define SECBOOT_FUSE 0
#define SHK_FUSE 1
#define DEBUG_DISABLED_FUSE 2
#define ANTI_ROLLBACK_FUSE 3
#define FEC_ENABLED_FUSE 4
#define RPMB_ENABLED_FUSE 5
#define DEBUG_RE_ENABLED_FUSE 6
#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))

/**

	Read security state.
      
    @retval         Status                  Secure state or error code in case of failure.
    
**/
UINT32 readSecurityState(void)
{
  QCOM_SCM_PROTOCOL *pQcomScmProtocol = NULL;
  EFI_STATUS Status = EFI_SUCCESS;
  UINT64 Parameters[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64 Results[SCM_MAX_NUM_RESULTS] = {0};
  tz_get_secure_state_rsp_t *SysCallRsp = (tz_get_secure_state_rsp_t *)Results;
  UINT32 SecurityStateReturn = 0x0;

  // Locate QCOM_SCM_PROTOCOL.
  Status = gBS->LocateProtocol(&gQcomScmProtocolGuid, NULL,
                               (VOID **)&pQcomScmProtocol);
  if (EFI_ERROR(Status)) // || (pQcomScmProtocol == NULL))
  {
    DEBUG((EFI_D_ERROR, "VB: readSecurityState: Locate SCM Status: (0x%x)\r\n",
           Status));
    Status = EFI_NOT_FOUND;
    goto ErrorExit;
  }
  // Make ScmSipSysCall
  Status = pQcomScmProtocol->ScmSipSysCall(
      pQcomScmProtocol, TZ_INFO_GET_SECURE_STATE,
      TZ_INFO_GET_SECURE_STATE_PARAM_ID, Parameters, Results);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "VB: readSecurityState: ScmSipSysCall Status: (0x%x)\r\n", Status));
    Status = ERROR_SECURITY_STATE;
    goto ErrorExit;
  }

  if (SysCallRsp->common_rsp.status != 1) {
    DEBUG((EFI_D_WARN, "VB: readSecurityState: ScmSysCall failed: Status = (0x%x)\r\n",
           SysCallRsp->common_rsp.status));
    Status = ERROR_SECURITY_STATE;
    goto ErrorExit;
  }
  // Parse the return value and assign
  SecurityStateReturn = SysCallRsp->status_0;

ErrorExit:

  if (EFI_SUCCESS != Status) {
    return ERROR_SECURITY_STATE; // Error during getting security state
  }
  return SecurityStateReturn; // Return the security status value
}

/**

	Determine whether the devcie is secure or not.
	
    @param[out]     *is_secure             indicates whether the device is secure or not
    @retval         status                 Indicates whether reading the security state was successful
    
**/

EFI_STATUS is_secure_device(BOOLEAN *is_secure)
{
  EFI_STATUS status = EFI_FAILURE;
  if (is_secure == NULL) {
    goto exit;
  }
  UINT32 secure_state = readSecurityState();
  if (secure_state == ERROR_SECURITY_STATE) {
    goto exit;
  }
  *is_secure = FALSE;
  /* Check for secure device: Bit#0 = 0, Bit#1 = 0 Bit#2 = 0 , Bit#5 = 0 , Bit#6 = 1 */
  if (!CHECK_BIT(secure_state, SECBOOT_FUSE) && !CHECK_BIT(secure_state, SHK_FUSE) &&
      !CHECK_BIT(secure_state, DEBUG_DISABLED_FUSE) && !CHECK_BIT(secure_state, RPMB_ENABLED_FUSE) &&
      CHECK_BIT(secure_state, DEBUG_RE_ENABLED_FUSE)) {
    *is_secure = TRUE;
  } 
  status = EFI_SUCCESS;
exit:
  return status;
}

/**

	Determine whether the devcie is secure or not without considering the 
	RPMB fuse. This is useful for when a secure device boots up.
	
    @param[out]     *is_secure_no_rpmb             Indicates whether the device is secure or not without considering RPMB fuse
    @retval         status                         Indicates whether reading the security state was successful
    
**/
EFI_STATUS is_secure_device_nocheck_rpmb(BOOLEAN *is_secure_no_rpmb)
{
  EFI_STATUS status = EFI_FAILURE;
  if (is_secure_no_rpmb == NULL) {
    goto exit;
  }
  UINT32 secure_state = readSecurityState();
  if (secure_state == ERROR_SECURITY_STATE) {
    goto exit;
  }
  *is_secure_no_rpmb = FALSE;
  /* Check for secure device: Bit#0 = 0, Bit#1 = 0 Bit#2 = 0 , Bit#5 = 0 , Bit#6 = 1 */
  if (!CHECK_BIT(secure_state, SECBOOT_FUSE) && !CHECK_BIT(secure_state, SHK_FUSE) &&
      !CHECK_BIT(secure_state, DEBUG_DISABLED_FUSE) && CHECK_BIT(secure_state, DEBUG_RE_ENABLED_FUSE)) {
    *is_secure_no_rpmb = TRUE;
  } 
  status = EFI_SUCCESS;
exit:
  return status;
}

EFI_STATUS set_fuse(UINT32 fuse_id)
{
  EFI_STATUS status = EFI_FAILURE;
  QCOM_SCM_PROTOCOL *test_scm_protocol = 0;
  UINT64 param[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64 Results[SCM_MAX_NUM_RESULTS] = {0};

  status = gBS->LocateProtocol (&gQcomScmProtocolGuid, NULL, (VOID**)&test_scm_protocol);
  if(status != EFI_SUCCESS || !test_scm_protocol) {
    DEBUG((EFI_D_ERROR, "LocateProtocol failed for set_fuse!\n"));
    goto exit;
  }

  param[0] = fuse_id;

  status = test_scm_protocol->ScmSipSysCall(test_scm_protocol, TZ_BLOW_SW_FUSE_ID, TZ_BLOW_SW_FUSE_ID_PARAM_ID, param, Results);
  if (status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "ScmSipSysCall failed for set_fuse!\n"));
    goto exit;
  }
  status = EFI_SUCCESS;
exit:
  return status;
}

EFI_STATUS get_fuse(UINT32 fuse_id, BOOLEAN *get_fuse_id)
{
  EFI_STATUS status = EFI_FAILURE;
  QCOM_SCM_PROTOCOL *test_scm_protocol = 0;
  UINT64 param[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64 Results[SCM_MAX_NUM_RESULTS] = {0};

  if(!get_fuse_id) {
    goto exit;
  }
  status = gBS->LocateProtocol (&gQcomScmProtocolGuid, NULL, (VOID**)&test_scm_protocol);
  if(status != EFI_SUCCESS || !test_scm_protocol) {
    DEBUG((EFI_D_ERROR, "LocateProtocol failed for get_fuse!\n"));
    goto exit;
  }

  param[0] = fuse_id;

  status = test_scm_protocol->ScmSipSysCall(test_scm_protocol, TZ_IS_SW_FUSE_BLOWN_ID, TZ_IS_SW_FUSE_BLOWN_ID_PARAM_ID, param, Results);
  if (status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "ScmSipSysCall failed for get_fuse!\n"));
    goto exit;
  }
  /*Results[0] is status, Results[1] is output value*/      
  if(Results[1] == 1) {
    *get_fuse_id = TRUE;
  } else {
    *get_fuse_id = FALSE;
  }
  status = EFI_SUCCESS;
exit:
  return status;
}

BOOLEAN allow_set_fuse(UINT32 version)
{
  /*if((major > 4) || (major == 4 && minor > 0))*/
  if((((version >> 22) & 0x3FF) > 4)
        || (((version >> 22) & 0x3FF) == 4 && ((version >> 12) & 0x3FF) > 0)) 
  {
    return TRUE;
  } else {
    return FALSE;
  }
}

EFI_STATUS ScmGetFeatureVersion(UINT32  FeatureId, UINT32 *Version)
{
  EFI_STATUS status = EFI_FAILURE;
  QCOM_SCM_PROTOCOL *test_scm_protocol = 0;
  UINT64 Parameters[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64 Results[SCM_MAX_NUM_RESULTS] = {0};
  tz_feature_version_req_t *SysCallReq = (tz_feature_version_req_t*)Parameters;
  tz_feature_version_rsp_t *SysCallRsp = (tz_feature_version_rsp_t*)Results;

  status = gBS->LocateProtocol (&gQcomScmProtocolGuid, NULL, (VOID**)&test_scm_protocol); 
  if(status != EFI_SUCCESS || !test_scm_protocol) {
    DEBUG((EFI_D_ERROR, "LocateProtocol failed for ScmGetFeatureVersion!\n"));
    goto exit;
  }
  
  SysCallReq->feature_id = FeatureId;  

  status = test_scm_protocol->ScmSipSysCall(test_scm_protocol, TZ_INFO_GET_FEATURE_VERSION_ID, TZ_INFO_GET_FEATURE_VERSION_ID_PARAM_ID, Parameters, Results);
  if (status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "ScmSipSysCall failed for ScmGetFeatureVersion!\n"));
    goto exit;
  }
  if (SysCallRsp->common_rsp.status != 1) {
    status = EFI_DEVICE_ERROR;
    DEBUG(( EFI_D_ERROR, "TZ_INFO_GET_FEATURE_VERSION_ID failed, Status = (0x%x)\r\n", SysCallRsp->common_rsp.status));
    goto exit;
  }

  *Version = SysCallRsp->version;

exit:
  return status;
}
