/*==============================================================================

FILE:      NOC_error_OEM_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/core.tz/2.0.2/settings/systemdrivers/icb/config/670/NOC_error_OEM_data.c#3 $ 
$DateTime: 2018/07/30 02:57:11 $
$Author: pwbldsvc $
$Change: 16760089 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2017/10/02  sds    Add support for chip versioned configs
2017/04/11  sds    Enable MMSS NOC
2017/03/01  sds    Add timeout enable register values
2017/02/07  sds    Created
 
        Copyright (c) 2017 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "NOC_error.h"
#include "NOC_error_HWIO.h"

/*============================================================================
                           TARGET SPECIFIC DATA
============================================================================*/

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/* OEM NOC Configuration Data*/
NOCERR_info_type_oem NOCERR_cfgdata_oem[] = 
{ 
  [0] = {
    .name        = "A1NOC",
    .intr_enable = true,
    .error_fatal = true,
    .sbms        = (NOCERR_sbm_info_type_oem []){ 
        [0] = { .faultin_en0_low = 0x1}, 
      },
    .to_reg_vals = NULL,
  },
  [1] = {
    .name        = "A2NOC",
    .intr_enable = true,
    .error_fatal = true,
    .sbms        = (NOCERR_sbm_info_type_oem []){ 
        [0] = { .faultin_en0_low = 0x1}, 
      },
    .to_reg_vals = NULL,
  },
  [2] = {
    .name        = "CONFIG_NOC",
    .intr_enable = true,
    .error_fatal = true,
    .sbms        = (NOCERR_sbm_info_type_oem []){ 
        [0] = { .faultin_en0_low  = 0xFFFFFFFF,
                .faultin_en0_high = 0x7FFF}, 
      },
    .to_reg_vals = (uint32_t []){ 0xFFFFFFF },
  },
  [3] = {
    .name        = "DC_NOC",
    .intr_enable = true,
    .error_fatal = true,
    .sbms        = (NOCERR_sbm_info_type_oem []){ 
        [0] = { .faultin_en0_low = 0x7F}, 
      },
    .to_reg_vals = (uint32_t []){ 0x3F },
  },
  [4] = {
    .name        = "MEM_NOC",
    .intr_enable = true,
    .error_fatal = true,
    .sbms        = (NOCERR_sbm_info_type_oem []){ 
        [0] = { .faultin_en0_low = 0x3FFF}, 
      },
    .to_reg_vals = NULL,
  },
  [5] = {
    .name        = "SYSTEM_NOC",
    .intr_enable = true,
    .error_fatal = true,
    .sbms        = (NOCERR_sbm_info_type_oem []){ 
        [0] = { .faultin_en0_low = 0x7FFF}, 
      },
    .to_reg_vals = (uint32_t []){ 0x2F5 }, /**< PCIe timeouts disabled. */
  },
  [6] = {
    .name        = "MMSS_NOC",
    .intr_enable = true,
    .error_fatal = false,
    .sbms        = (NOCERR_sbm_info_type_oem []){ 
        [0] = { .faultin_en0_low = 0x1}, 
      },
    .to_reg_vals = NULL,
  },
};

uint32_t clock_reg_vals[] =
{
  [0] = 0x1, /* Enable timeout reference clock */
  [1] = 0xF, /* Set timeout divider to maximum, timeout = ~6.8 ms */
};

NOCERR_config_info_type_oem NOCERR_propdata_oem =
{
  .num_configs = 3,
  .configs = (NOCERR_propdata_type_oem [])
    {
      /* Target info: SDM670 v1.0 */
      [0] = 
        {
          .family          = CHIPINFO_FAMILY_SDM670,
          .match           = false,
          .version         = CHIPINFO_VERSION(1,0),

          .len            = sizeof(NOCERR_cfgdata_oem)/sizeof(NOCERR_info_type_oem),
          .NOCInfoOEM     = NOCERR_cfgdata_oem,
          .clock_reg_vals = clock_reg_vals,
        },
	  /* Target info: Kingsman */
      [1] = 
        {
          .family          = CHIPINFO_FAMILY_SXR1130,
          .match           = false,
          .version         = CHIPINFO_VERSION(1,0),

          .len            = sizeof(NOCERR_cfgdata_oem)/sizeof(NOCERR_info_type_oem),
          .NOCInfoOEM     = NOCERR_cfgdata_oem,
          .clock_reg_vals = clock_reg_vals,
        },
	  /* Target info: QCS605 v1.0 and later */
      [2] = 
        {
          .family          = CHIPINFO_FAMILY_QCS605,
          .match           = false,
          .version         = CHIPINFO_VERSION(1,0),

          .len            = sizeof(NOCERR_cfgdata_oem)/sizeof(NOCERR_info_type_oem),
          .NOCInfoOEM     = NOCERR_cfgdata_oem,
          .clock_reg_vals = clock_reg_vals,
        },
    },
};
