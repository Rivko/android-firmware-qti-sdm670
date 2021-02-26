/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.bean;

import android.app.Activity;

import com.qualcomm.qti.qmmi.R;

import java.io.Serializable;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

public class TestCase implements Serializable {
    public static final int STATE_NONE = 0; //Case not test
    public static final int STATE_PASS = 1;
    public static final int STATE_UNKNOW = 2; //case tested, but the result unknow, it is pass from tool
    public static final int STATE_FAIL = -1;

    public static final int LAUNCH_MODE_FOREGROUND = 0;
    public static final int LAUNCH_MODE_BACKGROUND = 1;

    private String mGroup;
    private String mName;
    private boolean mAuto;
    private String mMethod;
    private int mResult;
    private boolean mEnable;
    private Class<?> mActivityClass;
    private Activity mActivity;
    private Class<?> mServiceClass;
    private HashMap<String, String> parameter = new HashMap<String, String>();
    private HashMap<String, Limit> limits = new HashMap<String, Limit>();

    private Map<String, String> mTestData = new HashMap<String, String>();
    private Date mStartTime;
    private Date mEndTime;
    private float mDuration;
    private boolean mLimitCheck;
    private String mToken;
    private int mLaunchMode;
    private boolean mIsRunning;

    public TestCase() {
        this.mResult = STATE_NONE;

        this.mAuto = false;
        this.mEnable = false;
    }

    public void copy(TestCase testCase) {

        this.mEnable = testCase.getEnable();
        this.mAuto = testCase.getAuto();
        this.mResult = testCase.getResult();
        this.mName = testCase.getName();
        this.mGroup = testCase.getGroup();
        this.mActivityClass = testCase.getActivityClass();
        this.mServiceClass = testCase.getServiceClass();
        this.mLaunchMode = testCase.getLaunchMode();
        this.mIsRunning = testCase.isRunning();
        this.mMethod = testCase.getMethod();
        this.mActivity = testCase.getActivity();
        this.mStartTime = testCase.getStartTime();
        this.mEndTime = testCase.getEndTime();
        this.mDuration = testCase.getDuration();
        this.mLimitCheck = testCase.getLimitCheck();
        this.mToken = testCase.getToken();

        this.parameter.clear();
        for (String stringKey : testCase.getParameter().keySet()) {
            parameter.put(stringKey, testCase.getParameter().get(stringKey));
        }

        this.limits.clear();
        for (String stringKey : testCase.getLimits().keySet()) {
            limits.put(stringKey, testCase.getLimits().get(stringKey));
        }

        this.mTestData.clear();
        for (String stringKey : testCase.getTestData().keySet()) {
            mTestData.put(stringKey, testCase.getTestData().get(stringKey));
        }
    }

    public String getMethod() {
        return mMethod;
    }

    public void setMethod(String mMethod) {
        this.mMethod = mMethod;
    }

    public HashMap<String, Limit> getLimits() {
        return limits;
    }

    public String getGroup() {
        return mGroup;
    }

    public static int getStateNone() {
        return STATE_NONE;
    }

    public Class<?> getActivityClass() {
        return mActivityClass;
    }

    public void setActivityClass(Class<?> mActivityClass) {
        this.mActivityClass = mActivityClass;
    }

    public Activity getActivity() {
        return mActivity;
    }

    public void setActivity(Activity mActivity) {
        this.mActivity = mActivity;
    }

    public Class<?> getServiceClass() {
        return mServiceClass;
    }

    public void setServiceClass(Class<?> mServiceClass) {
        this.mServiceClass = mServiceClass;
    }

    public void setGroup(String mGroup) {

        this.mGroup = mGroup;
    }

    public String getName() {
        return mName;
    }

    public void setName(String mName) {
        this.mName = mName;
    }

    public boolean getAuto() {
        return mAuto;
    }

    public void setAuto(boolean mAuto) {
        this.mAuto = mAuto;
    }

    public int getResult() {
        return mResult;
    }

    public void setResult(int mResult) {
        this.mResult = mResult;
    }

    public boolean getEnable() {
        return mEnable;
    }

    public void setEnable(boolean mEnable) {
        this.mEnable = mEnable;
    }

    public int getLaunchMode() {
        return mLaunchMode;
    }

    public void setLaunchMode(int launchMode) {
        this.mLaunchMode = launchMode;
    }

    public int getResultPic() {
        if (mResult == TestCase.STATE_PASS) {
            return R.drawable.passpic;
        } else if (mResult == TestCase.STATE_FAIL) {
            return R.drawable.failpic;
        } else if (mResult == TestCase.STATE_NONE) {
            return -1;
        }
        return -1;
    }

    public HashMap<String, String> getParameter() {
        return parameter;
    }

    public void setStartTime(Date time) {
        this.mStartTime = time;
    }

    public Date getStartTime() {
        return mStartTime;
    }

    public void setEndTime(Date time) {
        this.mEndTime = time;
    }

    public Date getEndTime() {
        return mEndTime;
    }

    public void setDuration(float duration) {
        this.mDuration = duration;
    }

    public float getDuration() {
        return mDuration;
    }

    public void addTestData(String key, String value) {
        this.mTestData.put(key,value);
    }

    public void clearTestData() {
        this.mTestData.clear();
    }
    public Map<String, String> getTestData() {
        return mTestData;
    }

    public void setLimitCheck(boolean check) {
        this.mLimitCheck = check;
    }

    public boolean getLimitCheck() {
        return mLimitCheck;
    }

    public boolean checkLimit(Float value, String key){
        TestCase.Limit limit = getLimits().get(key);
        if(limit == null)
            return false;
        setLimitCheck(true);
        if(value > limit.min && value < limit.max){
            return true;
        }else{
            return false;
        }
    }

    public void setToken(String token) {
        this.mToken = token;
    }

    public String getToken() {
        return mToken;
    }

    public void setRunning(boolean state){
    this.mIsRunning = state;
    }

    public boolean isRunning(){
    return mIsRunning;
    }

    public static class Limit implements Serializable {
        public String name;
        public int min;
        public int max;

        public Limit(String name) {
            this.name = name;
            this.min = Integer.MIN_VALUE;
            this.max = Integer.MAX_VALUE;
        }

        public Limit(String name, int min, int max) {
            this.name = name;
            this.min = min;
            this.max = max;
        }
    }


    @Override
    public String toString() {
        StringBuffer sb = new StringBuffer();
        for (String stringStringEntry : parameter.keySet()) {
            sb.append("param: (" + stringStringEntry + "," + parameter.get(stringStringEntry) + ")");
        }


        for (String stringStringEntry : limits.keySet()) {
            sb.append("limit:( " + stringStringEntry + "," + limits.get(stringStringEntry).min + "," + limits.get(stringStringEntry).max + ")");
        }

        return "TestCase{" +
                "mGroup='" + mGroup + '\'' +
                ", mName='" + mName + '\'' +
                ", mAuto='" + mAuto + '\'' +
                ", mResult=" + mResult +
                ", mEnable='" + mEnable + '\'' +
                ", mLaunchMode='" + mLaunchMode + '\'' +
                ", mActivityClass=" + mActivityClass +
                ", mServiceClass=" + mServiceClass +
                ", mIsRunning=" + mIsRunning +
                ", hash=" + sb.toString() +
                '}';
    }
}
