/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.qmmi.utils;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import android.hardware.Sensor;

public class SensorCalUtils {

    public static final int CAL_RESULT_PASS = 1;
    public static final int CAL_RESULT_FAIL = 0;
    public static final int CAL_RESULT_UNSUPPORTED = -1;
    public static final int CAL_INTERNAL_ERROR = -999;
    public static final int CAL_TEST_TYPE_SW = 0;
    public static final int CAL_TEST_TYPE_HW = 1;
    public static final int CAL_TEST_TYPE_OEM = 2;
    public static final int CAL_TEST_TYPE_COM = 3;

    public static String calibrateSensor(Sensor sensor, int testType){

        LogUtils.logi("Start to calibrate");

        if (getSCalInstance() != null){
            LogUtils.logi("Calibrate Sensor by HIDL");
            return calSensorByHidl(sensor, testType);
        }
        return " :" + CAL_INTERNAL_ERROR;
    }

    private static Object getSCalInstance() {
        Object instance = null;

        try {
            Class<?> clazz = Class.forName("vendor.qti.hardware.sensorscalibrate.V1_0.ISensorsCalibrate");
            Method getService= clazz.getMethod("getService", null);
            instance = getService.invoke(new Object(), new Object[]{});
            LogUtils.logi("get Sensor Calibrate Instance");
        } catch (ClassNotFoundException e) {
            // TODO Auto-generated catch block
            LogUtils.loge("NoClassDefFoundError occur for ISensorsCalibrate!");
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            // TODO Auto-generated catch block
            LogUtils.loge("NoClassDefFoundError occur for ISensorsCalibrate.getService()!");
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            // TODO Auto-generated catch block
            LogUtils.loge("get ISensorsCalibrate Instance failed!");
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            // TODO Auto-generated catch block
            LogUtils.loge("get ISensorsCalibrate Instance failed!");
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            // TODO Auto-generated catch block
            LogUtils.loge("get ISensorsCalibrate Instance failed!");
            e.printStackTrace();
        }

        return instance;
    }

    private static String calSensorByHidl(Sensor sensor, int testType){

        Object ISensorsCalibrate = getSCalInstance();
        String ret = null;

        if (ISensorsCalibrate != null){
            try {
                Class<?> instance = ISensorsCalibrate.getClass();
                Method sensorCal = instance.getMethod("SensorsCal", int.class, byte.class);
                LogUtils.logi("Calibrate Sensor Type : " + sensor.getType() + " Test Type : " + testType);
                Object result = sensorCal.invoke(ISensorsCalibrate, sensor.getType(), (byte)testType);
                ret = String.valueOf(result);
                LogUtils.logi("Calibrate Sensor Result : " + ret);
            } catch (NoSuchMethodException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } catch (IllegalAccessException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } catch (IllegalArgumentException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } catch (InvocationTargetException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        } else {
            LogUtils.loge("the sensorscalibrate hidl not exist");
        }
        return ret;
    }
}
