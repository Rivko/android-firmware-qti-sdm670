/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.model;

import com.qualcomm.qti.qmmi.bean.TestCase;

import java.util.List;

public interface CaseOpCallback {

    interface LoadCasesCallback {
        void onCasesLoaded(List<TestCase> cases);
    }

    interface GetCaseCallback {
        void onCaseLoaded(TestCase testCase);
    }

}
