/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Gps;

import android.Manifest;
import android.app.Service;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.location.Criteria;
import android.location.GnssStatus;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.os.Looper;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.utils.LogUtils;

import java.util.ArrayList;
import java.util.List;

public class GpsService extends BaseService {

    private final static int MSG_TIME_OUT = 0;

    private LocationManager mLocationManager = null;
    private TestCase mTestCase;

    private int mTimeout = 0;
    private GpsCountDown mGpsCountDown = null;
    private boolean hasResult = false;

    LocationListener mLocationListener = new LocationListener() {

        public void onLocationChanged(Location location) {
            LogUtils.logi("onLocationChanged callback ");
        }

        public void onProviderDisabled(String provider) {
            LogUtils.logi("onProviderDisabled callback ");
        }

        public void onProviderEnabled(String provider) {
            LogUtils.logi("onProviderEnabled callback ");
        }

        public void onStatusChanged(String provider, int status, Bundle extras) {
            LogUtils.logi("onStatusChanged callback ");
        }
    };

    private Handler mHandler = new Handler() {
        public void handleMessage(android.os.Message msg) {
            switch (msg.what) {
                case MSG_TIME_OUT:
                    //Set case fail.
                    if (hasResult) {
                        updateResultForCase(mTestCase.getName(), TestCase.STATE_PASS);
                    } else {
                        updateResultForCase(mTestCase.getName(),TestCase.STATE_FAIL);
                    }
                default:
                    break;
            }
        }
    };

    GnssStatus.Callback gnssStatusCallback = new GnssStatus.Callback() {
        @Override
        public void onSatelliteStatusChanged(GnssStatus status) {
            LogUtils.logi("status satelie count: " + status.getSatelliteCount());
            StringBuffer sb = new StringBuffer();

            int count = status.getSatelliteCount();
            hasResult = count > 0 ? true : false ;

            sb.append(getApplication().getResources().getString(R.string.get_satelite_count) + count + "\n");
            for (int i = 0; i < status.getSatelliteCount(); ++i) {
                status.getAzimuthDegrees(i);
                status.getCn0DbHz(i);
                status.getConstellationType(i);
                status.getElevationDegrees(i);
                status.getSvid(i);
                status.hasAlmanacData(i);
                status.hasEphemerisData(i);
                status.usedInFix(i);
                sb.append("Svid(" + status.getSvid(i) + "): " + status.getCn0DbHz(i) + "dB\n");
                LogUtils.logi("status : " + status.getAzimuthDegrees(i) + "," + status.getCn0DbHz(i) + "," + status.getConstellationType(i) + "," + status.getElevationDegrees(i) + "," + status.hasAlmanacData(i));

                mTestCase.addTestData(String.valueOf(status.getSvid(i)), "cn0DbHz:" + status.getCn0DbHz(i));
            }

            if (count > 0) {
                updateResultForCase(mTestCase.getName(), TestCase.STATE_PASS);
            } else {
                updateResultForCase(mTestCase.getName(),TestCase.STATE_FAIL);
            }

            updateView(mTestCase.getName(), sb.toString());
        }
    };

    @Override
    public void register() {

    }

    public int onStartCommand(Intent intent, int flags, int startId) {

        mLocationManager = (LocationManager) getSystemService(LOCATION_SERVICE);
        if (mLocationManager != null) {
            LogUtils.loge("Get mLocationManager failed ");
        }
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void onDestroy() {

        super.onDestroy();
    }

    @Override
    public int stop(TestCase testCase) {

        if (mLocationManager != null) {
            mLocationManager.unregisterGnssStatusCallback(gnssStatusCallback);
            mLocationManager.removeUpdates(mLocationListener);
        }

        if ( mGpsCountDown != null ) mGpsCountDown.cancel();
        return 0;
    }

    @Override
    public int run(TestCase testCase) {
        LogUtils.logi("gps service run");
        mTestCase = testCase;
        mTimeout = Integer.valueOf(mTestCase.getParameter().get("timeout"));

        if (this.checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED ||
                this.checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            return Service.START_NOT_STICKY;
        }

        if (mLocationManager != null) {
            mLocationManager.registerGnssStatusCallback(gnssStatusCallback);
            Criteria criteria;
            criteria = new Criteria();
            criteria.setAccuracy(Criteria.ACCURACY_FINE);
            criteria.setAltitudeRequired(false);
            criteria.setBearingRequired(false);
            criteria.setCostAllowed(true);
            criteria.setPowerRequirement(Criteria.POWER_LOW);

            String provider = mLocationManager.getBestProvider(criteria, true);
            if (provider != null) {
                mLocationManager.requestLocationUpdates(provider, 500, 0, mLocationListener);
                updateView(mTestCase.getName(), getApplicationContext().getString(R.string.gps_searching));

                if (mTimeout > 0){
                    LogUtils.logi("Start to count down to scan BT devices");
                    new countDownThread().start();
                }
            }
        }

        return 0;
    }

    private class countDownThread extends Thread{
        public countDownThread(){}

        @Override
        public void run() {
            // TODO Auto-generated method stub
            Looper.prepare();
            mGpsCountDown = new GpsCountDown();
            mGpsCountDown.start();
            Looper.loop();
        }
    }

    private class GpsCountDown extends CountDownTimer{

        public GpsCountDown() {
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
