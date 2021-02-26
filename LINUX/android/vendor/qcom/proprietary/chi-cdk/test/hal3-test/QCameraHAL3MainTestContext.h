////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Copyright (c) 2016, The Linux Foundation. All rights reserved.*/

#ifndef __HAL3TEST_H__
#define __HAL3TEST_H__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
namespace qcamera {


typedef enum
{
    MENU_BASE = 0,
    MENU_START_PREVIEW,
    MENU_START_VIDEO,
    MENU_START_CAPTURE,
    MENU_START_RAW_CAPTURE,
    MENU_TOGGLE_IR_MODE,
    MENU_TOGGLE_SVHDR_MODE,
    MENU_TOGGLE_BINNING_CORRECTION,
    MENU_EXIT,
    MENU_INVALID
} menu_id;

typedef struct {
    menu_id base_menu;
    char *  basemenu_name;
} HAL3TEST_BASE_MENU_TBL_T;

typedef struct {
    const char * menu_name;
} HAL3TEST_SENSOR_MENU_TBL_T;

typedef struct {
    menu_id main_menu;
    const char * menu_name;
} CAMERA_BASE_MENU_TBL_T;

class CameraHAL3Base;
class MainTestContext
{
    bool mTestRunning;
    bool irmode;
    bool svhdrmode;
public:
    MainTestContext();
    int hal3appGetUserEvent();
    int hal3appDisplaySensorMenu(uint8_t );
    void hal3appDisplayCapabilityMenu();
    int hal3appDisplayPreviewMenu();
    int hal3appDisplayVideoMenu();
    void hal3appDisplayRawCaptureMenu();
    void hal3appDisplaySnapshotMenu();
    void hal3appDisplayExitMenu();
    int hal3appPrintMenu();
};

}

#endif
