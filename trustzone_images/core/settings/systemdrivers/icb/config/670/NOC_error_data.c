/*==============================================================================

FILE:      NOC_error_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/core.tz/2.0.2/settings/systemdrivers/icb/config/670/NOC_error_data.c#3 $ 
$DateTime: 2018/07/30 02:57:11 $
$Author: pwbldsvc $
$Change: 16760089 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2017/10/02  sds    Add support for chip versioned configs
2017/04/11  sds    Enable MMSS NOC
2017/03/01  sds    Add timeout enable registers
2017/02/07  sds    Created
 
        Copyright (c) 2017 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "NOC_error.h"
#include "NOC_error_HWIO.h"
#include "HALhwio.h"

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/* Internal NOC Configuration Data*/
static NOCERR_info_type NOCERR_cfgdata[] = 
{ 
  [0] = {
    .name            = "A1NOC",
    .hw              = &QNOC_4_0,
    .base_addr       = (void *)HWIO_ADDR(AGGRE_NOC_A1NOC_ERRLOG_SWID_LOW),
    .intr_vector     = 395,
    .num_sbms        = 1,
    .sb_hw           = (NOC_sideband_hw_type *[]){ &QNOC_SB_4_0_L },
    .sb_base_addrs   = (void *[]){ (void *)HWIO_ADDR(AGGRE_NOC_A1NOC_SBM_SWID_LOW) },
    .num_tos         = 0,
    .to_addrs        = NULL,
    .syndrome        = { .sbms = (NOCERR_sbm_syndrome_type []){ {0, 0} }, },
  },
  [1] = {
    .name            = "A2NOC",
    .hw              = &QNOC_4_0,
    .base_addr       = (void *)HWIO_ADDR(AGGRE_NOC_A2NOC_ERRLOG_SWID_LOW),
    .intr_vector     = 404,
    .num_sbms        = 1,
    .sb_hw           = (NOC_sideband_hw_type *[]){ &QNOC_SB_4_0_L },
    .sb_base_addrs   = (void *[]){ (void *)HWIO_ADDR(AGGRE_NOC_A2NOC_SBM_SWID_LOW) },
    .num_tos         = 0,
    .to_addrs        = NULL,
    .syndrome        = { .sbms = (NOCERR_sbm_syndrome_type []){ {0, 0} }, },
  },
  [2] = {
    .name            = "CONFIG_NOC",
    .hw              = &QNOC_4_0,
    .base_addr       = (void *)HWIO_ADDR(CNOC_OBS_SWID_LOW),
    .intr_vector     = 226,
    .num_sbms        = 1,
    .sb_hw           = (NOC_sideband_hw_type *[]){ &QNOC_SB_4_0 },
    .sb_base_addrs   = (void *[]){ (void *)HWIO_ADDR(CNOC_SBM_SWID_LOW) },
    .num_tos         = 1,
    .to_addrs        = (void *[]){ (void *)HWIO_ADDR(CNOC_SBM_FLAGOUTSET0_LOW) },
    .syndrome        = { .sbms = (NOCERR_sbm_syndrome_type []){ {0, 0} }, },
  },
  [3] = {
    .name            = "DC_NOC",
    .hw              = &QNOC_4_0,
    .base_addr       = (void *)HWIO_ADDR(DC_NOC_ERL_ERRORLOGGER_MAIN_ERRORLOGGER_SWID_LOW),
    .intr_vector     = 290,
    .num_sbms        = 1,
    .sb_hw           = (NOC_sideband_hw_type *[]){ &QNOC_SB_4_0_L },
    .sb_base_addrs   = (void *[]){ (void *)HWIO_ADDR(DC_NOC_DC_NOC_SBM_MAIN_SIDEBANDMANAGER_SWID_LOW) },
    .num_tos         = 1,
    .to_addrs        = (void *[]){ (void *)HWIO_ADDR(DC_NOC_DC_NOC_SBM_MAIN_SIDEBANDMANAGER_FLAGOUTSET0_LOW) },
    .syndrome        = { .sbms = (NOCERR_sbm_syndrome_type []){ {0, 0} }, },
  },
  [4] = {
    .name            = "MEM_NOC",
    .hw              = &QNOC_4_0,
    .base_addr       = (void *)HWIO_ADDR(MEMNOC_MEM_NOC_ERRORLOGGER_SWID_LOW),
    .intr_vector     = 108,
    .num_sbms        = 1,
    .sb_hw           = (NOC_sideband_hw_type *[]){ &QNOC_SB_4_0_L },
    .sb_base_addrs   = (void *[]){ (void *)HWIO_ADDR(MEMNOC_MEM_NOC_SBM_SWID_LOW) },
    .num_tos         = 0,
    .to_addrs        = NULL,
    .syndrome        = { .sbms = (NOCERR_sbm_syndrome_type []){ {0, 0} }, },
  },
  [5] = {
    .name            = "SYSTEM_NOC",
    .hw              = &QNOC_4_0,
    .base_addr       = (void *)HWIO_ADDR(SNOC_OBS_SWID_LOW),
    .intr_vector     = 198,
    .num_sbms        = 1,
    .sb_hw           = (NOC_sideband_hw_type *[]){ &QNOC_SB_4_0_L },
    .sb_base_addrs   = (void *[]){ (void *)HWIO_ADDR(SNOC_CENTER_SBM_SWID_LOW) },
    .num_tos         = 1,
    .to_addrs        = (void *[]){ (void *)HWIO_ADDR(SNOC_CENTER_SBM_FLAGOUTSET0_LOW) },
    .syndrome        = { .sbms = (NOCERR_sbm_syndrome_type []){ {0, 0} }, },
  },
  [6] = {
    .name            = "MMSS_NOC",
    .hw              = &QNOC_4_0,
    .base_addr       = (void *)HWIO_ADDR(MNOC_ERRLOG_SWID_LOW),
    .intr_vector     = 186,
    .num_sbms        = 1,
    .sb_hw           = (NOC_sideband_hw_type *[]){ &QNOC_SB_4_0_L },
    .sb_base_addrs   = (void *[]){ (void *)HWIO_ADDR(MNOC_SBM_SWID_LOW) },
    .num_tos         = 0,
    .to_addrs        = NULL,
    .syndrome        = { .sbms = (NOCERR_sbm_syndrome_type []){ {0, 0} }, },
  },
};

