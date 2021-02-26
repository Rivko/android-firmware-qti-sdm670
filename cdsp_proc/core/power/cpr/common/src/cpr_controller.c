/**
 * @file:  cpr_controller.c
 *
 * Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 *
 * $DateTime: 2018/07/30 01:21:36 $
 * $Header: //components/rel/core.qdsp6/2.1.c4/power/cpr/common/src/cpr_controller.c#1 $
 * $Change: 16759227 $
 */

#include "cpr_controller.h"
#include "cpr_logs.h"
#include "cpr_hal.h"
#include "cpr_image.h"

/*
 * Initialize CPR controller for the common config among threads/rails
 */
void cpr_controller_init(cpr_controller* controller)
{
    CPR_LOG_TRACE( "Initializing controller @ 0x%x", (uint32)controller->hal.base );

    // Enable the clocks
    if(controller->ahbClk != NULL) {
        cpr_image_enable_clock(controller->ahbClk);
    }

    if(controller->refClk != NULL) {
        cpr_image_enable_clock(controller->refClk);
    }

    cpr_hal_configure_controller( &controller->hal, controller->stepQuotMin, controller->stepQuotMax );
    cpr_hal_bypass_sensors( &controller->hal, controller->bypassSensors, controller->bypassSensorsCount, true );
    cpr_hal_disable_sensors( &controller->hal, controller->disableSensors, controller->disableSensorsCount );
}
