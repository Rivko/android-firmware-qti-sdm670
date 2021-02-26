/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.model;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.os.Bundle;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.PlatformInfo;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.model.ConfigParser.TestMode;
import com.qualcomm.qti.qmmi.service.diag.DiagCommand;
import com.qualcomm.qti.qmmi.utils.FileUtils;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.Utils;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

public class CaseManager implements CaseOpCallback, ControllerInterface {

    public static final int MMI_IDLE = 0;
    public static final int MMI_BUSY = 1;
    private int mStatus = MMI_IDLE;

    /**
     * The test ui case
     */
    private ConfigParser.TestMode mCurMode = ConfigParser.TestMode.UI;
    private String mPcbaResultFile;
    private String mAssemblyResultFile;
    private String mUiResultFile;

    //The list of current mode test cases.
    private List<TestCase> mTestCaseList = new ArrayList<TestCase>();
    //The map of current mode test cases, same items as mTestCaseList
    private Map<String, TestCase> mTestCaseMap = new HashMap<String, TestCase>();

    /**
     * state change listeners
     */
    private final List<TestStateChangedListener> listeners = new LinkedList<TestStateChangedListener>();

    private CaseServiceManager mCaseServiceManager;

    private volatile static CaseManager mCaseManager = null;
    private static Context mContext;
    private PackageInfo mPackageInfo;
    private ResultThread mResultThread;

    private CaseManager() {
        super();
        mCaseServiceManager = CaseServiceManager.getInstance();
        mResultThread = new ResultThread();
        mResultThread.start();
    }

    public static CaseManager getInstance(Context context) {
        if (mCaseManager == null) {
            mContext = context;
            synchronized (CaseManager.class) {
                if (mCaseManager == null) {
                    mCaseManager = new CaseManager();
                }
            }
        }
        return mCaseManager;
    }

    public void setPackageInfo(PackageInfo packageInfo) {
        this.mPackageInfo = packageInfo;
    }

    public int getStatus() {
        return mStatus;

    }

    public void setStatus(int mStatus) {
        mStatus = mStatus;
    }

    public TestCase getTestCaseByName(String caseName) {
        return mTestCaseMap.get(caseName);
    }

    public TestMode getCurMode(){
        return mCurMode;
    }

    public List<TestCase> getTestCaseList() {
        return mTestCaseList;
    }

    public interface TestStateChangedListener {
        void onTestCaseStateChanged();
    }


    public void addTestStateChangedListener(TestStateChangedListener listener) {
        listeners.add(listener);
    }

    public void removeTestStateChangedListener(TestStateChangedListener listener) {
        listeners.remove(listener);
    }

    public void updateCaseResult(TestCase testCase, int state) {

        if (testCase != null) {
            LogUtils.logi( "updateCaseResult: case: " + testCase.getName() + " state: " + state);
            testCase.setResult(state);

            /**update the end time and calculate the duration*/
            testCase.setEndTime(Utils.getCurrentTime());
            if(testCase.getStartTime()!= null) {
                float duration = (testCase.getEndTime().getTime() - testCase.getStartTime().getTime()) / 1000;
                testCase.setDuration(duration);
            }else{
                testCase.setDuration(0);
            }

            //update to result file
            updateResFiles(mCurMode);

        }
        notifyStateChanged();
    }

    private void notifyStateChanged() {
        for (TestStateChangedListener listener : listeners) {
            listener.onTestCaseStateChanged();
        }
    }

