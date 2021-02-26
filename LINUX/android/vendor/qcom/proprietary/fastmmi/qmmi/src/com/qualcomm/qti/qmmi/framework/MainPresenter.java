/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.framework;

import android.content.Context;
import android.content.pm.PackageInfo;

import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.model.CaseManager;
import com.qualcomm.qti.qmmi.model.CaseOpCallback;
import com.qualcomm.qti.qmmi.model.ConfigParser.TestMode;
import com.qualcomm.qti.qmmi.utils.LogUtils;

import java.util.List;

public class MainPresenter implements CasesContract.Presenter, CaseManager.TestStateChangedListener, CaseOpCallback.LoadCasesCallback {
    private Context mContext;
    private CasesContract.View mCasesView;
    CaseManager mCaseManager = null;

    @Override
    public List<TestCase> getTestCaseList() {
        return mCaseManager.getTestCaseList();
    }

    @Override
    public TestCase getTestCaseByName(String caseName) {
        return mCaseManager.getTestCaseByName(caseName);
    }

    @Override
    public void setPackageInfo(PackageInfo packageInfo) {
        mCaseManager.setPackageInfo(packageInfo);
    }

    public MainPresenter(CasesContract.View casesView, Context context) {
        mCaseManager = CaseManager.getInstance(context);
        mCaseManager.addTestStateChangedListener(this);
        mCasesView = casesView;
        mContext = context;
    }

    @Override
    public void start() {
        TestMode mode = mCaseManager.getCurMode();
        LogUtils.logi("current mode : " + mode);
        mode = mode == null? TestMode.UI : mode;
        mCaseManager.loadCases(this, mode);
    }

    @Override
    public void clearResult() {
        mCaseManager.clearResults("");
    }

    @Override
    public void onTestCaseStateChanged() {
        this.updateResult();
    }

    public void updateResult(){
        mCasesView.updateResult();
        mCasesView.updateStatus(mCaseManager.getCaseStatus());
    }

    @Override
    public void onCasesLoaded(List<TestCase> cases) {
        LogUtils.logi(" size:" + cases.size());
        if (mCasesView == null) {
            return;
        }

        /**update view list*/
        mCasesView.showCases(cases);
        /**start service*/
        mCasesView.startAllServices(cases);
        /**update status basr*/
        mCasesView.updateStatus(mCaseManager.getCaseStatus());
    }

    @Override
    public void stopCase(TestCase testCase) {
        // TODO Auto-generated method stub
        mCaseManager.stopCase(testCase);
    }

    @Override
    public int runCase(TestCase testCase) {
        // TODO Auto-generated method stub
        return mCaseManager.runCase(testCase);
    }

    @Override
    public void updateCaseResult(TestCase testCase, int resultId) {
        // TODO Auto-generated method stub
        mCaseManager.updateCaseResult(testCase, resultId);
    }

    @Override
    public void startCaseService(Context context, TestCase testCase) {
        // TODO Auto-generated method stub
        mCaseManager.startCaseService(context,testCase);
    }
}
