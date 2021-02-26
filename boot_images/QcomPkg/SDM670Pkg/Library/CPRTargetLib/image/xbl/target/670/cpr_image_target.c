/**
 * @file:  cpr_image_target.c
 *
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2018/07/19 03:25:26 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/CPRTargetLib/image/xbl/target/670/cpr_image_target.c#3 $
 * $Change: 16679875 $
 */
#include "cpr_image_target.h"
#include "cpr_logs.h"
#include "pm_smps.h"
#include "cpr_utils.h"
#include "cpr.h"
static uint32 cpr_pte_qc_spare_fuse;

const cpr_settings* cpr_image_target_get_settings(void)
{
    extern const cpr_settings cpr_settings_670;

    return &cpr_settings_670;
}

boolean cpr_image_target_is_turbo_mode_only(void)
{
    /*
     * CR 1050500:
     * TURBO_MODE_ONLY fuse: QC Spare 16[45] - bit 13 of 0x780324/0x784324
     */
    cpr_pte_qc_spare_fuse = CPR_HWIO_IN(SECURITY_CONTROL_CORE_REG_BASE + 0x004324);

    return ((cpr_pte_qc_spare_fuse & (1 << 13)) != 0);
}

cpr_domain_id cpr_image_target_railway_rail_to_cpr_domain(int railway_rail)
{
    switch(railway_rail)
    {
        case 0: return CPR_RAIL_MX;
        case 1: return CPR_RAIL_CX;
        case 2: return CPR_RAIL_VDDA;
        case 3: return CPR_RAIL_LPI_MX;
        case 4: return CPR_RAIL_LPI_CX;
        case 5: return CPR_RAIL_WLAN;
        default:
            CPR_LOG_FATAL("Unsupported rail %u", railway_rail);
            break;
    }

    CPR_ASSERT(0);

    return (cpr_domain_id)0;
}

railway_corner cpr_image_target_cpr_mode_to_railway_corner(cpr_voltage_mode mode)
{
    railway_corner map[CPR_VOLTAGE_MODE_COUNT] = {
        [CPR_VOLTAGE_MODE_OFF                ] = RAILWAY_NO_REQUEST,
        [CPR_VOLTAGE_MODE_RETENTION          ] = RAILWAY_RETENTION,
        [CPR_VOLTAGE_MODE_MIN_SVS            ] = RAILWAY_SVS_MIN,
        [CPR_VOLTAGE_MODE_LOW_SVS            ] = RAILWAY_SVS_LOW,
        [CPR_VOLTAGE_MODE_SVS                ] = RAILWAY_SVS,
        [CPR_VOLTAGE_MODE_SVS_L1             ] = RAILWAY_SVS_HIGH,
        [CPR_VOLTAGE_MODE_NOMINAL            ] = RAILWAY_NOMINAL,
        [CPR_VOLTAGE_MODE_NOMINAL_L1         ] = RAILWAY_TURBO,
        [CPR_VOLTAGE_MODE_TURBO              ] = RAILWAY_TURBO,
        [CPR_VOLTAGE_MODE_TURBO_L1           ] = RAILWAY_TURBO_L1,
        [CPR_VOLTAGE_MODE_SUPER_TURBO        ] = RAILWAY_SUPER_TURBO,
        [CPR_VOLTAGE_MODE_SUPER_TURBO_NO_CPR ] = RAILWAY_SUPER_TURBO_NO_CPR,
    };

    return (mode < CPR_VOLTAGE_MODE_COUNT) ? map[mode] : map[CPR_VOLTAGE_MODE_COUNT - 1];
}

