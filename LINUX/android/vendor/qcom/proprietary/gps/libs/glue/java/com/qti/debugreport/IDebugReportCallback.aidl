/* ======================================================================
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.debugreport;

import android.os.Bundle;

oneway interface IDebugReportCallback {
    void onDebugDataAvailable(in Bundle debugReport);
}
