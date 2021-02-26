/**
 * @file:  cpr_image_target.c
 * @brief: image-specific and target-specific CPR functions.
 *
 * Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 *
 * $DateTime: 2018/07/30 01:21:36 $
 * $Header: //components/rel/core.qdsp6/2.1.c4/power/cpr/image/cdsp/target/670/cpr_image_target.c#1 $
 * $Change: 16759227 $
 */
#include "cpr_image_target.h"
#include "cpr_data.h"
#include "cpr_cfg.h"
#include "cpr_utils.h"
#include "DDIChipInfo.h"
#include "voltage_level.h"
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

/**
*
* <!-- cpr_image_target_get_ldo_voltage -->
*
*/
uint32 cpr_image_target_get_ldo_voltage(cpr_domain_info* info)
{
    cpr_submode_settings* modeState;
    uint32 voltage_uv;

    cpr_utils_get_mode_settings( CPR_RAIL_MSS_LDO, info, NULL, &modeState );

    /*
     * TODO: get correct magic formula from PTE
     */
    voltage_uv = modeState->ceiling;

    switch(info->u.mode)
    {
        case CPR_VOLTAGE_MODE_MIN_SVS:
            voltage_uv -= 60000;
            break;

        case CPR_VOLTAGE_MODE_LOW_SVS:
            voltage_uv -= 20000;
            break;

        case CPR_VOLTAGE_MODE_SVS:
            voltage_uv -= 90000;
            break;

        default:
            /*
             * not supported
             */
            CPR_LOG_FATAL("Unsupported mode for eLDO voltage: %d", info->u.mode);
    }

    CPR_LOG_TRACE("Calculated eLDO voltage for mode %d : %d uv", info->u.mode, voltage_uv);

    return voltage_uv;
}

/**
*
* <!-- cpr_image_target_get_foundry -->
*
* @brief Get the foundry and return a foundry enum that our CPR driver understands
*
*/
cpr_foundry_id cpr_image_target_get_foundry(void)
{
    DalChipInfoFamilyType familyVersion;
    cpr_foundry_id cpr_foundry = CPR_FOUNDRY_TSMC;

    CPR_ASSERT(familyVersion = DalChipInfo_ChipFamily());

    if (familyVersion == DALCHIPINFO_FAMILY_SDM670)
    {
        cpr_foundry = CPR_FOUNDRY_SS;
    }

    return cpr_foundry;
}

/**
*
* <!-- cpr_image_target_convert_vlvl_to_cpr -->
*
* @brief Converts vlvl corner enum to CPR voltage mode enum
*
*/
cpr_voltage_mode cpr_image_target_vlvl_corner_to_cpr_mode(rail_voltage_level corner)
{
    cpr_voltage_mode mode = CPR_VOLTAGE_MODE_OFF;

    switch(corner)
    {
        case RAIL_VOLTAGE_LEVEL_OFF      : mode = CPR_VOLTAGE_MODE_OFF;        break;
        case RAIL_VOLTAGE_LEVEL_RET      : mode = CPR_VOLTAGE_MODE_RETENTION;  break;
        case RAIL_VOLTAGE_LEVEL_MIN_SVS  : mode = CPR_VOLTAGE_MODE_MIN_SVS;    break;
        case RAIL_VOLTAGE_LEVEL_LOW_SVS  : mode = CPR_VOLTAGE_MODE_LOW_SVS;    break;
        case RAIL_VOLTAGE_LEVEL_SVS      : mode = CPR_VOLTAGE_MODE_SVS;        break;
        case RAIL_VOLTAGE_LEVEL_SVS_L1   : mode = CPR_VOLTAGE_MODE_SVS_L1;     break;
        case RAIL_VOLTAGE_LEVEL_NOM      : mode = CPR_VOLTAGE_MODE_NOMINAL;    break;
        case RAIL_VOLTAGE_LEVEL_NOM_L1   : mode = CPR_VOLTAGE_MODE_NOMINAL_L1; break;
        case RAIL_VOLTAGE_LEVEL_TUR      : mode = CPR_VOLTAGE_MODE_TURBO;      break;
        default: CPR_LOG_FATAL("Unsupported vlvl corner %u", corner);          break;
    }

    return mode;
}

/**
*
* <!-- cpr_image_target_convert_vcs_to_cpr -->
*
* @brief Converts CPR voltage mode enum to vlvl corner enum
*
*/
rail_voltage_level cpr_image_target_cpr_mode_to_vlvl_corner(cpr_voltage_mode mode)
{
    rail_voltage_level corner = RAIL_VOLTAGE_LEVEL_OFF;

    switch(mode)
    {
        case CPR_VOLTAGE_MODE_OFF:        corner = RAIL_VOLTAGE_LEVEL_OFF;      break;
        case CPR_VOLTAGE_MODE_RETENTION:  corner = RAIL_VOLTAGE_LEVEL_RET;      break;
        case CPR_VOLTAGE_MODE_MIN_SVS:    corner = RAIL_VOLTAGE_LEVEL_MIN_SVS;  break;
        case CPR_VOLTAGE_MODE_LOW_SVS:    corner = RAIL_VOLTAGE_LEVEL_LOW_SVS;  break;
        case CPR_VOLTAGE_MODE_SVS:        corner = RAIL_VOLTAGE_LEVEL_SVS;      break;
        case CPR_VOLTAGE_MODE_SVS_L1:     corner = RAIL_VOLTAGE_LEVEL_SVS_L1;   break;
        case CPR_VOLTAGE_MODE_NOMINAL:    corner = RAIL_VOLTAGE_LEVEL_NOM;      break;
        case CPR_VOLTAGE_MODE_NOMINAL_L1: corner = RAIL_VOLTAGE_LEVEL_NOM_L1;   break;
        case CPR_VOLTAGE_MODE_TURBO:      corner = RAIL_VOLTAGE_LEVEL_TUR;      break;
        default: CPR_LOG_FATAL("Unsupported CPR mode %u", mode); break;
    }

    return corner;
}

