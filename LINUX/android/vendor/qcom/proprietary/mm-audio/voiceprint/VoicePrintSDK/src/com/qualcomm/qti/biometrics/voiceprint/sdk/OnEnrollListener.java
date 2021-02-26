/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.biometrics.voiceprint.sdk;

import android.os.Bundle;

/**
 * The listener interface for receiving OnEnrollListener events.
 * The class that is interested in processing a OnEnrollListener
 * event implements this interface, and the object created
 * with that class is registered with a component using the
 * component's <code>enroll<code> method. When
 * the OnEnrollListener event occurs, that object's appropriate
 * method is invoked.
 */
public interface OnEnrollListener extends OnResultBase {

    /**
     * @param success Voice Model file has been saved.
     * @param extra   Any extra information including error code if not successful.
     *                This can be null.
     * @brief Invoked when Enrollment training has completed and saved.
     * @details This method is invoked when all training has completed, the
     * completeEnroll event has been called, and the voice model file
     * has been saved.
     */
    public void onEnrollComplete(boolean success, Bundle extra);

}