    private void initCasesAndResult(TestMode type) {
        //try to load case from local dir first
        mCurMode = type;

        Map<String, PlatformInfo> platformInfoMap = new HashMap<String, PlatformInfo>();

        List<TestCase> allTestCasesList = new ArrayList<TestCase>();
        Map<String, TestCase> allTestCasesMap = new HashMap<String, TestCase>();

        List<TestCase> uiCasesList = new ArrayList<TestCase>();
        List<TestCase> pcbaCasesList = new ArrayList<TestCase>();
        List<TestCase> assmebleCasesList = new ArrayList<TestCase>();
        Map<String, TestCase> uiCasesMap = new HashMap<String, TestCase>();
        Map<String, TestCase> pcbaCasesMap = new HashMap<String, TestCase>();
        Map<String, TestCase> assembleCasesMap = new HashMap<String, TestCase>();

        /**Load case from configure*/
        ConfigParser.parseCaseConfigFile(mContext.getResources().getXml(
                R.xml.test_case_config), allTestCasesList, allTestCasesMap);
        ConfigParser.parsePlatformConfigFile(mContext.getResources().getXml(
                R.xml.platform_case_config), platformInfoMap);

        /**Platform UI case*/
        PlatformInfo platformInfo = null;
        String platformName = PlatformManager.getPlatformName();

        if (platformInfoMap.get(platformName) != null) {
            platformInfo = platformInfoMap.get(platformName);
        } else {
            platformInfo = platformInfoMap.get(PlatformManager.PLATFORM_DEFAULT);
        }
        LogUtils.logi("cur platform name is:" + platformInfo.getName());

        /**Platform UI case*/
        String ui_default_file = mContext.getFilesDir().getAbsolutePath() + "/" + ConfigParser.FILE_UI_DEFAULT_XML;
        ConfigParser.parseCaseConfigFile(ui_default_file, uiCasesList, null);
        if (uiCasesList.size() < 1) {
            List<String> uiCaseNames = platformInfo.getUiCases();
            for (String uiCaseName : uiCaseNames) {
                TestCase tcTarget = new TestCase();
                TestCase tcSource = allTestCasesMap.get(uiCaseName);

                if (tcSource != null && tcSource.getEnable()) {
                    tcTarget.copy(tcSource);
                    uiCasesList.add(tcTarget);
                } else {
                    LogUtils.logi("the ui case config in platform not support: " + uiCaseName);
                }
            }
            ConfigParser.generateConfig(ui_default_file, uiCasesList, ConfigParser.TestMode.UI);
        }

        /**Platform PCBA case*/
        String pcba_default_file = mContext.getFilesDir().getAbsolutePath() + "/" + ConfigParser.FILE_PCBA_DEFAULT_XML;
        ConfigParser.parseCaseConfigFile(pcba_default_file, pcbaCasesList, null);
        if (pcbaCasesList.size() < 1) {
            List<String> pcbaCaseNames = platformInfo.getPcbaCases();
            for (String pcbaCaseName : pcbaCaseNames) {
                TestCase tcTarget = new TestCase();
                TestCase tcSource = allTestCasesMap.get(pcbaCaseName);

                if (tcSource != null && tcSource.getEnable()) {
                    tcTarget.copy(tcSource);
                    pcbaCasesList.add(tcTarget);
                } else {
                    LogUtils.logi("the pcba case config in platform not support: " + pcbaCaseName);
                }
            }
            ConfigParser.generateConfig(pcba_default_file, pcbaCasesList, ConfigParser.TestMode.PCBA);
        }

        /**Platform Assemble case*/
        String assemble_default_file = mContext.getFilesDir().getAbsolutePath() + "/" + ConfigParser.FILE_ASSEMBLE_DEFAULT_XML;
        ConfigParser.parseCaseConfigFile(assemble_default_file, assmebleCasesList, null);
        if (assmebleCasesList.size() < 1) {
            List<String> assembleCaseNames = platformInfo.getAssembleCases();
            for (String assembleCaseName : assembleCaseNames) {
                TestCase tcTarget = new TestCase();
                TestCase tcSource = allTestCasesMap.get(assembleCaseName);

                if (tcSource != null && tcSource.getEnable()) {
                    tcTarget.copy(tcSource);
                    assmebleCasesList.add(tcTarget);
                } else {
                    LogUtils.logi("the assembleCaseName case config in platform not support: " + assembleCaseName);
                }
            }
            ConfigParser.generateConfig(assemble_default_file, assmebleCasesList, ConfigParser.TestMode.ASSEMBLY);
        }

        mUiResultFile = mContext.getFilesDir().getAbsolutePath() + "/" + ResultParser.FILE_RESULT_UI_XML;
        mPcbaResultFile = mContext.getFilesDir().getAbsolutePath() + "/" + ResultParser.FILE_RESULT_PCBA_XML;
        mAssemblyResultFile = mContext.getFilesDir().getAbsolutePath() + "/" + ResultParser.FILE_RESULT_ASSEMBLY_XML;

        switch(mCurMode){
        case PCBA:
            mTestCaseList = pcbaCasesList;
            break;
        case ASSEMBLY:
            mTestCaseList = assmebleCasesList;
            break;
        case UI:
            mTestCaseList = uiCasesList;
            break;
        }

        readResultFile();
    }

