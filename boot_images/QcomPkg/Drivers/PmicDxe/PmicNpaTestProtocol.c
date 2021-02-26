/*! @file PmicNpaTestProtocol.c 

*  PMIC- NPATEST MODULE RELATED DECLARATION
*  This file contains functions and variable declarations to support 
*  the PMIC NPATEST module.to enable APT tests to call in NPA API calls for executing NPA Tests
*
*  Copyright (c) 2012 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/22/16   pb     Port latest changes and Enable protocol in 8998
04/28/14   va     New file.(Expose Npa Test protocol)
===========================================================================*/

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include "pm_uefi.h"

#include <Protocol/EFIPmicNpaTest.h>
//#include <Library/PmicLib/npa/test/pm_npa_test.h>
#include <Library/PmicLib/prm/inc/pm_prm_test.h>


/*===========================================================================
EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

/**
EFI_PmicNpaTestInit ()

@brief
Initializes NPATEST
*/
EFI_STATUS
  EFI_PmicNpaTestInit(void)
{
  EFI_STATUS Status = EFI_SUCCESS;
  return Status;
}



/**
EFI_PmicNpaTestInit ()

@brief
Initializes NPATEST
*/
EFI_STATUS
EFIAPI
  EFI_CreateTestClients()
{
  EFI_STATUS Status = EFI_SUCCESS;

    /* Call to create test clinets */
    //pm_npa_test_init();
    pm_prm_test_init();

    return Status;
}


/**
EFI_GetResourceInfo ()

@brief
Get Npa Resource Info
*/
EFI_STATUS
EFIAPI
EFI_GetResourceInfo(EFI_PMIC_PRM_TEST_RESOURCEINFO *PamResourceInfo)
{
  EFI_STATUS Status = EFI_SUCCESS;

  PMIC_PRM_TEST_RESOURCEINFO pam_resource_info;

  if(NULL == PamResourceInfo)
  {
    return EFI_DEVICE_ERROR;
  }

  /* Call to get PAM resource Info */
  Status = (EFI_STATUS)pm_prm_get_resource_info(&pam_resource_info);

  PamResourceInfo->NumOfTestNodes = (UINT32)(pam_resource_info.num_of_test_nodes);
  PamResourceInfo->RsrcTestName = (char*)(pam_resource_info.rsrc_test_name);
  PamResourceInfo->NodeRsrcArr = (EFI_PM_PRM_NODE_RESOURCE_INFO*)(pam_resource_info.node_rsrc_arr);

  return Status;
}


/**
PMIC NPATEST UEFI Protocol implementation
*/
EFI_QCOM_PMIC_NPATEST_PROTOCOL PmicNpaTestProtocolImplementation = 
{
  PMIC_NPATEST_REVISION,
  EFI_PmicNpaTestInit,
  EFI_CreateTestClients,
  EFI_GetResourceInfo
};

