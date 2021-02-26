/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.asr;

import android.content.Context;

import com.qualcomm.qti.smartassistant.asr.baidu.DuerVoiceSdkImpl;
import com.qualcomm.qti.smartassistant.utils.LogUtils;

public class VoiceSdkHelper {
    private static final String TAG = "VoiceSdkHelper";
    private IVoiceSdk mVoiceSdkImpl;
    private Context mContext;

    public VoiceSdkHelper(Context context){
        mContext = context;
        mVoiceSdkImpl = new DuerVoiceSdkImpl(context);
    }

    public void initializeSdk(){
        try {
            LogUtils.v(TAG,"initializeSdk begin");
            mVoiceSdkImpl.initializeSdk(mContext);
            LogUtils.v("initializeSdk end");
        }catch (Exception e){
            e.printStackTrace();
            LogUtils.e("initializeSdk exception = "+e.toString());
        }
    }

    public void addVoiceSdkListener(IVoiceSdkListener listener){
        mVoiceSdkImpl.addVoiceSdkListener(listener);
    }

    public void removeVoiceSdkListener(IVoiceSdkListener listener){
        mVoiceSdkImpl.removeVoiceSdkListener(listener);
    }

    public void startRecognition(boolean captureAvailable, int captureSession,
                                 int soundModelHandle){
        try {
            LogUtils.v("startRecognition begin");
            mVoiceSdkImpl.startRecognition(captureAvailable, captureSession, soundModelHandle);
            LogUtils.v("startRecognition end");
        }catch (Exception e){
            e.printStackTrace();
            LogUtils.e("startRecognition exception = "+e.toString());
        }
    }

    public void stopRecognition(){
        try {
            LogUtils.v("stopRecognition begin");
            mVoiceSdkImpl.stopRecognition();
            LogUtils.v("stopRecognition end");
        }catch (Exception e){
            e.printStackTrace();
            LogUtils.e("stopRecognition exception = "+e.toString());
        }
    }

    public void updateContacts(String contacts){
        try {
            mVoiceSdkImpl.updateContacts(contacts);
        }catch (Exception e){
            e.printStackTrace();
            LogUtils.e("updateContacts exception = "+e.toString());
        }
    }

    public void releaseSdk(){
        try {
            LogUtils.v("releaseSdk begin");
            mVoiceSdkImpl.releaseSdk();
            LogUtils.v("releaseSdk end");
        }catch (Exception e){
            e.printStackTrace();
            LogUtils.e("releaseSdk exception = "+e.toString());
        }
    }
}
