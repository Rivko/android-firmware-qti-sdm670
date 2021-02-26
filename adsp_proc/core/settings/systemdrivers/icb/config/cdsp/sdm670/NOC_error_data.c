/*==============================================================================

FILE:      NOC_error_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/core.qdsp6/2.1/settings/systemdrivers/icb/config/cdsp/sdm670/NOC_error_data.c#2 $ 
$DateTime: 2017/12/06 09:58:14 $
$Author: pwbldsvc $
$Change: 14996571 $ 

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
    //already enabled in pil and reference count was 0 so when try disbling cdsp crashed 
    // .clk_name_array    = (char *[]){ "turing_wrapper_cdsp_noc_axi_clk", "turing_q6ss_q6_axim_clk"},
    // .clk_disable_array = (bool[]){ false, false },
    // .num_of_clk        = 2,
  },
  [1] = {
    .name              = "VAPSS_NOC",
    .hw                = &QNOC_4_0,
    .base_addr         = (void *)HWIO_ADDR(TURING_VAPSS_NOC_VAPSS_ERRLOG_SWID_LOW),
    .intr_vector       = 11,
    .intr_priority     = 1,
    .num_sbms          = 0,
    .sb_hw             = NULL,
    .sb_base_addrs     = NULL,
    .num_tos           = 0,
    .to_addrs          = NULL,
    .syndrome          = { .sbms = (NOCERR_sbm_syndrome_type []){ {0, 0} }, },
    .deferral_pref     = NOCERR_DEFER_ONCE,
    .power_domain      = "vapss_gdsc",
    .power_domain_id   = 0,
    .clk_name_array    = (char *[]){ "turing_vapss_axi_clk", "turing_vapss_vap_tcms_clk", "turing_vapss_vap_core_clk" },
    .clk_disable_array = (bool[]){ true, true, true },
    .num_of_clk        = 3,
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
    .intr_clear  = false,
    .intr_enable = true, /* Disabled on v1 */
    .error_fatal = true,
    .sbms        = NULL,
    .to_reg_vals = NULL,
  },
};

NOCERR_propdata_type_oem NOCERR_propdata_oem =
{
   .len            = sizeof(NOCERR_cfgdata_oem)/sizeof(NOCERR_info_type_oem),
   .NOCInfoOEM     = NOCERR_cfgdata_oem,
};
