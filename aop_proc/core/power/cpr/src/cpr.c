/*=========================================================================
                                   cpr.c

DESCRIPTION:
  This file initializes CPR HW block using auto generated R-Init output.

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
=========================================================================*/

//=========================================================================
//                      Includes and Variable Definitions
//=========================================================================
//
//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------


#include "rinit.h"
#include "CoreVerify.h"
#include "cpr_types.h"
#include "cmd_db.h"
#include "cprf_hwio.h"
#include "cpr_hwio.h"
#include "cpr_internal.h"
#include "cpr_target.h"
#include "cprf.h"
#include "cpr_image.h"
#include "cpr_retention.h"
#include "tcsr_hwio.h"
#include "ChipInfo.h"

//-------------------------------------------------------------------------
// Public Function Definition
//-------------------------------------------------------------------------

cpr_return_status cpr_enable_controller (uint8 id, boolean enable)
{
    return cpr_target_cprf_ctrl_intr_en(id, enable, FALSE);
}

cpr_return_status cpr_enable_controller_force (uint8 id, boolean enable)
{
    return cpr_target_cprf_ctrl_intr_en(id, enable, TRUE);
}

cpr_return_status cpr_toggle_cprf_interface_en(uint8 id, boolean enable)
{
    return cpr_target_enable_cprf_interface_en(id, enable);
}

uint32 cpr_get_ret_voltage (uint8 id)
{
    return cpr_target_get_ret_voltage(id);
}

cpr_return_status cpr_set_ret_voltage (uint8 id, uint32 vol)
{
    return cpr_target_set_ret_voltage(id, vol);
}

/*
 * cpr_init
 */
void cpr_init (void)
{
    int ret_val = 0;

    ret_val = rinit_module_init ("cpr");
    CORE_VERIFY (ret_val == RINIT_SUCCESS);

    if (ChipInfo_GetChipVersion() < CHIPINFO_VERSION(2,0)) {
        ret_val = rinit_module_init ("cprv1");
        CORE_VERIFY(ret_val == RINIT_SUCCESS);

        //Hack to overwrite the CX Disabled sensors for 845v1.
        if (ChipInfo_GetChipFamily() == CHIPINFO_FAMILY_SDM845) {
            ret_val = rinit_module_init ("cprwa");
            CORE_VERIFY(ret_val == RINIT_SUCCESS);
        }
    } else {
        ret_val = rinit_module_init ("cprv2");
        CORE_VERIFY(ret_val == RINIT_SUCCESS);
    }

    cpr_target_init();
    cpr_retention_init();
}