    private void readResultFile(){
        String curResultFile = null;

        // read the previous result file
        switch(mCurMode){
        case PCBA:
            curResultFile = mPcbaResultFile;
            break;
        case ASSEMBLY:
            curResultFile = mAssemblyResultFile;
            break;
        case UI:
            curResultFile = mUiResultFile;
            break;
        }

        for(TestCase testCase: mTestCaseList){
            mTestCaseMap.put(testCase.getName(), testCase);
        }

        ResultParser.readResultFile(curResultFile, mTestCaseMap);
    }

    public void loadCases(final LoadCasesCallback callback, TestMode type) {
        LogUtils.logi("load case start");

        initCasesAndResult(type);

        callback.onCasesLoaded(mTestCaseList);

    }

    private void updateResFiles(TestMode mode){

        mResultThread.setTestCaseList(mTestCaseList);
        if (mResultThread != null) {
            switch(mode){
                case PCBA:
                    LogUtils.logi("TestMode: PCBA");
                    mResultThread.setFilePath(mPcbaResultFile);
                    break;
                case ASSEMBLY:
                    LogUtils.logi("TestMode: ASSEMBLY");
                    mResultThread.setFilePath(mAssemblyResultFile);
                    break;
                case UI:
                    LogUtils.logi("TestMode: UI");
                    mResultThread.setFilePath(mUiResultFile);
                    break;
            }
        }
        mResultThread.flushResult();
    }

    public String getCaseStatus() {
        StringBuffer sb = new StringBuffer();
        int passCount = 0, failCount = 0, remainCount = 0;

        int totalCount = mTestCaseList.size();
        for (TestCase t : mTestCaseList) {
            if (t.getResult() == TestCase.STATE_PASS)
                passCount++;
            else if (t.getResult() == TestCase.STATE_FAIL)
                failCount++;
            else if (t.getResult() == TestCase.STATE_NONE)
                remainCount++;
        }
        sb.append(passCount + " P | " + failCount + " F  | " + remainCount + " L | " + totalCount + " R ");
        return sb.toString();
    }

    public int runCase(TestCase testCase) {
        testCase.setStartTime(Utils.getCurrentTime());
        setStatus(MMI_BUSY);
        //clear case's history result data
        testCase.clearTestData();

        //clear case's history state
        testCase.setResult(TestCase.STATE_NONE);
        //update to result file
        updateResFiles(mCurMode);

        /**update UI*/
        Intent intent = new Intent();
        intent.setAction(Utils.ACTION_DIAG_UPDATE_TESTCAST_RESULT);
        mContext.sendBroadcast(intent);

        return mCaseServiceManager.runService(mContext, testCase);
    }

    public void runCaseFunction(TestCase testCase, String functionName) {
        setStatus(MMI_BUSY);
        mCaseServiceManager.runServiceFunction(testCase, functionName);
    }

    public void stopCase(TestCase testCase) {
        mCaseServiceManager.stopService(testCase);
        setStatus(MMI_IDLE);
    }

    public void stopAllService(Context context) {
        mCaseServiceManager.stopAllService(context);
    }

    public void startCaseService(Context context, TestCase testCase) {
        mCaseServiceManager.startCaseService(context, testCase);
    }

    public void startParseResultFile() {

    }

    /******************ControllerInterface start ***********************/
    public boolean diagLoadCases(File cfg) {
        LogUtils.logi("diag load case start");

        List<TestCase> testCaseList = new ArrayList<TestCase>();
        /**Load case from configure*/
        mCurMode = ConfigParser.parseCaseConfigFile(cfg.getAbsolutePath(), testCaseList, null);
        LogUtils.logd("Current mode:" + mCurMode);

        if (testCaseList.size() < 1) {
            LogUtils.loge("No test case parsed, please check the config file");
            return false;
        }

        //clear current mode case map and re-load items
        mTestCaseList.clear();
        mTestCaseMap.clear();

        for (TestCase testCase : testCaseList) {
            TestCase tcTarget = new TestCase();
            if (testCase != null) {
                tcTarget.copy(testCase);
                mTestCaseList.add(tcTarget);
                mTestCaseMap.put(testCase.getName(), tcTarget);
            } else {
                LogUtils.loge("The test case is null! " + testCase.getName());
            }
        }

        return true;
    }

