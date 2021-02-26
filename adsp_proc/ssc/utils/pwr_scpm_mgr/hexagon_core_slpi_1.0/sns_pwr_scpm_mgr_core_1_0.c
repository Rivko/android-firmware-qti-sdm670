/**
 * sns_pwr_sleep_mgr_core_1_0.c
 *
 * SCPM manager core dependencies for older targets
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 **/

#include "DDIChipInfo.h"
#include "sns_island.h"
#include "sns_types.h"

SNS_SECTION(".text.sns") bool 
sns_pwr_scpm_mgr_check_chip_and_version(DalChipInfoIdType chip, DalChipInfoVersionType version)
{
  UNUSED_VAR(chip);
  UNUSED_VAR(version);
  return false;
}
