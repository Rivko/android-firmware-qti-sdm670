/* ======================================================================
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.debugreport;

import java.com.qti.debugreport.IDebugReportCallback;
import android.os.Bundle;

interface IDebugReportService {

    void registerForDebugReporting (in IDebugReportCallback callback);
    void unregisterForDebugReporting (in IDebugReportCallback callback);

    void startReporting();
    void stopReporting();

    Bundle getDebugReport();
}
