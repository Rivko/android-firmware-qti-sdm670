/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.framework;

import android.content.Context;
import android.content.pm.PackageInfo;

import com.qualcomm.qti.qmmi.bean.TestCase;

import java.util.List;

/**
 * This specifies the contract between the view and the presenter.
 */
public interface CasesContract {

    interface View {

        void showCases(List<TestCase> cases);

        void updateResult();

        void updateStatus(String status);

        void startAllServices(List<TestCase> cases);
    }

    interface Presenter {
        void start();

        List<TestCase> getTestCaseList();

        TestCase getTestCaseByName(String caseName);

        void setPackageInfo(PackageInfo info);

        void clearResult();

        void stopCase(TestCase testCase);

        int runCase(TestCase testCase);

        void updateCaseResult(TestCase testCase, int resultId);

        void startCaseService(Context context,TestCase testCase);
    }
}
