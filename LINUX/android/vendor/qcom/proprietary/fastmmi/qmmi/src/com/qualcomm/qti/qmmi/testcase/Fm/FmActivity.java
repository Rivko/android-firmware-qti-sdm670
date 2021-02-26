/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/* Copyright (c) 2017, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package com.qualcomm.qti.qmmi.testcase.Fm;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.media.AudioManager;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.view.View;
import android.view.Window;
import android.widget.Button;

import java.text.DecimalFormat;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseActivity;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.Utils;

import android.media.AudioSystem;

import qcom.fmradio.FmReceiver;
import qcom.fmradio.FmRxEvCallbacksAdaptor;
import qcom.fmradio.FmConfig;
import qcom.fmradio.FmTransceiver;

public class FmActivity extends BaseActivity {
    public final static int UPDATE_MAIN_VIEW = 0;
    public final static int TIME_OUT = 1;
    private static final String RADIO_DEVICE = "/dev/radio0";

    private FmReceiver mReceiver = null;
    Button mSearchBtn;
    Context mContext = null;

    private static float mFrequency = 0f;
    private static float mFreqConfig = 0f;
    private static int mTimeout = 0;
    private static volatile boolean mIsTimeOut = false;
    private FmCountDown mFmCountDown = null;

    private boolean mHeadsetPlugged = false;
    private BroadcastReceiver mHeadsetReceiver = null;
    private volatile boolean mIsFirstTimePlay = true;
    private AlertDialog mHeadSetDialog = null;
    private AlertDialog mFmReceiverDialog = null;

    private Object mEventWaitLock = new Object();
    private boolean mEventReceived = false;
    private static boolean mFMReceiverEnable = false;

    private static final int RADIO_TIMEOUT = 1500;
    private static final int ENABLE_SLIMBUS_DATA_PORT = 1;
    private static final int ENABLE_SLIMBUS_CLOCK_DATA = 2;

    FmRxEvCallbacksAdaptor mFmCallback = new FmRxEvCallbacksAdaptor() {
        public void FmRxEvSearchComplete(int freq) {
            LogUtils.logi("FmRxEvSearchComplete:" + freq);
            FmActivity.mFrequency = (float) (freq / 1000.00);
            LogUtils.logi("Show Frequency : " + FmActivity.mFrequency);
            mHandler.sendEmptyMessage(UPDATE_MAIN_VIEW);
            mTestCase.addTestData("frequency:", String.valueOf(FmActivity.mFrequency));
        }

        public void FmRxEvSearchCancelled(){
            LogUtils.logi("FmRxEvSearchCancelled:");
        }

        public void FmRxEvRadioTuneStatus(int freq) {
            LogUtils.logi("FmRxEvRadioTuneStatus:" + freq);
            FmActivity.mFrequency = (float) (freq / 1000.00);
            LogUtils.logi("Show Frequency : " + FmActivity.mFrequency);
            mHandler.sendEmptyMessage(UPDATE_MAIN_VIEW);
            mTestCase.addTestData("frequency:", String.valueOf(FmActivity.mFrequency));
        }

        public void FmRxEvEnableReceiver() {
            LogUtils.logi("FmRxEvEnableReceiver");
            mReceiver.setRawRdsGrpMask();
            mFMReceiverEnable = true;
            cancelFmReceiverDialog();
            if (mReceiver != null && mReceiver.isCherokeeChip()) {
                synchronized(mEventWaitLock) {
                    mEventReceived = true;
                    mEventWaitLock.notify();
                }
            }

            if (mIsFirstTimePlay) {
                FmActivity.this.runOnUiThread(new Runnable(){

                    @Override
                    public void run() {
                        // TODO Auto-generated method stub
                        if (mIsFirstTimePlay){
                            cancelHeadSetDialog();
                            cancelFmReceiverDialog();
                            playFM(false);
                        }
                    }
                });
            }
        }

        public void FmRxEvEnableSlimbus(int status){
            LogUtils.logi("FmRxEvEnableSlimbus:" + status);
            if (mReceiver != null && mReceiver.isCherokeeChip()) {
                synchronized(mEventWaitLock) {
                    mEventReceived = true;
                    mEventWaitLock.notify();
                }
            }
        }

        public void FmRxEvDisableReceiver(){
            LogUtils.logi("FmRxEvDisableReceiver");
            mFMReceiverEnable = false;
            if (mReceiver != null && mReceiver.isCherokeeChip()) {
                synchronized (mEventWaitLock) {
                    mEventReceived = true;
                    mEventWaitLock.notify();
                }
            }
        }
    };

    @Override
    public void finish() {
        if ( mFmCountDown != null ) mFmCountDown.cancel();

        super.finish();
    }

    void bindView() {
        mSearchBtn = (Button) findViewById(R.id.fm_search);
        mTextView.setText(mFrequency + "MHZ");

        mSearchBtn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                playFM(true);
            }
        });
    }

    private boolean isHeadSetPlugged(){
        LogUtils.logi("isHeadSetPlugged: " + mHeadsetPlugged);
        return mHeadsetPlugged;
    }

    private void playFM(boolean isManualSearch){

        LogUtils.logd("mFMReceiverEnable: " + mFMReceiverEnable);

        if (mFMReceiverEnable){
            boolean bStatus = false;

            if (isHeadSetPlugged()) {
                mIsFirstTimePlay = false;
                if (!isManualSearch) {
                    bStatus = setStation((int)(mFreqConfig *1000));
                    if (bStatus){
                        mIsTimeOut = false;
                        new countDownThread().start();
                        enableSearchBtn(false);
                    }
                } else {
                    bStatus = searchUPOrDown(true);
                }

                setDeviceOutFmState(true);
            } else {
                showHeadSetDialog();
            }
        } else {
            showFmReceiverDialog();
            updateResult(TestCase.STATE_FAIL);
        }
    }

    public boolean setStation(int freq){
        boolean bStatus = false;
        if ( mReceiver != null ){
            LogUtils.logi("tune specific freq : " + freq);
            bStatus = mReceiver.setStation(freq);
            LogUtils.logi("Set Station result : " + bStatus);
        }
        return bStatus;
    }

    public void setAudio() {
        AudioManager audioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        setVolumeControlStream(AudioManager.STREAM_MUSIC);
        audioManager.setMode(AudioManager.MODE_NORMAL);
        AudioSystem.setForceUse(AudioSystem.FOR_MEDIA, AudioSystem.FORCE_NONE);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        mContext = this;
        requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
        super.onCreate(savedInstanceState);

        registerHeadsetListener();

        mFreqConfig = Float.valueOf(mTestCase.getParameter().get("frequency"));
        mFrequency = mFreqConfig;
        mTimeout = Integer.valueOf(mTestCase.getParameter().get("timeout"));
        LogUtils.logi("frequency : " + mFreqConfig);
        LogUtils.logi("timeout : " + mTimeout);

        setAudio();
        bindView();

        mIsTimeOut = false;

        boolean enabled = enableFmReceiver();
        if (!enabled) {
            showFmReceiverDialog();
            updateResult(TestCase.STATE_FAIL);
        }
    }

    public boolean enableFmReceiver() {
        boolean bStatus = false;
        if (mReceiver == null) {
            try {
                mReceiver = new FmReceiver(RADIO_DEVICE, mFmCallback);
                LogUtils.logd("Init FmReceiver:" + (mReceiver != null));
            } catch (InstantiationException e) {
                LogUtils.loge("enable fail: " + e);
            }

            if (mReceiver != null){

                if (mReceiver.isCherokeeChip()) {
                    bStatus = enableFMCherokee();
                } else{
                    bStatus = enableFM();
                }
            }
        }
        return bStatus;
    }

    private boolean enableFMCherokee(){
        LogUtils.logi("Enable FmReceiver");
        boolean bStatus = false;

        // Send command to power up SB slave block
        mEventReceived = false;
        mReceiver.EnableSlimbus(ENABLE_SLIMBUS_CLOCK_DATA);
        bStatus = waitForEvent();

        // Send command to enable FM core
        mEventReceived = false;
        mReceiver.EnableSlimbus(ENABLE_SLIMBUS_DATA_PORT);
        bStatus = waitForEvent();

        FmConfig mFmConfig = getFmDefConfig();
        mEventReceived = false;
        boolean ret = mReceiver.enable(mFmConfig, this);
        bStatus = waitForEvent();
        LogUtils.logi("Cherokee enable : " + ret);

        return bStatus;
    }

    private boolean enableFM(){
        LogUtils.logi("Enable FmReceiver");
        boolean bStatus = false;
        FmConfig mFmConfig = getFmDefConfig();
        mEventReceived = false;
        bStatus = mReceiver.enable(mFmConfig, this);
        LogUtils.logi("Enable : " + bStatus);
        return bStatus;
    }

    private FmConfig getFmDefConfig() {
        FmConfig fmConfig = new FmConfig();
        fmConfig.setRadioBand(FmTransceiver.FM_US_BAND);
        fmConfig.setEmphasis(FmTransceiver.FM_DE_EMP75);
        fmConfig.setChSpacing(FmReceiver.FM_CHSPACE_100_KHZ);
        fmConfig.setRdsStd(FmTransceiver.FM_RDS_STD_RBDS);
        fmConfig.setLowerLimit(87500);
        fmConfig.setUpperLimit(107900);
        return fmConfig;
    }

    private void showHeadSetDialog(){
        if (mHeadSetDialog == null || !mHeadSetDialog.isShowing()){
            mHeadSetDialog = new AlertDialog.Builder(mContext)
                    .setTitle(getString(R.string.insert_headset))
                    .setPositiveButton(getString(R.string.ok), null).setCancelable(false).show();
        }
    }

    private void cancelHeadSetDialog(){
        if (mHeadSetDialog != null){
            mHeadSetDialog.cancel();
            mHeadSetDialog = null;
        }
    }

    private void showFmReceiverDialog(){
        if (mFmReceiverDialog == null || !mFmReceiverDialog.isShowing()){
            mFmReceiverDialog = new AlertDialog.Builder(mContext)
                    .setTitle(getString(R.string.fm_enable_fail))
                    .setPositiveButton(getString(R.string.ok), null).setCancelable(false).show();
        }
    }

    private void cancelFmReceiverDialog(){
        if (mFmReceiverDialog != null){
            mFmReceiverDialog.cancel();
            mFmReceiverDialog = null;
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    /**
     * Registers an intent to listen for ACTION_HEADSET_PLUG
     * notifications. This intent is called to know if the headset
     * was plugged in/out
     */
    public void registerHeadsetListener() {
        if (mHeadsetReceiver == null) {
            mHeadsetReceiver = new BroadcastReceiver() {

                @Override
                public void onReceive(Context context, Intent intent) {
                    //TODO Auto-generated method stub
                    String action = intent.getAction();
                    LogUtils.logi("on receive HeadsetListener : " + action);
                    if (Intent.ACTION_HEADSET_PLUG.equals(action)){
                        LogUtils.logi("ACTION_HEADSET_PLUG Intent received");
                        LogUtils.logi(" state: " + intent.getIntExtra("state", 0));
                        mHeadsetPlugged = (intent.getIntExtra("state", 0) == 1);
                        LogUtils.logi("mHeadsetPlugged : " + mHeadsetPlugged);

                        if (mIsFirstTimePlay && mFMReceiverEnable && mHeadsetPlugged) {
                            cancelHeadSetDialog();
                            playFM(false);
                        } else if (!mHeadsetPlugged){
                            LogUtils.logd(" Headset Plug out, need do some pause actions");
                            //If headset is plugged out.
                            setDeviceOutFmState(false);
                            showHeadSetDialog();

                            //TODO: need pause FM
                            cancelSearch();
                            enableSearchBtn(true);
                        }
                    }
                }
            };

            IntentFilter iFilter = new IntentFilter();
            iFilter.addAction(Intent.ACTION_HEADSET_PLUG);
            iFilter.addCategory(Intent.CATEGORY_DEFAULT);
            registerReceiver(mHeadsetReceiver, iFilter);
        }
    }

    public boolean cancelSearch()
    {
       boolean bCommandSent=false;
       if (mReceiver != null)
       {
          LogUtils.logi("cancelSearch");
          bCommandSent = mReceiver.cancelSearch();
       }
       return bCommandSent;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        fmOff();

        /* Unregister the headset Broadcase receiver */
        if (mHeadsetReceiver != null) {
            unregisterReceiver(mHeadsetReceiver);
            mHeadsetReceiver = null;
        }
    }

    private boolean fmOff() {
        boolean bStatue = false;

        if (mReceiver != null) {

            if (mFMReceiverEnable){
                setDeviceOutFmState(false);
            }

            if (mReceiver.isCherokeeChip()) {
                bStatue = fmOffImplCherokee();
            } else {
                bStatue = fmOffImpl();
            }
        }
        return bStatue;
    }

    private boolean fmOffImplCherokee() {
        boolean bStatus=false;
        if (mReceiver != null && mReceiver.getFMState() != mReceiver.FMState_Turned_Off){
           mEventReceived = false;
           bStatus = mReceiver.disable(this);
           if (bStatus &&
                   (mReceiver.getFMState() == mReceiver.subPwrLevel_FMTurning_Off)) {
               bStatus = waitForEvent();
           }
           mReceiver = null;
        }
        return(bStatus);
    }

    private boolean fmOffImpl(){
        boolean bStatus=false;

        // This will disable the FM radio device
        if (mReceiver != null && mReceiver.getFMState() != mReceiver.FMState_Turned_Off){
           bStatus = mReceiver.disable(this);
           mReceiver = null;
        }
        return(bStatus);
    }

    @Override
    protected int getLayoutId() {
        return R.layout.fm_act;
    }

    Handler mHandler = new Handler() {
        public void handleMessage(android.os.Message msg) {
            switch (msg.what) {
                case UPDATE_MAIN_VIEW:
                    LogUtils.logi("Time Out : " + mIsTimeOut);

                    if (mIsTimeOut == false) {
                        LogUtils.logi("Cancel Count Down.");
                        if ( mFmCountDown != null ) mFmCountDown.cancel();
                        updateResult(TestCase.STATE_PASS);
                    } else {
                        updateResult(TestCase.STATE_FAIL);
                    }

                    updateView(mFrequency + " MHZ");
                    enableSearchBtn(true);
                    break;
                case TIME_OUT:
                    updateResult(TestCase.STATE_FAIL);
                default:
                    break;
            }
        }
    };

    public void updateView(String message) {
        // TODO Auto-generated method stub
        Bundle bundle = new Bundle();
        bundle.putString(Utils.BUNDLE_KEY_CASE_NAME, mTestCase.getName());
        bundle.putString(Utils.BUNDLE_KEY_MESSAGE, message);
        super.updateView(bundle);
    }

    public boolean searchUPOrDown(boolean isUp) {
        boolean bStatus = searchStation(isUp);
        if (bStatus){
            mIsTimeOut = false;
            new countDownThread().start();
            enableSearchBtn(false);
        }
        return bStatus;
    }

    public boolean searchStation(boolean isUp){
        boolean ret = false;
        if ( mReceiver != null ){
            int searchDir = isUp? FmReceiver.FM_RX_SEARCHDIR_UP : FmReceiver.FM_RX_SEARCHDIR_DOWN;
            LogUtils.logi("search direction : " + searchDir);
            ret = mReceiver.searchStations(FmReceiver.FM_RX_SRCH_MODE_SEEK,
                  FmReceiver.FM_RX_DWELL_PERIOD_1S,
                  searchDir);
            LogUtils.logi("Search Station result : " + ret);
        }
        return ret;
    }

    private void enableSearchBtn(boolean enable){
        int resId = enable? R.string.fm_search : R.string.fm_searching; 
        mSearchBtn.setClickable(enable);
        mSearchBtn.setText(resId);
    }

    private void setDeviceOutFmState(boolean available) {
        LogUtils.logi("Device OUT FM connection State: " + available);
        int state = available ? AudioSystem.DEVICE_STATE_AVAILABLE : AudioSystem.DEVICE_STATE_UNAVAILABLE;
        AudioSystem.setDeviceConnectionState(AudioSystem.DEVICE_OUT_FM, state, "", "");
    }

    private class countDownThread extends Thread{
        public countDownThread(){}

        @Override
        public void run() {
            // TODO Auto-generated method stub
            Looper.prepare();
            mFmCountDown = new FmCountDown();
            mFmCountDown.start();
            Looper.loop();
        }
    }

    private class FmCountDown extends CountDownTimer{

        public FmCountDown() {
            super(mTimeout,mTimeout);
        }

        @Override
        public void onFinish() {
            // TODO Auto-generated method stub
            LogUtils.logi("Time Out:");
            mIsTimeOut = true;
            mHandler.sendEmptyMessage(TIME_OUT);
        }

        @Override
        public void onTick(long millisUntilFinished) {
            // TODO Auto-generated method stub
        }
    }

    private boolean waitForEvent() {
        boolean status = false;

        synchronized (mEventWaitLock) {
            LogUtils.logi("waiting for event");
            try {
                if (mEventReceived == false)
                    mEventWaitLock.wait(RADIO_TIMEOUT);
                if (mEventReceived == true)
                    status = true;
            } catch (IllegalMonitorStateException e) {
                LogUtils.loge("Exception caught while waiting for event");
                e.printStackTrace();
            } catch (InterruptedException ex) {
                LogUtils.loge("Exception caught while waiting for event");
                ex.printStackTrace();
            }
        }
        return status;
    }
}