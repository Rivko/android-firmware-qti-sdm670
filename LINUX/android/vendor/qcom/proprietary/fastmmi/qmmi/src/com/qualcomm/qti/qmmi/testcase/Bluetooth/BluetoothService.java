/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Bluetooth;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import com.qualcomm.qti.qmmi.bean.DeviceInfo;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.Utils;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.os.Looper;
import android.os.Parcelable;

import com.qualcomm.qti.qmmi.R;

public class BluetoothService extends BaseService {

    private final static int MIN_COUNT = 1;
    private final static int MSG_TIME_OUT = 0;

    BluetoothAdapter mBluetoothAdapter = null;
    private TestCase mTestCase;
    private static int mTimeout = 0;
    IntentFilter filter;
    private BTCountDown mBTCountDown = null;

    Map<String,DeviceInfo> mDeviceMap = new HashMap<String,DeviceInfo>();

    BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {

            String action = intent.getAction();
            LogUtils.logi("onReceive get intent action:" + action);
            BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
            int rssi = (int)intent.getShortExtra(BluetoothDevice.EXTRA_RSSI, Short.MIN_VALUE);

            if (BluetoothDevice.ACTION_FOUND.equals(action)) {

                String name = device.getName();
                String address = device.getAddress();

                LogUtils.logi("onReceive ACTION_FOUND add device to device list - device name : " + name
                        + " : device address : " + address + " : rssi : " + rssi);

                String key = name == null ? address : name;
                LogUtils.logi("key:" + key);

                mDeviceMap.put(key, new DeviceInfo(key, device.getAddress(), rssi));

                mTestCase.addTestData(key, "rssi:" + rssi);

                if (mDeviceMap.size() >= MIN_COUNT) {
                    updateUI(context.getString(R.string.bluetooth_found_device));
                }

            } else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {
                LogUtils.logi("onReceive ACTION_DISCOVERY_FINISHED");
                if (mDeviceMap.size() < MIN_COUNT) {
                    updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
                    updateUI(context.getString(R.string.bluetooth_scan_null));
                } else {
                    updateResultForCase(mTestCase.getName(), TestCase.STATE_PASS);
                }
            } else if (BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action)) {
                LogUtils.logi("onReceive ACTION_DISCOVERY_STARTED");

                if (mTimeout > 0){
                    LogUtils.logi("Start to count down to scan BT devices");
                    new countDownThread().start();
                }

            } else if (BluetoothAdapter.ACTION_STATE_CHANGED.equals(action)) {
                LogUtils.logi("onReceive ACTION_STATE_CHANGED");
                if (BluetoothAdapter.STATE_ON == intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, 0)) {
                    scanDevice();
                    if (BluetoothAdapter.STATE_OFF == intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, 0))
                        mBluetoothAdapter.enable();
                } else if (BluetoothAdapter.STATE_TURNING_ON == intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, 0)) {
                    updateUI(context.getString(R.string.bluetooth_turning_on));
                }
            }
        }
    };

    private Handler mHandler = new Handler() {
        public void handleMessage(android.os.Message msg) {
            switch (msg.what) {
                case MSG_TIME_OUT:
                    //Set case fail.
                    if (mDeviceMap.size() < MIN_COUNT) {
                        updateResultForCase(mTestCase.getName(), TestCase.STATE_FAIL);
                    } else {
                        updateResultForCase(mTestCase.getName(), TestCase.STATE_PASS);
                    }
                default:
                    break;
            }
        }
    };

    private void updateUI(String msg){
        Bundle bundle = new Bundle();

        List<DeviceInfo> deviceList = new ArrayList<DeviceInfo>();
        Set<String> keys = mDeviceMap.keySet();
        for(String key: keys){
            deviceList.add(mDeviceMap.get(key));
        }

        bundle.putParcelableArrayList("BluetoothList", (ArrayList<? extends Parcelable>) deviceList);
        bundle.putString(Utils.BUNDLE_KEY_MESSAGE, msg);
        bundle.putString(Utils.BUNDLE_KEY_CASE_NAME, mTestCase.getName());
        updateView(bundle);
    }

    public int onStartCommand(Intent intent, int flags, int startId) {

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter != null) {
            LogUtils.loge("Get mBluetoothAdapter failed ");
        }
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void register() {
    }

    @Override
    public int run(TestCase testCase) {
        LogUtils.logi("Bluetooth Service run");

        mTestCase = testCase;
        mTimeout = Integer.valueOf(mTestCase.getParameter().get("timeout"));

        registerReceiver();

        if (mBluetoothAdapter.getState() == BluetoothAdapter.STATE_ON) {
            LogUtils.logi("BluetoothAdapter state is on, start to scan devices.");
            scanDevice();
        } else {
            LogUtils.logi("BluetoothAdapter state is off, start to turn on.");
            if (mBluetoothAdapter.getState() != BluetoothAdapter.STATE_TURNING_ON) {
                LogUtils.logi("BluetoothAdapter state is STATE_TURNING_ON, start to turn on.");
                mBluetoothAdapter.enable();
            }
        }

        return 0;
    }

    private void registerReceiver(){
        filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
        filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        this.getApplicationContext().registerReceiver(mReceiver, filter);
    }

    private void scanDevice() {
        updateUI(getString(R.string.bluetooth_scan_start));
        if (mBluetoothAdapter.isDiscovering()) {
            mBluetoothAdapter.cancelDiscovery();
        }
        mBluetoothAdapter.startDiscovery();
    }

    private void cancelScan() {
        LogUtils.logi("cancelScan");
        if (mBluetoothAdapter.isDiscovering()) {
            mBluetoothAdapter.cancelDiscovery();
        }
        mDeviceMap.clear();
    }

    @Override
    public int stop(TestCase testCase) {
        LogUtils.logi("Stop");

        try {
            if (null != mReceiver){
                this.getApplicationContext().unregisterReceiver(mReceiver);
            }
        } catch (Exception e) {
            LogUtils.logi("Error" + e);
        }

        cancelScan();
        if ( mBTCountDown != null ) mBTCountDown.cancel();

        return 0;
    }

    private class countDownThread extends Thread{
        public countDownThread(){}

        @Override
        public void run() {
            // TODO Auto-generated method stub
            Looper.prepare();
            mBTCountDown = new BTCountDown();
            mBTCountDown.start();
            Looper.loop();
        }
    }

    private class BTCountDown extends CountDownTimer{

        public BTCountDown() {
            super(mTimeout,mTimeout);
        }

        @Override
        public void onFinish() {
            // TODO Auto-generated method stub
            LogUtils.logi("Time Out:");
            mHandler.sendEmptyMessage(MSG_TIME_OUT);
        }

        @Override
        public void onTick(long millisUntilFinished) {
            // TODO Auto-generated method stub
        }
    }
}