static void *clock_reg_addrs[] =
{
  (void *)HWIO_ADDR(GCC_NOC_BUS_TIMEOUT_EXTREF_CBCR),
  (void *)HWIO_ADDR(GCC_NOC_BUS_TIMEOUT_EXTREF_CDIVR)
};

NOCERR_config_info_type NOCERR_propdata =
{
  .num_configs = 3,
  .configs = (NOCERR_propdata_type [])
    {
      /* Target info: SDM670 v1.0 and later */
      [0] = 
        {
          .family          = CHIPINFO_FAMILY_SDM670,
          .match           = false,
          .version         = CHIPINFO_VERSION(1,0),

          /* NoC info. */
          .len             = sizeof(NOCERR_cfgdata)/sizeof(NOCERR_info_type),
          .NOCInfo         = NOCERR_cfgdata,
          .num_clock_regs  = sizeof(clock_reg_addrs)/sizeof(clock_reg_addrs[0]),
          .clock_reg_addrs = clock_reg_addrs,
        },
	  /* Target info: kingsman */
      [1] = 
        {
          .family          = CHIPINFO_FAMILY_SXR1130,
          .match           = false,
          .version         = CHIPINFO_VERSION(1,0),

          /* NoC info. */
          .len             = sizeof(NOCERR_cfgdata)/sizeof(NOCERR_info_type),
          .NOCInfo         = NOCERR_cfgdata,
          .num_clock_regs  = sizeof(clock_reg_addrs)/sizeof(clock_reg_addrs[0]),
          .clock_reg_addrs = clock_reg_addrs,
        },
      /* Target info: QCS605 v1.0 and later */
      [2] = 
        {
          .family          = CHIPINFO_FAMILY_QCS605,
          .match           = false,
          .version         = CHIPINFO_VERSION(1,0),

          /* NoC info. */
          .len             = sizeof(NOCERR_cfgdata)/sizeof(NOCERR_info_type),
          .NOCInfo         = NOCERR_cfgdata,
          .num_clock_regs  = sizeof(clock_reg_addrs)/sizeof(clock_reg_addrs[0]),
          .clock_reg_addrs = clock_reg_addrs,
        },
    },
};
