/*
===========================================================================

FILE:         railway_config.c

DESCRIPTION:
  Per target railway configurations

===========================================================================

                             Edit History

$Date: 2018/01/25 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "railway_config.h"
#include "pm_ldo.h"
#include "pm_smps.h"
#include "pm_version.h" 

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

//
// BEGIN config data; should migrate to the system enumeration data method
//
static const railway_config_data_t temp_config_data =
{
    .rails     = (railway_rail_config_t[])
    {
        //Must init Mx first, as voting on the other rails will cause Mx changes to occur.
        {
            .vreg_name = "vddmx",

            // SMPS2A
            .pmic_chip_id   = 0,
            .pmic_peripheral_index = PM_SMPS_2,

            .pmic_volt_func = pm_smps_volt_level,

            .initial_corner = RAILWAY_NO_REQUEST,
            .default_retention_uv = 488000,
        },

        // VDDCX
        {
            .vreg_name = "vddcx",

            // SMPS1C_4C
            .pmic_chip_id   = 2,
            .pmic_peripheral_index = PM_SMPS_1,

            .pmic_volt_func = pm_smps_volt_level,

            .initial_corner = RAILWAY_NO_REQUEST,
            .default_retention_uv = 400000,
        },

        // VDDA_EBI
        {
            .vreg_name = "vdda_ebi",

            // SMPS2A
            .pmic_chip_id   = 0,
            .pmic_peripheral_index = PM_SMPS_2,

            .pmic_volt_func = NULL,

            .initial_corner = RAILWAY_NO_REQUEST,
            .default_retention_uv = 400000,
        },

        // VDD_LPI_MX
        {
            .vreg_name = "vdd_lpi_mx",

            // SMPS2A
            .pmic_chip_id   = 0,
            .pmic_peripheral_index = PM_SMPS_2,

            .pmic_volt_func = NULL, // This rail is to just support PMIC driver to query corner voltage, and Railway driver does not call PMIC API to program its voltage.

            .initial_corner = RAILWAY_NO_REQUEST,
            .default_retention_uv = 536000,
        },

        // VDD_LPI_CX
        {
            .vreg_name = "vdd_lpi_cx",

            // LDO7A
            .pmic_chip_id   = 0,
            .pmic_peripheral_index = PM_LDO_7,

            .pmic_volt_func = NULL, // This rail is to just support PMIC driver to query corner voltage, and Railway driver does not call PMIC API to program its voltage.

            .initial_corner = RAILWAY_NO_REQUEST,
            .default_retention_uv = 400000,
        },

        // VDDA_WLAN
        {
            .vreg_name = "vdda_wlan",

            // LDO1A
            .pmic_chip_id   = 0,
            .pmic_peripheral_index = PM_LDO_1,

            .pmic_volt_func = NULL, // This rail is to just support PMIC driver to query corner voltage, and Railway driver does not call PMIC API to program its voltage.
                                    // WLAN voltage is programmed by PMIC driver in pm_config_ssc_active_and_retention_level().

            .initial_corner = RAILWAY_NO_REQUEST,
            .default_retention_uv = 488000,
        },
    },

    .num_rails = 6,
};
//
// END config data
//

const railway_config_data_t * const RAILWAY_CONFIG_DATA = &temp_config_data;

