/******************************************************************
 *
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * THIS IS AUTO-GENERATED CPRF and SMEM INIT FILE FOR 670.
 *
 * DateTime: 2020/01/13 01:53:12
 *
 *****************************************************************/

#include "cpr_types.h"
#include "cpr_smem.h"
#include "cpr_image.h"
#include "cmd_db.h"
#include "cprf_hwio.h"
#include "cpr_hwio.h"
#include "cpr_internal.h"
#include "cpr_target.h"
#include "cprf.h"



void cpr_target_init(void)
{

    {
        /********************************************/
        /* Get VP info from Shared Memory for cx*/
        /********************************************/
        cpr_rail cx_rail;
        cpr_rail_state cx_state;
        cx_rail.id = CPR_RAIL_CX;

        cx_state.cMode = CPR_CONTROL_NONE;
        cx_state.modeSettingsCount = 0;
        cx_state.modeSettings = 0x0;
        cx_state.activeMode = 0x0;
        cx_state.previousMode = 0x0;

        cpr_smem_deserialize_config(&cx_rail, &cx_state);

        if (cx_state.cMode == CPR_CONTROL_SW_CLOSED_LOOP ||
                cx_state.cMode == CPR_CONTROL_HW_CLOSED_LOOP)
            cpr_target_set_closed_loop(0, TRUE);
 
        if (cx_state.modeSettingsCount) {
            cpr_target_update_rail_settings(&cx_state, 0, 0x60628000, 0);
            cpr_target_free_rail_settings(&cx_state);
        }
    }

    {
        /*******************************/
        /* Enable Open Loop for cx */
        /*******************************/
        uint32 cx_vrm = cmd_db_query_addr("vrm.cx");
        uint32 cx_vrm_settings = 0;

        cx_vrm_settings |= (cx_vrm & 0xFFFF) <<
            HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_VOLTAGE_ADDRESS_OFFSET_SHFT;
        cx_vrm_settings |= ((cx_vrm & 0xFF00) >> 8) <<
            HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_RESOURCE_ID_SHFT;

        HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_OUTI(0,
            cx_vrm_settings);

        HWIO_RPMH_CPRF_CPRFm_ENABLE_OUTI(0, 1);
    }

    {
        /*********************************/
        /* Enable Closed Loop for cx */
        /*********************************/
        cpr_target_cprf_ctrl_intr_en(0,
            cpr_target_is_closed_loop(0), false);
        cpr_target_cprc_loop_en(0x60628000, cpr_target_is_closed_loop(0));
    }

    {
        /*******************************/
        /* Enable Open Loop for gfx */
        /*******************************/
        uint32 gfx_vrm = cmd_db_query_addr("vrm.gfx");
        uint32 gfx_vrm_settings = 0;

        gfx_vrm_settings |= (gfx_vrm & 0xFFFF) <<
            HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_VOLTAGE_ADDRESS_OFFSET_SHFT;
        gfx_vrm_settings |= ((gfx_vrm & 0xFF00) >> 8) <<
            HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_RESOURCE_ID_SHFT;

        HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_OUTI(5,
            gfx_vrm_settings);

        HWIO_RPMH_CPRF_CPRFm_ENABLE_OUTI(5, 1);
    }

    {
        /*******************************/
        /* Enable Open Loop for mss */
        /*******************************/
        uint32 mss_vrm = cmd_db_query_addr("vrm.mss");
        uint32 mss_vrm_settings = 0;

        mss_vrm_settings |= (mss_vrm & 0xFFFF) <<
            HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_VOLTAGE_ADDRESS_OFFSET_SHFT;
        mss_vrm_settings |= ((mss_vrm & 0xFF00) >> 8) <<
            HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_RESOURCE_ID_SHFT;

        HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_OUTI(6,
            mss_vrm_settings);

        HWIO_RPMH_CPRF_CPRFm_ENABLE_OUTI(6, 1);
    }

    {
        /********************************************/
        /* Get VP info from Shared Memory for mx*/
        /********************************************/
        cpr_rail mx_rail;
        cpr_rail_state mx_state;
        mx_rail.id = CPR_RAIL_MX;

        mx_state.cMode = CPR_CONTROL_NONE;
        mx_state.modeSettingsCount = 0;
        mx_state.modeSettings = 0x0;
        mx_state.activeMode = 0x0;
        mx_state.previousMode = 0x0;

        cpr_smem_deserialize_config(&mx_rail, &mx_state);

        if (mx_state.cMode == CPR_CONTROL_SW_CLOSED_LOOP ||
                mx_state.cMode == CPR_CONTROL_HW_CLOSED_LOOP)
            cpr_target_set_closed_loop(1, TRUE);
 
        if (mx_state.modeSettingsCount) {
            cpr_target_update_rail_settings(&mx_state, 1, 0x6062c000, 0);
            cpr_target_free_rail_settings(&mx_state);
        }
    }

    {
        /*******************************/
        /* Enable Open Loop for mx */
        /*******************************/
        uint32 mx_vrm = cmd_db_query_addr("vrm.mx");
        uint32 mx_vrm_settings = 0;

        mx_vrm_settings |= (mx_vrm & 0xFFFF) <<
            HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_VOLTAGE_ADDRESS_OFFSET_SHFT;
        mx_vrm_settings |= ((mx_vrm & 0xFF00) >> 8) <<
            HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_RESOURCE_ID_SHFT;

        HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_OUTI(1,
            mx_vrm_settings);

        HWIO_RPMH_CPRF_CPRFm_ENABLE_OUTI(1, 1);
    }

    {
        /*********************************/
        /* Enable Closed Loop for mx */
        /*********************************/
        cpr_target_cprf_ctrl_intr_en(1,
            cpr_target_is_closed_loop(1), false);
        cpr_target_cprc_loop_en(0x6062c000, cpr_target_is_closed_loop(1));
    }

    {
        /*******************************/
        /* Enable Open Loop for ebi */
        /*******************************/
        uint32 ebi_vrm = cmd_db_query_addr("vrm.ebi");
        uint32 ebi_vrm_settings = 0;

        ebi_vrm_settings |= (ebi_vrm & 0xFFFF) <<
            HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_VOLTAGE_ADDRESS_OFFSET_SHFT;
        ebi_vrm_settings |= ((ebi_vrm & 0xFF00) >> 8) <<
            HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_RESOURCE_ID_SHFT;

        HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_OUTI(2,
            ebi_vrm_settings);

        HWIO_RPMH_CPRF_CPRFm_ENABLE_OUTI(2, 1);
    }

    {
        /********************************************/
        /* Get VP info from Shared Memory for lpi_cx*/
        /********************************************/
        cpr_rail lpi_cx_rail;
        cpr_rail_state lpi_cx_state;
        lpi_cx_rail.id = CPR_RAIL_LPI_CX;

        lpi_cx_state.cMode = CPR_CONTROL_NONE;
        lpi_cx_state.modeSettingsCount = 0;
        lpi_cx_state.modeSettings = 0x0;
        lpi_cx_state.activeMode = 0x0;
        lpi_cx_state.previousMode = 0x0;

        cpr_smem_deserialize_config(&lpi_cx_rail, &lpi_cx_state);

        if (lpi_cx_state.cMode == CPR_CONTROL_SW_CLOSED_LOOP ||
                lpi_cx_state.cMode == CPR_CONTROL_HW_CLOSED_LOOP)
            cpr_target_set_closed_loop(3, TRUE);
 
        if (lpi_cx_state.modeSettingsCount) {
            cpr_target_update_rail_settings(&lpi_cx_state, 3, 0x6062c000, 2);
            cpr_target_free_rail_settings(&lpi_cx_state);
        }
    }

    {
        /*******************************/
        /* Enable Open Loop for lcx */
        /*******************************/
        uint32 lcx_vrm = cmd_db_query_addr("vrm.lcx");
        uint32 lcx_vrm_settings = 0;

        lcx_vrm_settings |= (lcx_vrm & 0xFFFF) <<
            HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_VOLTAGE_ADDRESS_OFFSET_SHFT;
        lcx_vrm_settings |= ((lcx_vrm & 0xFF00) >> 8) <<
            HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_RESOURCE_ID_SHFT;

        HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_OUTI(3,
            lcx_vrm_settings);

        HWIO_RPMH_CPRF_CPRFm_ENABLE_OUTI(3, 1);
    }

    {
        /********************************************/
        /* Get VP info from Shared Memory for lpi_mx*/
        /********************************************/
        cpr_rail lpi_mx_rail;
        cpr_rail_state lpi_mx_state;
        lpi_mx_rail.id = CPR_RAIL_LPI_MX;

        lpi_mx_state.cMode = CPR_CONTROL_NONE;
        lpi_mx_state.modeSettingsCount = 0;
        lpi_mx_state.modeSettings = 0x0;
        lpi_mx_state.activeMode = 0x0;
        lpi_mx_state.previousMode = 0x0;

        cpr_smem_deserialize_config(&lpi_mx_rail, &lpi_mx_state);

        if (lpi_mx_state.cMode == CPR_CONTROL_SW_CLOSED_LOOP ||
                lpi_mx_state.cMode == CPR_CONTROL_HW_CLOSED_LOOP)
            cpr_target_set_closed_loop(4, TRUE);
 
        if (lpi_mx_state.modeSettingsCount) {
            cpr_target_update_rail_settings(&lpi_mx_state, 4, 0x6062c000, 3);
            cpr_target_free_rail_settings(&lpi_mx_state);
        }
    }

    {
        /*******************************/
        /* Enable Open Loop for lmx */
        /*******************************/
        uint32 lmx_vrm = cmd_db_query_addr("vrm.lmx");
        uint32 lmx_vrm_settings = 0;

        lmx_vrm_settings |= (lmx_vrm & 0xFFFF) <<
            HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_VOLTAGE_ADDRESS_OFFSET_SHFT;
        lmx_vrm_settings |= ((lmx_vrm & 0xFF00) >> 8) <<
            HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_RESOURCE_ID_SHFT;

        HWIO_RPMH_CPRF_CPRFm_VRM_SETTINGS_OUTI(4,
            lmx_vrm_settings);

        HWIO_RPMH_CPRF_CPRFm_ENABLE_OUTI(4, 1);
    }

}
