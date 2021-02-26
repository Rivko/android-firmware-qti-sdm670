/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.service.diag;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

import com.qualcomm.qti.qmmi.model.CaseManager;
import com.qualcomm.qti.qmmi.utils.LogUtils;


public class MmiDiagJNIInterface {

    private static CaseManager mCaseManager;
    public static boolean isMmiJniLoaded = false;
    static Handler h = null;

    static {
        try {
            LogUtils.logi("Trying to load libmmi_jni");
            System.loadLibrary("mmi_jni");
            isMmiJniLoaded = true;
        } catch (UnsatisfiedLinkError ule) {
            LogUtils.loge("Error: failed to load libmmi_jni");
        }
    }

    public static void eventHandler(String s) {
        if (h == null) {
            LogUtils.loge( "Handle is empty");
            return;
        }
        LogUtils.logi("Going to send message");
        Bundle b = new Bundle();
        b.putString("diag_command", s);
        Message m = Message.obtain();
        m.setData(b);
        m.setTarget(h);
        m.sendToTarget();
    }

    public static int commandHandler(int cmd, String params) {
        LogUtils.logi("commandHandler start to handle cmd: " + cmd + ", params: " + params);
        int ret = -1;

        if (mCaseManager == null) {
            LogUtils.loge("the controllerInterface not intial, exit");
            return -1;
        }

        switch (cmd) {
            case DiagCommand.FTM_AP_TEST_APP_STATUS:
                ret = mCaseManager.getAppStatus(params);
                break;

            case DiagCommand.FTM_AP_SELECT_SEQUENCE:
                ret = mCaseManager.selectSequence(params);
                break;

            case DiagCommand.FTM_AP_CLEAR_RESULTS:
                ret = mCaseManager.clearResults(params);
                break;

            case DiagCommand.FTM_AP_EXECUTE_SINGLE_TEST:
                ret = mCaseManager.runSingleTest(params);
                break;

            case DiagCommand.FTM_AP_EXECUTE_ALL_TESTS:
                ret = mCaseManager.runAllTests(params);
                break;

            case DiagCommand.FTM_AP_TEST_LIST_TO_FILE:
                ret = mCaseManager.saveTestListToFile(params);
                break;

            case DiagCommand.FTM_AP_EXIT_TEST:
                ret = mCaseManager.exitTest(params);
                break;

            case DiagCommand.FTM_AP_UTILITY:
                ret = mCaseManager.doUtility(params);
                break;

            case DiagCommand.FTM_AP_APPEND_TO_RES:
                ret = mCaseManager.appendToResult(params);
                break;

            case DiagCommand.FTM_AP_READ_APP_VERSION:
                ret = mCaseManager.getAppVersion(params);
                break;

            case DiagCommand.FTM_SUBCMD_GET_FAIL_COUNT:
                ret = mCaseManager.getFailCount(params);
                break;

            case DiagCommand.FTM_SUBCMD_GET_SEQUENCE_COUNT:
                ret = mCaseManager.getSequenceCount();
                break;

            case DiagCommand.FTM_AP_SET_TOKEN:
                ret = mCaseManager.setToken(params);
                break;
            default:
                LogUtils.logi("Invalid command received:" + cmd + ", please check it");
                break;
        }

        return ret;
    }


    public void setHandler(Handler h) {
        this.h = h;
    }

    public void setCaseManager(CaseManager caseManager) {
        this.mCaseManager = caseManager;
    }

    //  public static native void getNextCommand();
}
