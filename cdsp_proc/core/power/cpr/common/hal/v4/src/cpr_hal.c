/**
 * @file:  cpr_hal.c
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2018/07/30 01:21:36 $
 * $Header: //components/rel/core.qdsp6/2.1.c4/power/cpr/common/hal/v4/src/cpr_hal.c#1 $
 * $Change: 16759227 $
 *
 */
#include "../../v3/src/cpr_hal.c"

static void cpr_hal_write_kv_margin_adj_en(cpr_hal_handle* hdl, boolean enable)
{
    cpr_hal_controller *controller = hdl->controller;
    // Enable/Disable the feature
    CPR_HWIO_OUT_FIELD(HWIO_CPR_MARGIN_ADJ_CTL_ADDR(controller->base), HWIO_CPR_MARGIN_ADJ_CTL_KV_MARGIN_ADJ_EN, enable);
    controller->marginAdjEnabled = CPR_HWIO_IN_FIELD(HWIO_CPR_MARGIN_ADJ_CTL_ADDR( controller->base ), HWIO_CPR_MARGIN_ADJ_CTL_KV_MARGIN_ADJ_EN);
}

static void cpr_hal_write_margin_temp_bands(cpr_hal_handle* hdl, uint8 modeIndex, const cpr_voltage_plan* vp) 
{
/* ToDo: Configure Temp Bands. To read Temp Data.
    const uint8 *tempBands = vp->modes[modeIndex].marginTempBands;
    uint32 dataValue;


    CPR_LOG_INFO( "Setting SDELTA Table" );

    dataValue = CPR_HWIO_SET_FIELD_VALUE(tempBands[0], HWIO_CPR_MARGIN_TEMP_0_MARGIN_TEMPBAND0) |
                CPR_HWIO_SET_FIELD_VALUE(tempBands[1], HWIO_CPR_MARGIN_TEMP_0_MARGIN_TEMPBAND1) |
                CPR_HWIO_SET_FIELD_VALUE(tempBands[2], HWIO_CPR_MARGIN_TEMP_0_MARGIN_TEMPBAND2) |
                CPR_HWIO_SET_FIELD_VALUE(tempBands[3], HWIO_CPR_MARGIN_TEMP_0_MARGIN_TEMPBAND3);

    CPR_HWIO_OUT( HWIO_CPR_MARGIN_TEMP_0_ADDR(hdl->controller->base), dataValue );

    cpr_hal_read_temp_sensor_data(hdl);
*/
}

void cpr_hal_enable_temp_margin_adj(cpr_hal_handle* hdl, const cpr_voltage_plan* vp, boolean enable )
{
    if (vp->tempAdjCfg == NULL)
    {
        return;
    }

    cpr_hal_write_kv_margin_adj_en(hdl, enable);
}

void cpr_hal_set_temp_margin(cpr_hal_handle* hdl, uint8 modeIndex, const cpr_voltage_plan* vp)
{
    // If temperature adjustment is not enabled
    if (vp->tempAdjCfg == NULL)
    {
        CPR_LOG_INFO( "Temperature Adjustment is not enabled, exiting temperature adjustment init" );
        return;
    }
    // Write the new SDELTA table for the new corner
    cpr_hal_write_margin_temp_bands(hdl, modeIndex, vp);
}

void cpr_hal_init_temp_margin_adj(cpr_hal_handle* hdl, uint8 modeIndex, const cpr_voltage_plan* vp)
{
    // If temperature adjustment is not enabled
    if (vp->tempAdjCfg == NULL)
    {
        // By HPG specification. If temperature adjustment is not enabled then set 0 to kvMarginAdjEn
        CPR_LOG_INFO( "Temperature Adjustment is not enabled, exiting temperature adjustment init" );
        cpr_hal_write_kv_margin_adj_en(hdl, false);
        return;
    }

    const uintptr_t         base   = hdl->controller->base;
    const cpr_temp_adj_cfg *cfg    = vp->tempAdjCfg;
    const uint8            *points = cfg->tempPoints;

    // Write the CPR MISC Register which contains the temp sensor id start and end
    CPR_HWIO_OUT_FIELD(HWIO_CPR_MISC_REGISTER_ADDR(base), HWIO_CPR_MISC_REGISTER_TEMP_SENSOR_ID_START, cfg->tempSensorStartId);
    CPR_HWIO_OUT_FIELD(HWIO_CPR_MISC_REGISTER_ADDR(base), HWIO_CPR_MISC_REGISTER_TEMP_SENSOR_ID_END, cfg->tempSensorEndId);

    // Write the point 0, 1, 2 temperature thresholds
    CPR_HWIO_OUT_FIELD(HWIO_CPR_MARGIN_TEMP_POINT0N1_ADDR(base), HWIO_CPR_MARGIN_TEMP_POINT0N1_POINT0, points[0] * 10);
    CPR_HWIO_OUT_FIELD(HWIO_CPR_MARGIN_TEMP_POINT0N1_ADDR(base), HWIO_CPR_MARGIN_TEMP_POINT0N1_POINT1, points[1] * 10);
    CPR_HWIO_OUT_FIELD(HWIO_CPR_MARGIN_TEMP_POINT2_ADDR(base)  , HWIO_CPR_MARGIN_TEMP_POINT2_POINT2  , points[2] * 10);

    cpr_hal_write_margin_temp_bands(hdl, modeIndex, vp);

#if 0 /* Disable debug feature to save memory */
    // DEBUGGING PURPOSES ONLY
    // Used to allow data to be given to the debug_bus_select registers
    // Also allows user to specify the step quotent used for the feature instead of what HW is programmed to do.
    // See CPR HPG 12.20-12.22
    if (cfg->perRoKvMarginEn)
    {
        CPR_HWIO_OUT_FIELD(HWIO_CPR_MARGIN_ADJ_CTL_ADDR(base), HWIO_CPR_MARGIN_ADJ_CTL_KV_MARGIN_ADJ_SINGLE_STEP_QUOT, cfg->singleStepQuot);
        CPR_HWIO_OUT_FIELD(HWIO_CPR_MARGIN_ADJ_CTL_ADDR(base), HWIO_CPR_MARGIN_ADJ_CTL_PER_RO_KV_MARGIN_EN, cfg->perRoKvMarginEn);
    }
#endif
}

void cpr_hal_read_temp_sensor_data(cpr_hal_handle* hdl)
{
    cpr_hal_controller *controller = hdl->controller;
    *(uint32*)&controller->tempSensorData = CPR_HWIO_IN( HWIO_CPR_TEMP_SENSOR_DATA_ADDR( controller->base) );
}