    @Override
    public int getFailCount(String params) {
        int count = 0;
        for (TestCase t : mTestCaseList) {
            if (t.getResult() == TestCase.STATE_FAIL)
                count++;
        }
        return count;
    }

    @Override
    public int getSequenceCount() {
        return mTestCaseList.size();
    }

    @Override
    public int getAppStatus(String params) {
        return getStatus();
    }

    @Override
    public int selectSequence(String cfgFileName) {
        String baseDir = mContext.getApplicationContext().getFilesDir().getAbsolutePath() + "/";
        String filePathAbs = baseDir + cfgFileName;

        File cfg = new File(filePathAbs);
        if (!cfg.exists()) {
            LogUtils.loge("the select config file not exist");
            return -1;
        }

        if (!diagLoadCases(cfg)) {
            LogUtils.loge("parse config file fail");
            return -1;
        }

        readResultFile();

        /**update UI*/
        Intent intent = new Intent();
        intent.setAction(Utils.ACTION_DIAG_UPDATE_TESTCAST_LIST);
        mContext.sendBroadcast(intent);
        return 0;
    }

    @Override
    public int clearResults(String params) {
        for (TestCase t : mTestCaseList) {
            t.setResult(TestCase.STATE_NONE);
        }

        /**update result*/
        updateResFiles(mCurMode);

        /**update UI*/
        Intent intent = new Intent();
        intent.setAction(Utils.ACTION_DIAG_UPDATE_TESTCAST_RESULT);
        mContext.sendBroadcast(intent);
        return 0;
    }

    @Override
    public int runSingleTest(String caseName) {
        /**check the case exist*/
        TestCase testCase = getTestCaseByName(caseName);
        if (testCase == null) {
            LogUtils.loge("Case " + caseName + " doesn't exist, can't run!");
            return -1;
        } else if (testCase.isRunning()) {
            LogUtils.loge("Case " + caseName + " is running, can't run again!");
            return -1;
        }

        /** For auto case, run in Background without UI **/
        if (testCase.getAuto()){
            LogUtils.logi("Run service for auto case : " + testCase.getServiceClass().getName());
            testCase.setLaunchMode(TestCase.LAUNCH_MODE_BACKGROUND);
            runCase(testCase);
        } else {
            LogUtils.logi("Run non-auto case : " + testCase.getServiceClass().getName());
            Bundle bundle = new Bundle();
            Intent intent = new Intent();
            bundle.putString(Utils.BUNDLE_KEY_CASE_NAME, testCase.getName());
            intent.putExtras(bundle);
            intent.setAction(Utils.ACTION_DIAG_START_TESTCAST);
            mContext.sendBroadcast(intent);
        }

        return 0;
    }

    @Override
    public int runAllTests(String params) {
        for (TestCase testCase : mTestCaseList) {
            if (testCase.getAuto()) {
                testCase.setLaunchMode(TestCase.LAUNCH_MODE_BACKGROUND);
                runCase(testCase);
            }
        }
        return 0;
    }

    @Override
    public int exitTest(String params) {
        if (params == null || params.split(":").length < 2) {
            LogUtils.loge("Wrong param");
            return -1;
        }
        String str[] = params.split(":");
        String caseName = str[0];
        String result = str[1];

        /**check the case exist*/
        TestCase testCase = getTestCaseByName(caseName.trim());
        if (testCase == null) {
            LogUtils.loge("Case " + caseName + " doesn't exist, can't exit!!");
            return -1;
        } else if (!testCase.isRunning()){
            LogUtils.loge("Case " + caseName + " is not running, can't exit!");
            return -1;
        }

        /**stop run*/
        LogUtils.logi("Find the case:" + testCase.getName() + ", stop test case with result:" + result);
        if ("1".equalsIgnoreCase(result)) {
            testCase.setResult(TestCase.STATE_PASS);
        } else if ("0".equalsIgnoreCase(result)) {
            testCase.setResult(TestCase.STATE_FAIL);
        } else if ("2".equalsIgnoreCase(result)) {
            if (testCase.getResult() == TestCase.STATE_NONE)
                testCase.setResult(TestCase.STATE_UNKNOW);
        }

        /**update the end time and calculate the duration*/
        testCase.setEndTime(Utils.getCurrentTime());
        if (testCase.getStartTime() != null) {
            float duration = (testCase.getEndTime().getTime() - testCase.getStartTime().getTime()) / 1000;
            testCase.setDuration(duration);
        } else {
            testCase.setDuration(0);
        }

        /**stop the service*/
        stopCase(testCase);

        /**exist the activity*/
        if (testCase.getActivity() != null) {
            LogUtils.logi("Find the activity, call finish");
            testCase.getActivity().finish();
        }

        /**update UI*/
        Intent intent = new Intent();
        intent.setAction(Utils.ACTION_DIAG_UPDATE_TESTCAST_RESULT);
        mContext.sendBroadcast(intent);

        /**update result*/
        updateResFiles(mCurMode);

        return 0;
    }


