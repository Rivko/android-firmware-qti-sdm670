/*==============================================================================

FILE:      NOC_error_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/core.qdsp6/2.1/settings/systemdrivers/icb/config/adsp/sdm670/NOC_error_data.c#1 $ 
$DateTime: 2017/07/21 22:10:47 $
$Author: pwbldsvc $
$Change: 13907704 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2017/03/01  sds    Add timeout enable registers
2017/02/07  sds    Created
 
        Copyright (c) 2017 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
                        Hardware uses flags
============================================================================*/
#define USES_QNOC_4_0
#define USES_QNOC_SB_4_0_L
/*============================================================================
                        INCLUDES
============================================================================*/
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
    .name            = "SSC_NOC",
    .hw              = &QNOC_4_0,
    .base_addr       = (void *)HWIO_ADDR(SSC_NOC_OBS_SWID_LOW),
    .intr_vector     = 273,
    .intr_priority   = 1,
    .num_sbms        = 1,
    .sb_hw           = (NOC_sideband_hw_type *[]){ &QNOC_SB_4_0_L },
    .sb_base_addrs   = (void *[]){ (void *)HWIO_ADDR(SSC_NOC_SBM_SWID_LOW) },
    .num_tos         = 1,
    .to_addrs        = (void *[]){ (void *)HWIO_ADDR(SSC_NOC_SBM_FLAGOUTSET0_LOW) },
    .syndrome        = { .sbms = (NOCERR_sbm_syndrome_type []){ {0, 0} }, },
  },
};

static void *clock_reg_addrs[] =
{
  (void *)HWIO_ADDR(SSC_SCC_QTIMER_TMR_CBCR),
  (void *)HWIO_ADDR(SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR)
};

NOCERR_propdata_type NOCERR_propdata =
{
  .len             = sizeof(NOCERR_cfgdata)/sizeof(NOCERR_info_type),
  .NOCInfo         = NOCERR_cfgdata,
  .num_clock_regs  = sizeof(clock_reg_addrs)/sizeof(clock_reg_addrs[0]),
  .clock_reg_addrs = clock_reg_addrs,
};

/* OEM NOC Configuration Data*/
NOCERR_info_type_oem NOCERR_cfgdata_oem[] = 
{ 
  [0] = {
    .name        = "SSC_NOC",
    .intr_clear  = false,
    .intr_enable = true,
    .error_fatal = true,
    .sbms        = (NOCERR_sbm_info_type_oem []){
        [0] = { .faultin_en0_low = 0x1EFF},
      },
    .to_reg_vals = (uint32_t []){ 0x1801F},
  },
};

uint32_t clock_reg_vals[] =
{
  [0] = 0x1, /* Enable timeout reference clock */
  [1] = 0x1FF, /* Set timeout divider to maximum */
};

NOCERR_propdata_type_oem NOCERR_propdata_oem =
{
   .len            = sizeof(NOCERR_cfgdata_oem)/sizeof(NOCERR_info_type_oem),
   .NOCInfoOEM     = NOCERR_cfgdata_oem,
   .clock_reg_vals = clock_reg_vals,
};
