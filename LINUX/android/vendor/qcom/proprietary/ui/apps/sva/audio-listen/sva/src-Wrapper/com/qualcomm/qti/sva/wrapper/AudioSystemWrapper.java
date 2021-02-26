/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.qti.sva.wrapper;

import android.media.AudioSystem;

public class AudioSystemWrapper {

    /*
     * Sets a group generic audio configuration parameters. The use of these parameters
     * are platform dependent, see libaudio
     *
     * param keyValuePairs  list of parameters key value pairs in the form:
     *    key1=value1;key2=value2;...
     */
    public static int setParameters(String keyValuePairs) {
        return AudioSystem.setParameters(keyValuePairs);
    }

}
