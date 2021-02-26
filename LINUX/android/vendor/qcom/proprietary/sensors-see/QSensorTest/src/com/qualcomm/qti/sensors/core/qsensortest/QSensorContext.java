/*============================================================================
@file QSensorContext.java

@brief
Singleton class used to hold global variables so that they are not tied to any
particular activity or service.

Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

package com.qualcomm.qti.sensors.core.qsensortest;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.os.PowerManager;
import android.util.Log;

import com.qualcomm.qti.sensors.core.stream.EventSensorAdapter;
import com.qualcomm.qti.sensors.core.stream.SensorAdapter;
import com.qualcomm.qti.sensors.core.stream.TriggerSensorAdapter;

import java.io.IOException;
import java.util.Scanner;
import java.util.LinkedList;
import java.util.List;

public class QSensorContext {
    static public String TAG = "QSensorTest";
    static private PowerManager.WakeLock wakeLock = null;
    static private SensorManager sensorManager;
    static public boolean optimizePower = false;
    static private SuspendMonitor suspendMonitor = null;
    static private Context context = null;
    static private List<SensorAdapter> sensorAdapterList;

    static public boolean beepEnabled = false;
    static public boolean autoRetryTrigger = true;
    static public boolean AcquireLock = false;

    /*QTI Defined Sensor Types*/
    private static final int qSensorTypeBase = 33171000;
    public  static final int sensorTypeRGB = qSensorTypeBase + 13;
    public  static final int sensorTypeCCT = qSensorTypeBase + 20;

    static public void init(Context context) {
        QSensorContext.context = context;

        if (sensorManager == null)
            sensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);

        suspendMonitor = QSensorContext.suspendMonitor();

        String optimizePower = SettingsDatabase.getSettings().getProperty("optimize_power_def");
        QSensorContext.optimizePower = null != optimizePower && optimizePower.equals("1");

        wakeLock = wakeLock();
    }

    static public Context getContext() {
        if (context == null) {
            throw new NullPointerException("QSensorContext not initialized. context is null");
        }

        return context;
    }

    static public PowerManager.WakeLock wakeLock() {
        if(null == QSensorContext.wakeLock) {
            PowerManager pm = (PowerManager) QSensorContext.getContext().getSystemService(Context.POWER_SERVICE);
            QSensorContext.wakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK | PowerManager.ON_AFTER_RELEASE, "qsensortest_wl");

            String holdWL = SettingsDatabase.getSettings().getProperty("hold_wake_lock");
            if(null != holdWL && holdWL.equals("1"))
                QSensorContext.wakeLock.acquire();
        }

        return QSensorContext.wakeLock;
    }

    static public SensorManager sensorManager() {
        if (sensorManager == null) {
            throw new NullPointerException("sensorManager not initialized");
        }
        return sensorManager;
    }

    static public SuspendMonitor suspendMonitor() {
        if (suspendMonitor == null)
            return new SuspendMonitor();
        return suspendMonitor;
    }

    static public List<SensorAdapter> sensorAdapterList() {
        List<Sensor> sensors = sensorManager().getSensorList(Sensor.TYPE_ALL);
        if (sensorAdapterList == null) {
            sensorAdapterList = new LinkedList<SensorAdapter>();
            for (Sensor sensor : sensors) {
                SettingsDatabase.SensorSetting settings = SettingsDatabase.getSettings().getSensorSetting(sensor);
                if (settings.getEnableStreaming()) {
                    SensorAdapter adapter;
                    /*RGB and CCT gives 3 values for each sample at a time unlike to other onChange Sensors*/
                    if((sensor.getType() == sensorTypeRGB) || (sensor.getType() == sensorTypeCCT)) {
                        adapter = new SensorAdapter(sensor, sensorManager);
                    }
                    else if(SettingsDatabase.TriggerMode.OnChange == settings.getTriggerMode()) {
                        adapter = new EventSensorAdapter(sensor, sensorManager);
                    }
                    else if(SettingsDatabase.TriggerMode.OneShot == settings.getTriggerMode()) {
                        adapter = new TriggerSensorAdapter(sensor, sensorManager);;
                    } /*consider continuous mode and special mode as same to list Tilt and Stepdetector*/
                    else {
                        adapter = new SensorAdapter(sensor, sensorManager);
                    }
                    sensorAdapterList.add(adapter);
                }
            }
        }

        return sensorAdapterList;
    }

/*
*
* @param property name passed to getprop
*/
    static public String readProperty(String property) throws IOException {
        Process process = new ProcessBuilder("getprop", property).start();
        Scanner scanner = null;
        String line = "";
        try {
            scanner = new Scanner(process.getInputStream());
            line = scanner.nextLine();
        } finally {
            if (scanner != null) {
                scanner.close();
            }
       }
       if (line.equals("")) {
            line = "false";
       }
       return line;
    }
}
