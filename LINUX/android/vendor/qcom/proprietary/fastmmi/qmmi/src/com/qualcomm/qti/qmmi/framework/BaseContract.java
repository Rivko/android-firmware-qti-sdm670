/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.framework;

import com.qualcomm.qti.qmmi.bean.TestCase;

import android.os.Bundle;

public interface BaseContract {

    interface View {
        void updateView(Bundle bundle);
    }

    interface Presenter {
        void updateCaseResult(TestCase testCase, int resultId);

        TestCase getTestCaseByName(String caseName);

        int runCase(TestCase testCase);

        void runCaseFunction(TestCase testCase, String functionName);

        void stopCase(TestCase testCase);
    }
}
