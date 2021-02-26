/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Wifi;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Resources;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.utils.LogUtils;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import static com.qualcomm.qti.qmmi.utils.LogUtils.TAG;

public class WifiService extends BaseService {

    private static WifiManager mWifiManager = null;
    private static WifiManager.WifiLock mWifiLock;
    private static List<ScanResult> wifiScanResult;
    private final static int SCAN_INTERVAL = 4000;
    private final static int OUT_TIME = 30000;
    private TestCase mTestCase;


    public int onStartCommand(Intent intent, int flags, int startId) {
        LogUtils.logi("onStartCommand");
        mWifiManager = (WifiManager) this
                .getSystemService(WIFI_SERVICE);
        if (mWifiManager == null) {
            LogUtils.loge("No mWifiManager service here");
        }

        return super.onStartCommand(intent, flags, startId);
    }


    @Override
    public void register() {

    }

    @Override
    public int stop(TestCase testCase) {
        finish();
        return 0;
    }

    @Override
    public int run(TestCase testCase) {

        LogUtils.logi("SensorService service run");
        mTestCase = testCase;
        start();
        return 0;
    }

    private void start() {


        /** Keep Wi-Fi awake */
        mWifiLock = mWifiManager.createWifiLock(
                WifiManager.WIFI_MODE_SCAN_ONLY, TAG);
        if (false == mWifiLock.isHeld())
            mWifiLock.acquire();

        switch (mWifiManager.getWifiState()) {
            case WifiManager.WIFI_STATE_DISABLED:
                enableWifi(true);
                break;
            case WifiManager.WIFI_STATE_DISABLING:
                break;
            case WifiManager.WIFI_STATE_UNKNOWN:
                break;
            default:
                break;
        }

        /***register receiver*/
        IntentFilter mFilter = new IntentFilter();
        mFilter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
        mFilter.addAction(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
        this.getApplicationContext().registerReceiver(mReceiver, mFilter);
        if (mWifiManager.getWifiState() == WifiManager.WIFI_STATE_ENABLED)
            mWifiManager.startScan();
    }

    public void finish() {
    	LogUtils.logi("finish");
        try {
            this.getApplicationContext().unregisterReceiver(mReceiver);
        } catch (Exception e) {
            LogUtils.logi("Error" + e);
        }

        if (true == mWifiLock.isHeld())
            mWifiLock.release();
    }

    private static void enableWifi(boolean enable) {

        if (mWifiManager != null)
            mWifiManager.setWifiEnabled(enable);
    }

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {

        public void onReceive(Context c, Intent intent) {

            LogUtils.logi(intent.getAction() + " wifiState="
                    + mWifiManager.getWifiState());
            if (WifiManager.SCAN_RESULTS_AVAILABLE_ACTION.equals(intent
                    .getAction())) {

                wifiScanResult = mWifiManager.getScanResults();
                if (wifiScanResult != null && wifiScanResult.size() > 0) {
                    Map<String, ScanResult> resultMap = new HashMap<String, ScanResult>();

                    for (ScanResult sr : wifiScanResult) {
                        LogUtils.logi(sr.toString());
                        if(sr.SSID != null) {
                            resultMap.put(sr.SSID, sr);
                        }
                    }

                    StringBuffer sb = new StringBuffer();
                    Resources resources = getApplicationContext().getResources();
                    sb.append(resources.getString(R.string.wifi_device_list));
                    sb.append("\n");

                    Set<String> keys = resultMap.keySet();
                    for(String key: keys){
                        ScanResult sr = resultMap.get(key);
                        sb.append("\n")
                            .append(sr.SSID).append("\n")
                            .append(resources.getString(R.string.wifi_level)).append(sr.level).append(", ")
                            .append(resources.getString(R.string.wifi_freq)).append(sr.frequency).append("\n");
                        mTestCase.addTestData(sr.SSID, "Channel:" + sr.channelWidth + ",Signal level:" + sr.level);
                    }

                    updateView(mTestCase.getName(), sb.toString());
                    updateResultForCase(mTestCase.getName(),TestCase.STATE_PASS);
                } else {
                    updateResultForCase(mTestCase.getName(),TestCase.STATE_FAIL);
                }

            } else if (WifiManager.WIFI_STATE_CHANGED_ACTION.equals(intent
                    .getAction())) {
                if (mWifiManager.getWifiState() == WifiManager.WIFI_STATE_ENABLED)
                    mWifiManager.startScan();
            }
        }

    };

}
