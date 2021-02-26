/**
 * sns_pwr_sleep_mgr_core_2_0.c
 *
 * SCPM Manager core dependencies for SDM845
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
  bool ret_val = false;
  
  if((chip == CHIPINFO_ID_SDM845) && (version < DALCHIPINFO_VERSION(2, 0)))
  {
    ret_val = true;
  }
  return ret_val;
}
