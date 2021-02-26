/* ==============================================================================
 * IHIDEventListener.aidl
 *
 * Copyright (c) 2013 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 ============================================================================== */

package com.qualcomm.wfd.service;

oneway interface IHIDEventListener {

    void onHIDReprtDescRcv(in byte[] HIDRD);

    void onHIDReprtRcv(in byte[] HIDRep);

}
