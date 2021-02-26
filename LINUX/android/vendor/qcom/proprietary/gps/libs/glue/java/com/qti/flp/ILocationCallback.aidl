/*  Copyright (c) 2015 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.flp;

import android.location.Location;

oneway interface ILocationCallback
{
    void onLocationAvailable(in Location[] locations);
}
