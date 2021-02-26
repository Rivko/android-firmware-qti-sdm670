/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.ui3d;

/**
 * Created by matthewf on 8/11/16.
 */
public class TargetAngles {

    float currentBaseAngle;
    float currentHeightAngle;
    float nextBaseAngle;
    float nextHeightAngle;

    //assume converged (stationary) target at first
    boolean converged = true;
    boolean baseAngleConverge = true;
    boolean heightAngleConverge = true;

    private float increment = 1.0f;

    public TargetAngles(float currentBaseAngle, float currentHeightAngle, float targetBaseAngle, float targetHeightAngle) {
        this.currentBaseAngle = currentBaseAngle;
        this.currentHeightAngle = currentHeightAngle;
        this.nextBaseAngle = targetBaseAngle;
        this.nextHeightAngle = targetHeightAngle;
    }

    public void setNextBaseAngle(float nBaseAngle)
    {
        baseAngleConverge = false;
        converged = false;
        nextBaseAngle = nBaseAngle;
    }

    public void setNextHeightAngle(float nHeightAngle)
    {
        heightAngleConverge = false;
        converged = false;
        nextHeightAngle = nHeightAngle;
    }

    public void incrementAngles() {
        if (!converged) {

            if (!baseAngleConverge) {
                if (currentBaseAngle < nextBaseAngle && nextBaseAngle > 0) {
                    currentBaseAngle += increment;
                } else if (currentBaseAngle > nextBaseAngle && nextBaseAngle <= 0) {
                    currentBaseAngle -= increment;
                } else
                    baseAngleConverge = true;
            }

            if (!heightAngleConverge) {

                if (currentHeightAngle < nextHeightAngle) {
                    currentHeightAngle += increment;
                    if (currentHeightAngle >= nextHeightAngle)
                        heightAngleConverge = true;
                } else if (currentHeightAngle > nextHeightAngle) {
                    currentHeightAngle -= increment;
                    if (currentHeightAngle <= nextHeightAngle)
                        heightAngleConverge = true;
                } else
                    heightAngleConverge = true;
            }

            converged = heightAngleConverge && baseAngleConverge;
            //Log.d("TARGET_DBG", "converged " + converged + " at Base/Height = " + currentBaseAngle + " / " + currentHeightAngle);
        }

/*
        Log.d("TARGET_DBG -- ", "Base,nextBase Angle = " + currentBaseAngle + "," + nextBaseAngle
                + " Height/nextHeight = " + currentHeightAngle + "," +nextHeightAngle
                + " baseConv/HeightConv/converged = " + baseAngleConverge + "/" + heightAngleConverge + "/" + converged);
*/
    }

}