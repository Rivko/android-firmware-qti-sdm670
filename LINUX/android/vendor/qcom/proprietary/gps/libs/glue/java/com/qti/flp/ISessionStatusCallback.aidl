/*  Copyright (c) 2017 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.flp;

import android.location.Location;

oneway interface ISessionStatusCallback
{
    void onBatchingStatusCb(in int status);
}
