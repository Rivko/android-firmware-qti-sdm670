/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.utils;

import java.util.Arrays;

/**
 * This class implements an IIR filter with an arbitrary order. For each calculation step, it will
 * use two sets of coefficients: Xc (the coefficients to apply against current and previous input
 * values) and Yc (the coefficients to apply against previous output values).
 */
public class IirFilter {

    private final float[] mXc;
    private final float[] mYc;
    private final float mGain;
    private final float[] mInputs;
    private final float[] mOutputs;
    private final int mOrder;
    private int mPos;

    /**
     * Construct an IIR filter with the given parameters. The order of the filter is inferred from
     * the length of Xc.
     * @param Xc Coefficients to apply against current and previous inputs. Xc[0] applies to the
     *        most recent input, and Xc[order - 1] applies to the oldest input.
     * @param Yc Coefficients to apply against previous outputs. Yc[0] specifies the filter gain,
     *        Yc[1] applies to the most recent output, and Yc[order - 1] applies to the oldest
     *        output.
     */
    public IirFilter(float[] Xc, float[] Yc) {
        mOrder = Xc.length;
        if (Yc.length != mOrder) {
            throw new IllegalArgumentException("Yc.length must be order");
        }

        mXc = Xc;
        mYc = Yc;
        mGain = 1f / mYc[0];
        mInputs = new float[mOrder];
        mOutputs = new float[mOrder];
        mPos = 0;
    }

    /**
     * Run the filter with the specified input for 10*(filter order) iterations. If the filter will
     * ever reach stability, it should have by then!
     * @param val Continuous input value to apply to the filter
     */
    public void resetWithInput(float val) {
        Arrays.fill(mInputs, 0);
        Arrays.fill(mOutputs, 0);
        mPos = 0;
        for (int i = 0; i < mOrder * 10; i++) {
            add(val);
        }
    }

    /**
     * Adds a single input sample to the filter, and calculate the new output
     * @param val Input sample
     * @return Output sample
     */
    public float add(float val) {
        mPos++;
        if (mPos == mOrder) {
            mPos = 0;
        }

        mInputs[mPos] = val;
        mOutputs[mPos] = 0;

        for (int i = 0, curPos = mPos; i < mOrder; i++, curPos--) {
            if (curPos < 0) {
                curPos = mOrder - 1;
            }
            mOutputs[mPos] += mXc[i] * mInputs[curPos] - (i > 0 ? mYc[i] * mOutputs[curPos] : 0);
        }

        return (mOutputs[mPos] * mGain);
    }
}
