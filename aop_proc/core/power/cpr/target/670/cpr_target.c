/******************************************************************
 *
 * @file: cpr_target.c
 * @brief: Target specific CPR driver functionality.
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 *****************************************************************/

#include "cpr_types.h"
#include "cpr_hwio.h"
#include "cprf_hwio.h"
#include "cpr_internal.h"
#include "cpr_target.h"
#include "cprf.h"
#include "cpr_smem.h"
#include "cpr_image.h"
#include "cpr_internal.h"
#include "rpmh_arc.h"

boolean cpr_closed_loop[HWIO_RPMH_CPRF_CPRFm_CTRL_MAXm+1] =
//      { CX  ,  MX  , VDDA, LPI_CX, LPI_MX, GFX,  MSS }
        {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};

cpr_return_status cpr_target_update_rail_settings(cpr_rail_state *state,
        uint8 cprf_id, uint32 base, uint32 thread)
{
    int mCount, smCount, tCount;
    cpr_mode_settings *ms;
    cpr_submode_settings *sms = NULL;
    cpr_quotient *tgt;

    for(mCount = 0; mCount < state->modeSettingsCount; mCount++) {
        for(smCount = 0; smCount < state->modeSettings[mCount].subModesCount;
                smCount++) {
            ms = &(state->modeSettings[mCount]);
            sms = &(ms->subModes[smCount]);
            //ToDo: Need a better way to co-relate mode to hlvl.
            cpr_target_set_submode(cprf_id, mCount+2, sms->ceiling, sms->floor);
        }
        for(tCount = 0; tCount < state->modeSettings[mCount].targetsCount;
                tCount++) {
            ms = &(state->modeSettings[mCount]);
            tgt = &(ms->targets[tCount]);
            cpr_target_set_target_quotient(base, thread, mCount+2, tgt->ro,
                    tgt->quotient);
        }
    }

    //To fix Klockworks err, check to make sure sms is not NULL.
    CORE_VERIFY(sms);

    //Set VMAX, VMIN of modes higher than 'highest supported mode'
    //to the same value as VMAX and VMIN of the 'highest supported mode'.
    for(mCount+=2; mCount<16; mCount++)
        cpr_target_set_submode(cprf_id, mCount, sms->ceiling, sms->floor);

    return CPR_RETURN_SUCCESS;
}

cpr_return_status cpr_target_free_rail_settings(cpr_rail_state *state)
{
    int mCount;
    cpr_mode_settings *ms;
    for(mCount = 0; mCount < state->modeSettingsCount; mCount++) {
        ms = &(state->modeSettings[mCount]);
        if (ms->targets)
            cpr_image_free(ms->targets);
        if (ms->subModes)
            cpr_image_free(ms->subModes);
    }

    if (state->modeSettings)
        cpr_image_free(state->modeSettings);

    return CPR_RETURN_SUCCESS;
}

cpr_return_status cpr_target_set_closed_loop(uint8 id, boolean isClosedLoop)
{
    if (id > HWIO_RPMH_CPRF_CPRFm_CTRL_MAXm)
        return CPR_RETURN_ERROR;
    cpr_closed_loop[id] = isClosedLoop;

    return CPR_RETURN_SUCCESS;
}

boolean cpr_target_is_closed_loop(uint8 id)
{
    if (id > HWIO_RPMH_CPRF_CPRFm_CTRL_MAXm)
        return CPR_RETURN_ERROR;
    return (cpr_closed_loop[id] == TRUE);
}

cpr_return_status cpr_target_cprf_ctrl_intr_en(uint8 id, boolean enable,
                                               boolean force)
{
    if (!force && !cpr_closed_loop[id])
       return CPR_RETURN_SUCCESS;

    HWIO_RPMH_CPRF_CPRFm_CTRL_OUTMI(id,
       HWIO_RPMH_CPRF_CPRFm_CTRL_CTRL_INTERACTION_EN_BMSK,
       (enable << HWIO_RPMH_CPRF_CPRFm_CTRL_CTRL_INTERACTION_EN_SHFT));

    return CPR_RETURN_SUCCESS;
}

cpr_return_status cpr_target_cprc_loop_en(uint32 base, boolean enable)
{
    CPR_HWIO_OUT_FIELD(HWIO_CPR_FSM_CTL_ADDR(base),
       HWIO_CPR_FSM_CTL_LOOP_EN, enable);
    return CPR_RETURN_SUCCESS;
}

cpr_return_status cpr_target_enable_cprf_interface_en(uint8 id, boolean enable)
{
    uint32 base = 0x0;

    if (id == RPMH_ARC_VDD_CX)
        base = 0x60628000;
    else if (id == RPMH_ARC_VDD_MX)
        base = 0x6062c000;
    else if (id == RPMH_ARC_VDD_GFX)
        base = 0x6509a000;

    if (!base)
        return CPR_RETURN_SUCCESS;

    CPR_HWIO_OUT_FIELD(HWIO_CPR_MISC_REGISTER_ADDR(base),
       HWIO_CPR_MISC_REGISTER_CPRF_INTERFACE_ENABLE, enable);

    return CPR_RETURN_SUCCESS;
}

uint32 cpr_target_get_ret_voltage (uint8 id)
{
    if (id > HWIO_RPMH_CPRF_CPRFm_CTRL_MAXm)
        return 0;

    return HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_INI2(id, 1);
}

cpr_return_status cpr_target_set_ret_voltage (uint8 id, uint32 vol)
{
    if (id > HWIO_RPMH_CPRF_CPRFm_CTRL_MAXm)
        return CPR_RETURN_ERROR;

    HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_OUTMI2(id, 1,
            HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_VMAX_BMSK,
            vol << HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_VMAX_SHFT);

    HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_OUTMI2(id, 1,
            HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_VMIN_BMSK,
            vol << HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_VMIN_SHFT);

    HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_OUTMI2(id, 1,
            HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_VSAFE_BMSK,
            vol << HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_VSAFE_SHFT);

    return CPR_RETURN_SUCCESS;
}

cpr_return_status cpr_target_set_target_quotient(uint32 base, uint8 thread,
        uint8 mode, uint32 ro, uint32 quotient)
{
    uint32 targetOffset = (HWIO_CPR_TARGET1_t_m_ADDR(0, 0, 0) -
            HWIO_CPR_TARGET0_t_m_ADDR(0, 0, 0)) * ro;

    CPR_HWIO_OUT_FIELD(HWIO_CPR_TARGET0_t_m_ADDR(base, thread, mode)
            + targetOffset, HWIO_CPR_TARGET0_t_m_TARGET0, quotient);

    return CPR_RETURN_SUCCESS;
}

cpr_return_status cpr_target_set_submode(uint8 cprf_id, uint8 hlvl,
        uint32 ceiling, uint32 floor)
{
    ceiling/=1000;
    floor/=1000;
    HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_OUTMI2(cprf_id, hlvl,
        HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_VMAX_BMSK,
        ceiling << HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_VMAX_SHFT);
    HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_OUTMI2(cprf_id, hlvl,
        HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_VMIN_BMSK,
        floor << HWIO_RPMH_CPRF_CPRFm_VOLTAGE_LIMITS_n_VMIN_SHFT);

    HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_OUTMI2(cprf_id, hlvl,
        HWIO_RPMH_CPRF_CPRFm_SAFE_VOLTAGE_n_VSAFE_BMSK, ceiling);

    return CPR_RETURN_SUCCESS;
}