    @Override
    public int doUtility(String params) {

        HidlManager hidlManager = HidlManager.getInstance();
        if (hidlManager == null) {
            LogUtils.loge("Not support");
            return -1;
        }

        if (params == null || params.split(":").length < 2) {
            LogUtils.loge("Wrong param");
            return -1;
        }

        String str[] = params.split(":");
        int utilityId = Integer.valueOf(str[0]);
        int utilityOps = Integer.valueOf(str[1]);
        LogUtils.loge("do utility (" + utilityId + "," + utilityOps + ")");
        switch (utilityId) {
            case DiagCommand.UTILITY_ID_AUTOSTART:
                break;

            case DiagCommand.UTILITY_ID_WLAN:
                if (utilityOps == DiagCommand.OPS_ON) {
                    hidlManager.wifiEnable(true);
                    hidlManager.runFtmDaemonApp("-n;-dd");
                } else {
                    hidlManager.wifiEnable(false);
                    hidlManager.stopFtmDaemonApp();
                }
                break;

            case DiagCommand.UTILITY_ID_BT:
            case DiagCommand.UTILITY_ID_NFC:
                if (utilityOps == DiagCommand.OPS_ON) {
                    hidlManager.runFtmDaemonApp("-n;-dd");
                } else {
                    hidlManager.stopFtmDaemonApp();
                }
                break;

            case DiagCommand.UTILITY_ID_CHARGER:
                if (utilityOps == DiagCommand.OPS_ON) {
                    hidlManager.chargerEnable(true);
                } else {
                    hidlManager.chargerEnable(false);
                }
                break;

            case DiagCommand.UTILITY_ID_SHIPMODE:
                hidlManager.enterShipMode();
                break;

            default:
                break;
        }
        return 0;
    }

    @Override
    public int saveTestListToFile(String params) {
        StringBuffer sb = new StringBuffer();
        for (TestCase t : mTestCaseList) {
            sb.append(t.getName() + "\n");
        }
        String filePath = mContext.getApplicationContext().getFilesDir() + "/testlist.txt";
        FileUtils.writeFile(filePath, sb.toString());
        return 0;
    }

    @Override
    public int appendToResult(String params) {

        if (params == null || params.split(":").length < 2) {
            LogUtils.loge("Wrong param");
            return -1;
        }

        String str[] = params.split(":");
        String caseName = str[0];
        String caseData = str[1];
        String kv[] = caseData.split("=");
        if (kv.length < 2) {
            LogUtils.loge("Wrong param");
            return -1;
        }

        /**check the case exist*/
        TestCase testCase = getTestCaseByName(caseName);
        if (testCase != null) {
            LogUtils.logi("find the case, update KV:(" + kv[0] + "," + kv[1] + ")");
            testCase.getTestData().put(kv[0], kv[1]);
        }

        return 0;
    }

    @Override
    public int getAppVersion(String params) {
        LogUtils.logi("xxxx versionCode:" + mPackageInfo.versionCode);
        return mPackageInfo.versionCode;
    }

    @Override
    public int setToken(String params) {
        if (params == null || params.split(":").length < 2) {
            LogUtils.loge("Wrong param");
            return -1;
        }

        String str[] = params.split(":");
        String caseName = str[0];
        String caseToken = str[1];

        TestCase testCase = getTestCaseByName(caseName);
        if (testCase != null) {
            LogUtils.logi("find the case, caseName" + caseName + ",caseToken:" + caseToken);
            testCase.setToken(caseToken);
        }
        return 0;
    }
    /*******************ControllerInterface end *******************/
}
