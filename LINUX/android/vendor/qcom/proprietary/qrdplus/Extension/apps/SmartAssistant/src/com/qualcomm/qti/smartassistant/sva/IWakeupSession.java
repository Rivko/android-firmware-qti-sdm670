/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.sva;

import android.content.Context;

public interface IWakeupSession {

    int establishSvaSession(String smName, Context context);

    int terminateSvaSession(String smName);

    void releaseAllSvaSessions();

    void addWakeupListener(IWakeupListener listener);

    void removeWakeupListener(IWakeupListener listener);

    int restartRecognition(int smHandle);

    SmSession querySessionInfo(String smName);

}
