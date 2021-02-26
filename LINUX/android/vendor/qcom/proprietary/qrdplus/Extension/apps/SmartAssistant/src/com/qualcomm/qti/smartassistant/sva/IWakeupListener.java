/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.sva;

import android.hardware.soundtrigger.SoundTrigger;

public interface IWakeupListener {

    void onRecognition(SoundTrigger.RecognitionEvent recognitionEvent);

    void onServiceStatusChange(int state);

    void onServiceDied();

}
