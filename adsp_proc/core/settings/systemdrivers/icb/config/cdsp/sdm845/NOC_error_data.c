/*==============================================================================

FILE:      NOC_error_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/core.qdsp6/2.1/settings/systemdrivers/icb/config/cdsp/sdm845/NOC_error_data.c#1 $ 
$DateTime: 2017/07/21 22:10:47 $
$Author: pwbldsvc $
$Change: 13907704 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2017/06/01  sds    Add deferred NoC error programming support
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
    .name            = "CDSP_NOC",
    .hw              = &QNOC_4_0,
    .base_addr       = (void *)HWIO_ADDR(TURING_CDSP_ERRLOG_SWID_LOW),
    .intr_vector     = 37,
    .intr_priority   = 1,
    .num_sbms        = 0,
    .sb_hw           = NULL,
    .sb_base_addrs   = NULL,
    .num_tos         = 0,
    .to_addrs        = NULL,
    .syndrome        = { .sbms = (NOCERR_sbm_syndrome_type []){ {0, 0} }, },
    .deferral_pref   = NOCERR_DEFER_NONE,
    .power_domain    = NULL,
    .power_domain_id = 0,
  },
  [1] = {
    .name            = "VAPSS_NOC",
    .hw              = &QNOC_4_0,
    .base_addr       = (void *)HWIO_ADDR(TURING_VAPSS_NOC_VAPSS_ERRLOG_SWID_LOW),
    .intr_vector     = 11,
    .intr_priority   = 1,
    .num_sbms        = 0,
    .sb_hw           = NULL,
    .sb_base_addrs   = NULL,
    .num_tos         = 0,
    .to_addrs        = NULL,
    .syndrome        = { .sbms = (NOCERR_sbm_syndrome_type []){ {0, 0} }, },
    .deferral_pref   = NOCERR_DEFER_ALWAYS,
    .power_domain    = "vapss_gdsc",
    .power_domain_id = 0,
  },
};

NOCERR_propdata_type NOCERR_propdata =
{
  .len             = sizeof(NOCERR_cfgdata)/sizeof(NOCERR_info_type),
  .NOCInfo         = NOCERR_cfgdata,
};

/* OEM NOC Configuration Data*/
NOCERR_info_type_oem NOCERR_cfgdata_oem[] = 
{ 
  [0] = {
    .name        = "CDSP_NOC",
    .intr_clear  = false,
    .intr_enable = true,
    .error_fatal = true,
    .sbms        = NULL,
    .to_reg_vals = NULL,
  },
  [1] = {
    .name        = "VAPSS_NOC",
    .intr_clear  = true,
    .intr_enable = false, /* Disabled on v1 */
    .error_fatal = false,
    .sbms        = NULL,
    .to_reg_vals = NULL,
  },
};

NOCERR_propdata_type_oem NOCERR_propdata_oem =
{
   .len            = sizeof(NOCERR_cfgdata_oem)/sizeof(NOCERR_info_type_oem),
   .NOCInfoOEM     = NOCERR_cfgdata_oem,
};
