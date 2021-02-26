/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.SystemInfo;

import android.bluetooth.BluetoothAdapter;
import android.content.res.Resources;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.telephony.TelephonyManager;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.model.HidlManager;
import com.qualcomm.qti.qmmi.service.diag.MmiDiagJNIInterface;
import com.qualcomm.qti.qmmi.utils.LogUtils;

public class SystemInfoService extends BaseService {
    /**
     * Get IMEI
     */
    public String getIMEI(int slotIndex) {
        TelephonyManager telephonyManager = (TelephonyManager) this.getSystemService(TELEPHONY_SERVICE);
        return telephonyManager.getImei(slotIndex);
    }

    /**
     * Get system version
     */
    private String getSystemVersion() {
        return android.os.Build.VERSION.RELEASE;
    }

    /**
     * Get bluetooth_act address
     */
    private String getBluetoothAddress() {
        BluetoothAdapter bluetooth = BluetoothAdapter.getDefaultAdapter();
        if (bluetooth != null) {
            return bluetooth.getAddress();
        }

        return null;
    }


    /**
     * Get wifi mac address
     */
    private String getWifiMacAddress() {
        WifiManager mWifiManager = (WifiManager) getSystemService(WIFI_SERVICE);
        if (mWifiManager != null) {
            WifiInfo wifiInfo = mWifiManager.getConnectionInfo();
            if (wifiInfo != null) {
                return wifiInfo.getMacAddress();
            }
        }

        return null;
    }

    /**
     * Get diag support status
     */
    private String diagSupportStatus() {
        return MmiDiagJNIInterface.isMmiJniLoaded ? this.getResources().getString(R.string.systemInfo_support)
                : this.getResources().getString(R.string.systemInfo_not_support);
    }

    @Override
    public void register() {

    }

    @Override
    public int stop(TestCase testCase) {
        return 0;
    }

    @Override
    public int run(TestCase testCase) {

        HidlManager hidlManager = HidlManager.getInstance();

        //   hidlManager.chargerEnable(true);
        //  hidlManager.enterShipMode();

        //  hidlManager.getSmbStatus();
        //hidlManager.chargerEnable(false);
        //hidlManager.enterShipMode();
        hidlManager.wifiEnable(true);
        LogUtils.logi("SystemInfo service run");
        StringBuffer sb = new StringBuffer();
        Resources resources = this.getResources();

        sb.append(resources.getString(R.string.systemInfo_version)).append(getSystemVersion()).append("\n")
                .append(resources.getString(R.string.systemInfo_modem)).append(Build.getRadioVersion()).append("\n")
                .append(resources.getString(R.string.systemInfo_serial)).append(Build.SERIAL).append("\n")
                .append(resources.getString(R.string.systemInfo_imei1)).append(getIMEI(0)).append("\n")
                .append(resources.getString(R.string.systemInfo_imei2)).append(getIMEI(1)).append("\n")
                .append(resources.getString(R.string.systemInfo_bt_address)).append(getBluetoothAddress()).append("\n")
                .append(resources.getString(R.string.systemInfo_wifi_mac)).append(getWifiMacAddress()).append("\n")
                .append(resources.getString(R.string.systemInfo_diag_support)).append(diagSupportStatus()).append("\n");

        updateView(testCase.getName(), sb.toString());
        testCase.addTestData("version", getSystemVersion());
        testCase.addTestData("modem", Build.getRadioVersion());
        testCase.addTestData("serial", Build.SERIAL);
        testCase.addTestData("imei1", getIMEI(0));
        testCase.addTestData("imei2", getIMEI(1));
        testCase.addTestData("bt_address", getBluetoothAddress());
        testCase.addTestData("wifi_mac", getWifiMacAddress());
        testCase.addTestData("diag_support", diagSupportStatus());
        updateResultForCase(testCase.getName(), TestCase.STATE_PASS);

        return 0;
    }

}
