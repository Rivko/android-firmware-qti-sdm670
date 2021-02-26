/*! \file pm_version_protocol_wrap.c
*  \n
*  \brief This file contains PMIC version auto-detection implementation
*  \n
*  \n &copy; Copyright 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/05/14   jjo     Fix build error in pm_get_pmic_revision
12/5/14   al       New file
========================================================================== */
/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <api/pmic/pm/pm_version.h>
#include <Protocol/EFIPmicVersion.h>



/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/*=========================================================================
      Function Definitions
==========================================================================*/

STATIC EFI_QCOM_PMIC_VERSION_PROTOCOL            *PmicVersionProtocol  = NULL;

pm_err_flag_type pm_version_protocol_init(void)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  pm_err_flag_type         err_flag = PM_ERR_FLAG_SUCCESS;

  if (NULL == PmicVersionProtocol)
  {
    Status = gBS->LocateProtocol(&gQcomPmicVersionProtocolGuid, NULL, (VOID **)&PmicVersionProtocol);
    err_flag = (Status == EFI_SUCCESS) ? PM_ERR_FLAG_SUCCESS : PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  return err_flag;
}


pm_err_flag_type pm_get_pmic_info(uint8 pmic_index, pm_device_info_type* pmic_device_info_ptr)

{
  pm_err_flag_type       err_flag = PM_ERR_FLAG_SUCCESS;
  EFI_STATUS  Status = EFI_SUCCESS;

  if ((PM_ERR_FLAG_SUCCESS == pm_version_protocol_init()) && pmic_device_info_ptr)
  {
    Status = PmicVersionProtocol->GetPmicInfo(pmic_index, (EFI_PM_DEVICE_INFO_TYPE *)pmic_device_info_ptr);

    err_flag = (Status == EFI_SUCCESS) ? PM_ERR_FLAG_SUCCESS : PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  return err_flag;
}



pm_model_type pm_get_pmic_model(uint8 pmic_index)
{
  pm_device_info_type  pmic_device_info;
  pm_model_type        pmic_model = PMIC_IS_INVALID;

  if (PM_ERR_FLAG_SUCCESS == pm_get_pmic_info(pmic_index, &pmic_device_info))
  {
    pmic_model = pmic_device_info.ePmicModel;
  }

  return pmic_model;
}


uint8 pm_get_pmic_revision(uint8 pmic_index)
{
  pm_device_info_type  pmic_device_info;
  uint8                pmic_revision = 0;

  if (PM_ERR_FLAG_SUCCESS == pm_get_pmic_info(pmic_index, &pmic_device_info) )
  {
    pmic_revision = pmic_device_info.nPmicAllLayerRevision;
  }

  return pmic_revision;
}