cpr_voltage_mode cpr_image_target_railway_corner_to_cpr_mode(railway_corner corner)
{
    cpr_voltage_mode map[RAILWAY_CORNERS_COUNT] = {
        [RAILWAY_NO_REQUEST        ] = CPR_VOLTAGE_MODE_OFF,
        [RAILWAY_RETENTION         ] = CPR_VOLTAGE_MODE_RETENTION,
        [RAILWAY_SVS_MIN           ] = CPR_VOLTAGE_MODE_MIN_SVS,
        [RAILWAY_SVS_LOW           ] = CPR_VOLTAGE_MODE_LOW_SVS,
        [RAILWAY_SVS               ] = CPR_VOLTAGE_MODE_SVS,
        [RAILWAY_SVS_HIGH          ] = CPR_VOLTAGE_MODE_SVS_L1,
        [RAILWAY_NOMINAL           ] = CPR_VOLTAGE_MODE_NOMINAL,
        [RAILWAY_TURBO             ] = CPR_VOLTAGE_MODE_TURBO,
        [RAILWAY_TURBO_L1          ] = CPR_VOLTAGE_MODE_TURBO_L1,
        [RAILWAY_SUPER_TURBO       ] = CPR_VOLTAGE_MODE_SUPER_TURBO,
        [RAILWAY_SUPER_TURBO_NO_CPR] = CPR_VOLTAGE_MODE_SUPER_TURBO_NO_CPR,
    };

    return (corner < RAILWAY_CORNERS_COUNT) ? map[corner] : map[RAILWAY_CORNERS_COUNT - 1];
}

void cpr_image_target_enable_measurements(cpr_rail* rail, cpr_measurement_type type)
{
    /* TODO: set rail to PWM */
}

void cpr_image_target_disable_measurements(cpr_rail* rail, cpr_measurement_type type)
{
    /* TODO: reset rail */
}

uint32 volt_round_up(uint32 aVal, uint32 multiplier)
{
    if(aVal % multiplier == 0)
    {
        return aVal;
    }

    return aVal + multiplier - (aVal % multiplier);
}

cpr_fuse* apc0_turbo_fuse  = (cpr_fuse[]) { {.count = 1, .data = (struct raw_fuse_data[]) { CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW5_MSB, CPR10_TARG_VOLT_TUR) } } };
cpr_fuse* apc0_nom_fuse  = (cpr_fuse[]) { {.count = 2, .data = (struct raw_fuse_data[]) { CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW6_LSB, CPR10_TARG_VOLT_NOM_5_2), CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW5_MSB, CPR10_TARG_VOLT_NOM_1_0) } } };

vpp_voltage_settings_t apc0_setting[] = {
    /*                                               olv_        olv_fuse         olv_  step_              */
    /* corner,  freq,  ref_mV, ceil_mV, floor_mV, ceil_mv,       multipler, fuse_adjust  size,    olv_fuse*/
    {APC_NOM, 1516800,     872,     872,      632,      0,                8,          12,     4, &apc0_nom_fuse,  },
	{APC_TUR, 1708800,     952,    1000,      712,      0,                8,           8,     4, &apc0_turbo_fuse,},
} ;
	

void cpr_image_target_cust_init()
{
	
	uint32 no_supported_corners = sizeof (apc0_setting)/sizeof (apc0_setting[0]);
	for (int i = 0; i < no_supported_corners; i++)
	{
		apc0_setting[i].olv_ceil_mV = apc0_setting[i].olv_ref_mV 
									+ (uint16) (cpr_utils_decode_fuse_value( *apc0_setting[i].olv_fuse, 1, true )*apc0_setting[i].olv_fuse_multiplier)
									+ apc0_setting[i].olv_fuse_adjust;
	
		apc0_setting[i].olv_ceil_mV = MIN(apc0_setting[i].initial_ceil_mV, MAX(apc0_setting[i].olv_ceil_mV, apc0_setting[i].initial_floor_mV));
	
		// round up to multiple of pmic step size
		apc0_setting[i].olv_ceil_mV = volt_round_up( apc0_setting[i].olv_ceil_mV, apc0_setting[i].step_size); 
	
	}

}

void cpr_set_apc0_olv(uint32 corner)
{
	uint8 pmic_chip_id = 0;
	uint8 pmic_peripheral_index = PM_SMPS_1;

	cpr_image_target_cust_init();
	
	pm_smps_volt_level( pmic_chip_id, pmic_peripheral_index, (apc0_setting[corner].olv_ceil_mV*1000));
}