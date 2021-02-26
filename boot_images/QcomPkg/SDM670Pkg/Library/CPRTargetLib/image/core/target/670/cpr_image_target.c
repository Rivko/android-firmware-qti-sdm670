/**
 * @file:  cpr_image_target.c
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2018/07/19 03:25:26 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/CPRTargetLib/image/core/target/670/cpr_image_target.c#4 $
 * $Change: 16679875 $
 */
#include "cpr_image_target.h"
#include "cpr_image.h"
#include "cpr_logs.h"
#include "cpr_smem.h"
#include "voltage_level.h"

static uint32 cpr_pte_qc_spare_fuse;

const cpr_settings* cpr_image_target_get_settings(void)
{
    cpr_settings *cpr_settings = cpr_image_malloc(sizeof(cpr_settings));

    cpr_rail gfx_rail;
    gfx_rail.id = CPR_RAIL_GFX;

    cpr_smem_deserialize_settings(&gfx_rail, cpr_settings);

    return cpr_settings;
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

void cpr_image_target_railway_rail_to_arc_domain(cpr_domain_id railId, char **res_name)
{
    switch(railId)
    {
        case CPR_RAIL_MX 	: *res_name = "mx.lvl";    break;
        case CPR_RAIL_CX	: *res_name = "cx.lvl";    break;
        case CPR_RAIL_MSS	: *res_name = "mss.lvl";   break;
        case CPR_RAIL_GFX	: *res_name = "gfx.lvl";   break;
		case CPR_RAIL_LPI_MX: *res_name = "lmx.lvl";   break;
        case CPR_RAIL_LPI_CX: *res_name = "lcx.lvl";   break;
        default:
            CPR_LOG_FATAL("Unsupported rail %u", railId);
            break;
    }
	
	return;
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

rail_voltage_level cpr_image_target_cpr_mode_to_vlvl(cpr_voltage_mode mode)
{
    switch (mode){
	case CPR_VOLTAGE_MODE_OFF         :       return  RAIL_VOLTAGE_LEVEL_OFF;
	case CPR_VOLTAGE_MODE_RETENTION   :       return  RAIL_VOLTAGE_LEVEL_RETENTION;
	case CPR_VOLTAGE_MODE_MIN_SVS     :       return  RAIL_VOLTAGE_LEVEL_SVS_MIN;
	case CPR_VOLTAGE_MODE_LOW_SVS     :       return  RAIL_VOLTAGE_LEVEL_SVS_LOW;
	case CPR_VOLTAGE_MODE_SVS         :       return  RAIL_VOLTAGE_LEVEL_SVS;
	case CPR_VOLTAGE_MODE_SVS_L1      :       return  RAIL_VOLTAGE_LEVEL_SVS_L1;
	case CPR_VOLTAGE_MODE_NOMINAL     :       return  RAIL_VOLTAGE_LEVEL_NOM;
	case CPR_VOLTAGE_MODE_NOMINAL_L1  :       return  RAIL_VOLTAGE_LEVEL_NOM_L1;
	case CPR_VOLTAGE_MODE_TURBO       :       return  RAIL_VOLTAGE_LEVEL_TUR;
	case CPR_VOLTAGE_MODE_TURBO_L1    :       return  RAIL_VOLTAGE_LEVEL_TUR_L1;
    default:
            CPR_LOG_FATAL("Unsupported Voltage mode %u", mode);
			break;
	}
	
	CPR_ASSERT(0);
	return (rail_voltage_level)0;
}

void cpr_image_target_enable_measurements(cpr_rail* rail, cpr_measurement_type type)
{
    /* TODO: set rail to PWM */
}

void cpr_image_target_disable_measurements(cpr_rail* rail, cpr_measurement_type type)
{
    /* TODO: reset rail */
}


void cpr_set_apc0_olv(uint32 corner)
{
/* only called from XBL not in uefi */
}