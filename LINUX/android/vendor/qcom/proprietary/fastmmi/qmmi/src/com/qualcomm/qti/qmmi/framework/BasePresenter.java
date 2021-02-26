/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.framework;

import android.content.Context;
import android.os.Bundle;

import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.model.CaseManager;
import com.qualcomm.qti.qmmi.model.CaseOpCallback;

public class BasePresenter implements BaseContract.Presenter{
    private Context mContext;
    private BaseContract.View mView;
    CaseManager mCaseManager = null;

    public BasePresenter(BaseContract.View view, Context context) {
        mCaseManager = CaseManager.getInstance(context);
        mView = view;
        mContext = context;
    }

    @Override
    public TestCase getTestCaseByName(String caseName) {
        return mCaseManager.getTestCaseByName(caseName);
    }

    @Override
    public void stopCase(TestCase testCase) {
        mCaseManager.stopCase(testCase);
    }

    @Override
    public int runCase(TestCase testCase) {
        return mCaseManager.runCase(testCase);
    }

    @Override
    public void runCaseFunction(TestCase testCase, String functionName) {
        mCaseManager.runCaseFunction(testCase, functionName);
    }

    @Override
    public void updateCaseResult(TestCase testCase, int resultId) {
        mCaseManager.updateCaseResult(testCase, resultId);
    }

    public void updateView(Bundle bundle) {
        mView.updateView(bundle);
    }
}
