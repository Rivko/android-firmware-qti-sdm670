/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.bean;

import java.util.ArrayList;
import java.util.List;

public class PlatformInfo {
    private String mName;
    private String mSubType;
    private List<String> mUiCases = new ArrayList<String>(0);
    private List<String> mPcbaCases = new ArrayList<String>(0);
    private List<String> mAssembleCases = new ArrayList<String>(0);

    public String getName() {
        return mName;
    }

    public void setName(String name) {
        mName = name;
    }

    public String getSubTypeId() {
        return mSubType;
    }

    public void setSubTypeId(String subType) {
        mSubType = subType;
    }

    public List<String> getUiCases() {
        return mUiCases;
    }

    public List<String> getPcbaCases() {
        return mPcbaCases;
    }

    public List<String> getAssembleCases() {
        return mAssembleCases;
    }

    public void setAssembleCases(List<String> mAssembleCases) {
        this.mAssembleCases = mAssembleCases;
    }

    @Override
    public String toString() {
        return "PlatformInfo{" +
                "mName='" + mName + '\'' +
                ", mSubType='" + mSubType + '\'' +
                ", mUiCases=" + mUiCases +
                ", mPcbaCases=" + mPcbaCases +
                '}';
    }
}
