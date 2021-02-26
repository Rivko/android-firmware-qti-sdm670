////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  QCameraHAL3Base.cpp
/// @brief Camera device Holder, Camera Test base
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOG_TAG "CameraTest"
#include "QCameraHAL3Base.h"
QCameraHAL3Base::QCameraHAL3Base(camera_module_t* module,int CameraId)
{
    mCameraId = CameraId;
    mModule = module;
    mDevice = new QCameraHAL3Device(mModule,CameraId);
}

QCameraHAL3Base::~QCameraHAL3Base()
{
    delete mDevice;
    mDevice = NULL;
}
