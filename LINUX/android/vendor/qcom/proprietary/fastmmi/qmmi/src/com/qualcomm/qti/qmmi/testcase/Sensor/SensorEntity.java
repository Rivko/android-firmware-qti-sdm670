/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.qmmi.testcase.Sensor;

import java.lang.reflect.Field;
import java.util.HashMap;
import java.util.List;

import com.qualcomm.qti.qmmi.utils.LogUtils;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorManager;

public class SensorEntity {

    //Map for sensor values' available length
    private final static HashMap<Integer,Integer> RESULT_LENGTH_MAP = new HashMap<Integer,Integer>();
    {
        RESULT_LENGTH_MAP.put(Sensor.TYPE_LIGHT, NUM_1);
        RESULT_LENGTH_MAP.put(Sensor.TYPE_PRESSURE, NUM_1);
        RESULT_LENGTH_MAP.put(Sensor.TYPE_PROXIMITY, NUM_1);
        RESULT_LENGTH_MAP.put(Sensor.TYPE_RELATIVE_HUMIDITY, NUM_1);
        RESULT_LENGTH_MAP.put(Sensor.TYPE_AMBIENT_TEMPERATURE, NUM_1);
        RESULT_LENGTH_MAP.put(Sensor.TYPE_STATIONARY_DETECT, NUM_1);
        RESULT_LENGTH_MAP.put(Sensor.TYPE_MOTION_DETECT, NUM_1);
        RESULT_LENGTH_MAP.put(Sensor.TYPE_HEART_BEAT, NUM_1);
        RESULT_LENGTH_MAP.put(TYPE_HALL_EFFECT, NUM_1);

        RESULT_LENGTH_MAP.put(Sensor.TYPE_ACCELEROMETER, NUM_3);
        RESULT_LENGTH_MAP.put(Sensor.TYPE_MAGNETIC_FIELD, NUM_3);
        RESULT_LENGTH_MAP.put(Sensor.TYPE_GYROSCOPE, NUM_3);
        RESULT_LENGTH_MAP.put(Sensor.TYPE_GRAVITY, NUM_3);
        RESULT_LENGTH_MAP.put(Sensor.TYPE_LINEAR_ACCELERATION, NUM_3);
        RESULT_LENGTH_MAP.put(Sensor.TYPE_ORIENTATION, NUM_3);

        RESULT_LENGTH_MAP.put(Sensor.TYPE_MAGNETIC_FIELD_UNCALIBRATED, NUM_6);
        RESULT_LENGTH_MAP.put(Sensor.TYPE_GYROSCOPE_UNCALIBRATED, NUM_6);
    }

    //sensor type
    private final static String HALL_EFFECT_STRING_TYPE_KEY = "hall_effect";
    private final static int TYPE_HALL_EFFECT = 33171016;
    private static int mHallEffectSensorType = TYPE_HALL_EFFECT;
    protected final static int TYPE_UNSUPPORT = -99999;

    private final static String SUFFIX_CAL = "_cal";
    //category for sensor case
    protected final static int CAT_SENSOR = 1;
    protected final static int CAT_SENSOR_CAL = 2;

    //Length of Sensor available values
    protected final static int NUM_1 = 1;
    protected final static int NUM_3 = 3;
    protected final static int NUM_6 = 6;

    private String typeName;
    private int type;
    private int category;
    private int valuesNum;
    private Context mContext;

    public SensorEntity(Context context, String typeName){
        mContext = context;
        setTypeName(typeName);
    }

    private void genEntity(String typeName){

        int genType = genType(typeName);
        setType(genType);
        int genCategory = genCategory(typeName);
        LogUtils.logi( "category : " + genCategory);
        setCategory(genCategory);
        int genNum = genValuesNum(genType);
        LogUtils.logi( "valuesNum : " + genNum);
        setValuesNum(genNum);
    }

    private int genType(String typeName){

        //for some specific/custom sensor type
        if ( typeName.equalsIgnoreCase(HALL_EFFECT_STRING_TYPE_KEY)) {
            SensorManager sensorManager =  (SensorManager) mContext.getSystemService(mContext.SENSOR_SERVICE);
            List<Sensor> list = sensorManager.getSensorList(Sensor.TYPE_ALL);
            for (int i = 0; i < list.size(); i++) {
                Sensor s = list.get(i);
                if (s.getStringType() != null && s.getStringType().contains(HALL_EFFECT_STRING_TYPE_KEY)) {
                    mHallEffectSensorType = s.getType();
                    RESULT_LENGTH_MAP.put(mHallEffectSensorType, NUM_1);
                    return mHallEffectSensorType;
                }
            }
            return TYPE_HALL_EFFECT;
        }

        int result = TYPE_UNSUPPORT;

        LogUtils.logi( "typeName : " + typeName);
        if (typeName.endsWith(SUFFIX_CAL)) typeName = typeName.replace(SUFFIX_CAL, "");

        Class sensor = Sensor.class;
        Field field;
        String fieldName = "TYPE_" + typeName.toUpperCase();
        try {
            field = sensor.getField(fieldName);
            result = field.getInt(sensor);
        } catch (NoSuchFieldException e) {
            LogUtils.loge( fieldName + " can not find!!");
        } catch (IllegalAccessException e) {
            // TODO Auto-generated catch bloc
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        LogUtils.logi( "Sensor type value : " + result + " for " + fieldName);
        return result;
    }

    private int genCategory(String typeName){
        int result = CAT_SENSOR;

        if (typeName.endsWith(SUFFIX_CAL)) result = CAT_SENSOR_CAL;

        return result;
    }

    private int genValuesNum(int type){
        if ( !RESULT_LENGTH_MAP.containsKey(type)){
            return NUM_3;
        } else {
            return RESULT_LENGTH_MAP.get(type);
        }
    }

    public String getTypeName() {
        return typeName;
    }

    public void setTypeName(String name){
        this.typeName = name;
        genEntity(typeName);
    }

    public int getType() {
        return type;
    }

    private void setType(int type){
        this.type = type;
    }

    public int getCategory() {
        return category;
    }

    private void setCategory(int category){
        this.category = category;
    }

    public int getValuesNum() {
        return valuesNum;
    }

    private void setValuesNum(int valuesNum){
        this.valuesNum = valuesNum;
    }
}
