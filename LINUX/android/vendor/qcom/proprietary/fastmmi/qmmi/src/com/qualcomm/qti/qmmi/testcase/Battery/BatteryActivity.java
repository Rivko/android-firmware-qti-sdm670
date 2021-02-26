/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Battery;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Resources;
import android.os.BatteryManager;
import android.os.Bundle;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseActivity;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.Utils;

public class BatteryActivity extends BaseActivity {

    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String batteryStatusString = "";
            String batteryHealthString = "";
            String batteryACString = "";
            StringBuilder batteryReportString = new StringBuilder();

            String action = intent.getAction();
            if (action.equals(Intent.ACTION_BATTERY_CHANGED)) {
                int batteryStatus = intent.getIntExtra(BatteryManager.EXTRA_STATUS, 0);
                int batteryHealth = intent.getIntExtra(BatteryManager.EXTRA_HEALTH, 0);
                int batteryLevel = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, 0);
                int batteryScale = intent.getIntExtra(BatteryManager.EXTRA_SCALE, 0);
                int batteryIconSmall = intent.getIntExtra(BatteryManager.EXTRA_ICON_SMALL, 0);
                int batteryPlugged = intent.getIntExtra(BatteryManager.EXTRA_PLUGGED, 0);
                int batteryVoltage = intent.getIntExtra(BatteryManager.EXTRA_VOLTAGE, 0);
                int batteryTemperature = intent.getIntExtra(BatteryManager.EXTRA_TEMPERATURE, 0);
                boolean batteryPresent = intent.getBooleanExtra(BatteryManager.EXTRA_PRESENT, false);
                String batteryTechnology = intent.getStringExtra(BatteryManager.EXTRA_TECHNOLOGY);

                Resources resource = getApplicationContext().getResources();
                switch (batteryStatus) {
                    case BatteryManager.BATTERY_STATUS_CHARGING:
                        batteryStatusString = resource.getString(R.string.battery_status_charging);
                        break;
                    case BatteryManager.BATTERY_STATUS_DISCHARGING:
                        batteryStatusString = resource.getString(R.string.battery_status_discharging);
                        break;
                    case BatteryManager.BATTERY_STATUS_NOT_CHARGING:
                        batteryStatusString = resource.getString(R.string.battery_status_not_charging);
                        break;
                    case BatteryManager.BATTERY_STATUS_FULL:
                        batteryStatusString = resource.getString(R.string.battery_status_full);
                        break;
                    case BatteryManager.BATTERY_STATUS_UNKNOWN:
                        batteryStatusString = resource.getString(R.string.battery_status_unknown);
                        break;
                }

                switch (batteryHealth) {
                    case BatteryManager.BATTERY_HEALTH_GOOD:
                        batteryHealthString = resource.getString(R.string.battery_health_good);
                        break;
                    case BatteryManager.BATTERY_HEALTH_OVERHEAT:
                        batteryHealthString = resource.getString(R.string.battery_health_overheat);
                        break;
                    case BatteryManager.BATTERY_HEALTH_DEAD:
                        batteryHealthString = resource.getString(R.string.battery_health_dead);
                        break;
                    case BatteryManager.BATTERY_HEALTH_OVER_VOLTAGE:
                        batteryHealthString = resource.getString(R.string.battery_health_voltage);
                        break;
                    case BatteryManager.BATTERY_HEALTH_UNKNOWN:
                        batteryHealthString = resource.getString(R.string.battery_health_unknown);
                        break;
                    case BatteryManager.BATTERY_HEALTH_UNSPECIFIED_FAILURE:
                        batteryHealthString = resource.getString(R.string.battery_health_failure);
                        break;
                }

                if (batteryPlugged == BatteryManager.BATTERY_PLUGGED_AC)
                    batteryACString = resource.getString(R.string.battery_plugged_ac);
                else if (batteryPlugged == BatteryManager.BATTERY_PLUGGED_USB)
                    batteryACString = resource.getString(R.string.battery_plugged_usb);

                String batteryPresentString = batteryPresent? resource.getString(R.string.battery_present_true): resource.getString(R.string.battery_present_false);

                batteryReportString.append(resource.getString(R.string.battery_status)).append(batteryStatusString).append("\n")
                        .append(resource.getString(R.string.battery_present)).append(String.valueOf(batteryPresentString)).append("\n")
                        .append(resource.getString(R.string.battery_health)).append(batteryHealthString).append("\n")
                        .append(resource.getString(R.string.battery_level)).append(String.valueOf(batteryLevel)).append("\n")
                        .append(resource.getString(R.string.battery_scale)).append(String.valueOf(batteryScale)).append("\n")
                        .append(resource.getString(R.string.battery_plugged)).append(batteryACString).append("\n")
                        .append(resource.getString(R.string.battery_icon_small)).append(String.valueOf(batteryIconSmall)).append("\n")
                        .append(resource.getString(R.string.battery_voltage)).append(String.valueOf(batteryVoltage)).append("\n")
                        .append(resource.getString(R.string.battery_temperature)).append(String.valueOf(batteryTemperature)).append("\n")
                        .append(resource.getString(R.string.battery_technology)).append(batteryTechnology).append("\n");

                mTestCase.addTestData("status",batteryStatusString);
                mTestCase.addTestData("present",String.valueOf(batteryPresent));
                mTestCase.addTestData("level",String.valueOf(batteryLevel));
                mTestCase.addTestData("voltage",String.valueOf(batteryVoltage));
                mTestCase.addTestData("temperature",String.valueOf(batteryTemperature));

                LogUtils.logi("battery info: " + batteryReportString);

                updateView(batteryReportString.toString());
                updateResult(TestCase.STATE_PASS);
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

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    protected void onResume() {
        super.onResume();
        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_BATTERY_CHANGED);
        registerReceiver(mBroadcastReceiver, filter);
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(mBroadcastReceiver);
    }

    @Override
    protected int getLayoutId() {
        return R.layout.case_act;
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
    }

}
