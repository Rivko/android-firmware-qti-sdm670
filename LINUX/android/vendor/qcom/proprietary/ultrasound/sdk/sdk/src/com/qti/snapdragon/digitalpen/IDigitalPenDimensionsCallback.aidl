/*===========================================================================
                           IDigitalPenDimensionsCallback

Copyright (c) 2014, The Linux Foundation. All rights reserved.

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
package com.qti.snapdragon.digitalpen;

/**
 * Interface for DigitalPenOffScreenDisplayAdapter to
 * receive off-screen dimensions change callback.
 *
 * {@hide}
 */
oneway interface IDigitalPenDimensionsCallback {
    void onDimensionsChange(int width, int height);
}

