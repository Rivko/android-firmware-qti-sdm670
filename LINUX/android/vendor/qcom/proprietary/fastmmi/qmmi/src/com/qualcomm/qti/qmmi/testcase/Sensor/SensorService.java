/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Sensor;

import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;

import java.io.Serializable;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Locale;
import java.util.Map;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.utils.FileUtils;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.SensorCalUtils;
import com.qualcomm.qti.qmmi.utils.NumericUtils;
import com.qualcomm.qti.qmmi.testcase.Sensor.SensorEntity;

public class SensorService extends BaseService {

    private static SensorManager mSensorManager = null;
    private static SensorListener mSensorListener = null;

    private HashMap<String, StringBuffer> mFileData = new HashMap<String, StringBuffer>();

    private HashMap<String, SensorEntity> mSensorEntityMap = new HashMap<String, SensorEntity>();

    private HashMap<String, SensorListener> mListenerMap = new HashMap<String, SensorListener>();
    private HashMap<String, String> mCalResultMap = new HashMap<String, String>();
    private HashMap<String, Long> mCaseFreqMap = new HashMap<String, Long>();
    private HashMap<String, SensorCountDown> mCountDownMap = new HashMap<String, SensorCountDown>();
    private HashMap<String, SensorData> mSensorDataMap = new HashMap<String, SensorData>();
    private HashMap<String, TestCase> mSensorCase = new HashMap<String, TestCase>();
    private Object mSensorDataWaitLock = new Object();

    private static final String TRACK_SEP_ITEMS = ";";
    private static final String TRACK_SEP_CONTENT = ",";
    private static final int DECIMALS = 5;
    private static final int PARSE_DATA = 0;

    public int onStartCommand(Intent intent, int flags, int startId) {
        LogUtils.logi( "onStartCommand");
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        if (mSensorManager == null) {
            LogUtils.loge( "No sensor service here !!");
        }

        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void register() {
    }

    @Override
    public int stop(TestCase testCase) {

        String name = testCase.getName();

        //Write Sensor Data into xxx.track file
        StringBuffer sb = mFileData.get(name);
        if (sb != null && sb.length() > 0){
            String sensorTraceFile = getTrackFileName(name);

            FileUtils.writeFile(sensorTraceFile, sb.toString());

            //Remove data that already recorded
            mFileData.remove(name);
        }

        if ( mListenerMap.containsKey(name)){
            mSensorManager.unregisterListener(mListenerMap.get(name));
            mListenerMap.remove(name);
            LogUtils.logi( "Unregister Listener for sensor case : " + name);
        }

        //Cancel countDownTimer
        if ( mCountDownMap.containsKey(name)){
            SensorCountDown down = mCountDownMap.get(name);
            if ( down != null) down.cancel();
            mCountDownMap.remove(name);
            LogUtils.logi( "Cancel CountDownTimer for sensor case : " + name);
        }

        mCalResultMap.remove(name);
        mSensorDataMap.remove(name);

        return 0;
    }

    @Override
    public int run(TestCase testCase) {

        String caseName = testCase.getName();
        String caseType = testCase.getParameter().get("type");

        mSensorCase.put(caseName, testCase);

        if ( caseType == null || caseType.equals("")) {
            LogUtils.loge( "SensorService : " + caseName + " 's type is NULL!");
            return -1;
        }

        SensorEntity entity = new SensorEntity(this,caseType);
        if ( !mSensorEntityMap.containsKey(caseName) ){
            mSensorEntityMap.put(caseName, entity);
        }

        String caseFreq = testCase.getParameter().get("freq");
        if ( caseFreq != null && !caseFreq.equals("") && NumericUtils.isPositiveInteger(caseFreq)){
            mCaseFreqMap.put(caseName, Long.valueOf(caseFreq));
        }

        int sensorType = entity.getType();

        LogUtils.logi( "Run SensorService : " + caseName + " param type : " + caseType + " sensor type : " + sensorType);

        if ( sensorType == SensorEntity.TYPE_UNSUPPORT) {
            LogUtils.loge( "Not support sensor type!please check");
            String error = getApplicationContext().getResources().getString(R.string.sensor_not_support);
            updateViewAndResult(caseName, error, false);
            return -1;
        }

        int category = entity.getCategory();

        Sensor sensor = mSensorManager.getDefaultSensor(sensorType);

        if (sensor == null) {
            LogUtils.loge( "No sensor type find for : " + sensorType);
            String error = getApplicationContext().getResources().getString(R.string.sensor_not_support);
            updateViewAndResult(caseName, error, false);
            return -1;
        }

        if ( category == SensorEntity.CAT_SENSOR_CAL) {
            //TODO: show content in UI
            String result = calibrateSensor(sensor);
            mCalResultMap.put(caseName, result);
            updateViewAndResultForCal(caseName, true);
        } else if ( category == SensorEntity.CAT_SENSOR) {
            if ( mListenerMap.containsKey(caseName) ){
                mSensorListener = mListenerMap.get(caseName);
            } else {
                mSensorListener = new SensorListener(caseName);
                mListenerMap.put(caseName, mSensorListener);
            }

            mSensorManager.registerListener(mSensorListener, sensor,
                    SensorManager.SENSOR_DELAY_NORMAL);
            StartCountDown(caseName);
        }

        return 0;
    }

    public class SensorListener implements SensorEventListener {

        private String caseName ;

        public SensorListener(String name) {
            super();
            caseName = name;
        }

        public void onSensorChanged(SensorEvent event) {

            LogUtils.logd(caseName + " changed sensor data");

            //check if listener unregister
            if ( !mListenerMap.containsKey(caseName)) return;

            //If this case has freq, store sensor data to handle with CountDownThread
            if ( mCaseFreqMap.containsKey(caseName)){
                synchronized(mSensorDataWaitLock) {
                    mSensorDataMap.put(caseName, new SensorData(event,false));
                }
            } else {
                //If this case has no freq, direct parse and show data
                parseData(caseName, event);
            }
        }

        public void onAccuracyChanged(Sensor arg0, int arg1) {
        }
    }

    private String parseData(String caseName, SensorEvent event){

        if ( caseName == null || caseName.equals("")) return null;
        if ( event == null) return null;
        if ( !mSensorEntityMap.containsKey(caseName) ) return null;

        String result = "";
        boolean caseState = false;

        SensorEntity entity = mSensorEntityMap.get(caseName);

        float[] values = event.values;
        LogUtils.logi( "caseName : " + caseName + " Value length : " + values.length);

        String[] verifyValues = new String[entity.getValuesNum()];
        StringBuffer newData = new StringBuffer();

        switch(entity.getValuesNum()){

            case SensorEntity.NUM_1 :

                boolean validateOne = false;

                if ( values.length < SensorEntity.NUM_1) {
                    result = this.getResources().getString(R.string.sensor_wrong_data_lenght);
                } else {
                    String resultOne = this.getResources().getString(R.string.sensor_result_one);

                    String valueOne1 = formatData(values[0]);
                    validateOne = valueOne1.equals("")? false:true;

                    result = String.format(resultOne, valueOne1);
                    newData.append(valueOne1).append(TRACK_SEP_ITEMS);
                }

                caseState = true;
                break;
            case SensorEntity.NUM_3 :

                boolean validateThree = false;

                if ( values.length < SensorEntity.NUM_3) {
                    result = this.getResources().getString(R.string.sensor_wrong_data_lenght);
                } else {
                    String resultThree = this.getResources().getString(R.string.sensor_result_three);

                    String valueThree1 = formatData(values[0]);
                    String valueThree2 = formatData(values[1]);
                    String valueThree3 = formatData(values[2]);
                    validateThree = ( valueThree1.equals("") && valueThree2.equals("") && valueThree3.equals("") )? false:true;

                    result = String.format(resultThree, valueThree1, valueThree2, valueThree3);

                    // Append Track Data
                    newData.append(valueThree1).append(TRACK_SEP_CONTENT)
                        .append(valueThree2).append(TRACK_SEP_CONTENT)
                        .append(valueThree3).append(TRACK_SEP_ITEMS);

                    // Append sensor value for Indication View in UI, just for 3 DOF value
                    result = result + ":" + event.sensor.getType() + ":"  + values[0] + ":" + values[1] + ":" + values[2];
                }

                caseState = true;
                break;
            case SensorEntity.NUM_6 :
                break;
            default :
                caseState = false;
                break;
        }

        prepareTrackData(caseName, newData);
        updateViewAndResult(caseName, result, caseState);

        LogUtils.logi( "result : " + result);
        LogUtils.logi( "caseState : " + caseState);
        return result;
    }

    private void prepareTrackData(String caseName, StringBuffer newData){

    if (newData == null || newData.length() == 0) return;

    StringBuffer oldData = mFileData.get(caseName);

    if ( oldData == null || oldData.length() == 0){
    oldData = new StringBuffer();
    }

    StringBuffer realRecordData = new StringBuffer();
    realRecordData.append(oldData).append(newData);

        /* Save data into a Fixed size StringBuffer. When case stop, will write into file.
         * If data which really need record is bigger than buffer size, need ignore some older ones.
         * Ignore rule: need remove data by whole item, can't remove part of item.
         */

        int length = realRecordData.length();
        if(length <= FileUtils.TRACE_FILE_BUFFER_SIZE){
            mFileData.put(caseName, realRecordData);
        } else {
            int deleteLength = 0;
            do{
                int index = realRecordData.indexOf(TRACK_SEP_ITEMS, 0);
                deleteLength = index + 1;
                realRecordData.delete(0, deleteLength);
            }while(deleteLength >= length);

            mFileData.put(caseName, realRecordData);
        }
    }

    private HashMap<String,Integer> parseCalResult (String result){
        LogUtils.logd("Cal Result: " + result);
        if ( result == null || result.equals("")) return null;

        //The result format is :  key1:value1;key2:value2;key3:value3
        HashMap<String, Integer> resultMap = new HashMap<String, Integer>();
        String[] list = result.split(";");
        for( String value : list){
            String[] values = value.split(":");
            if (values.length == 2)
                resultMap.put(values[0], Integer.valueOf(values[1]));
        }
        return resultMap;
    }

    private void updateViewAndResultForCal(String caseName, boolean isSupport){
        updateViewAndResult(caseName, "", isSupport);
    }

    private void updateViewAndResult(String caseName, String resultInfo, boolean isSupport){
        StringBuffer result = new StringBuffer();
        boolean isPass = true;
        boolean isCalCase = false;

        SensorEntity entity = mSensorEntityMap.get(caseName);
        isCalCase = entity.getCategory() == SensorEntity.CAT_SENSOR_CAL;

        TestCase testCase = mSensorCase.get(caseName);

        //Parse Calibration Result
        if ( mCalResultMap.containsKey(caseName)) {
            String calResult = mCalResultMap.get(caseName);
            String calString = this.getResources().getString(R.string.sensor_cal);

            HashMap<String, Integer> resultMap = parseCalResult(calResult);
            if ( resultMap != null){
                Iterator<Map.Entry<String, Integer>> entries = resultMap.entrySet().iterator();

                while (entries.hasNext()){
                    Map.Entry<String, Integer> entry = entries.next();
                    int resultValue = entry.getValue();
                    result.append(calString).append(" ").append(entry.getKey())
                            .append(" = ")
                            .append(tranCalResultForUI(resultValue)).append("\n");

                    //Add Test Data : calibration result
                    testCase.addTestData("calibration_" + entry.getKey(), tranCalResultForResultFile(entry.getValue()));

                    //For Sensor Calibration Cases
                    if (isCalCase && isPass){
                        if ( resultValue == SensorCalUtils.CAL_RESULT_FAIL
                                || resultValue == SensorCalUtils.CAL_RESULT_UNSUPPORTED
                                || resultValue == SensorCalUtils.CAL_INTERNAL_ERROR)
                            isPass = false;
                    }
                }
            }
        }

        StringBuffer sb = mFileData.get(caseName);

        //Just for sensor data cases, not for calibration cases.
        if(!isCalCase && sb != null && sb.length() > 0){
            //Add Test Data: track_filename
            String sensorTraceFile = getTrackFileName(caseName);
            testCase.addTestData("track_filename", sensorTraceFile);

            //Write latest sensor data into default_xxx_res.xml
            String[] contents = sb.toString().split(TRACK_SEP_ITEMS);
            String latest = contents[contents.length-1];

            LogUtils.logi( "Latest Value : " + latest);
            if ( latest != null && latest.length() > 0) {
                switch(entity.getValuesNum()){
                case SensorEntity.NUM_1 :
                    testCase.addTestData("value", latest);
                    break;
                case SensorEntity.NUM_3 :
                    String[] values = latest.split(TRACK_SEP_CONTENT);
                    if ( values.length == SensorEntity.NUM_3) {
                        testCase.addTestData("x", values[0]);
                        testCase.addTestData("y", values[1]);
                        testCase.addTestData("z", values[2]);
                    }
                    break;
                case SensorEntity.NUM_6 :
                    break;
                default :
                    break;
                }
            }
        }

        result.append(resultInfo);

        updateView(caseName, result.toString());

        if (isPass && isSupport){
            LogUtils.logi( "Case Name:" + caseName + " State : True");
            updateResultForCase(caseName, TestCase.STATE_PASS);
        } else {
            LogUtils.logi( "Case Name:" + caseName + " State : Fail");
            updateResultForCase(caseName, TestCase.STATE_FAIL);
        }
    }

    private String getTrackFileName(String caseName){
        return getApplicationContext().getFilesDir().getAbsolutePath() + "/" + caseName + ".track";
    }

    private String calibrateSensor(Sensor sensor){
        return SensorCalUtils.calibrateSensor(sensor, SensorCalUtils.CAL_TEST_TYPE_OEM);
    }

    private String tranCalResultForUI(int result) {
        switch(result){
            case SensorCalUtils.CAL_RESULT_PASS :
                return this.getResources().getString(R.string.sensor_cal_pass);
            case SensorCalUtils.CAL_RESULT_FAIL :
                return this.getResources().getString(R.string.sensor_cal_failed);
            case SensorCalUtils.CAL_RESULT_UNSUPPORTED :
                return this.getResources().getString(R.string.sensor_cal_not_support);
            case SensorCalUtils.CAL_INTERNAL_ERROR :
                return this.getResources().getString(R.string.sensor_cal_internal_error);

        }
        return "";
    }

    private String tranCalResultForResultFile(int result) {
        Resources currentRes = this.getResources();
        android.content.res.AssetManager asserts = currentRes.getAssets();
        android.util.DisplayMetrics metrics = currentRes.getDisplayMetrics();
        android.content.res.Configuration config = new android.content.res.Configuration(currentRes.getConfiguration());
        config.locale = Locale.ENGLISH;
        Resources enRes = this.createConfigurationContext(config).getResources();

        switch(result){
            case SensorCalUtils.CAL_RESULT_PASS :
                return enRes.getString(R.string.sensor_cal_pass);
            case SensorCalUtils.CAL_RESULT_FAIL :
                return enRes.getString(R.string.sensor_cal_failed);
            case SensorCalUtils.CAL_RESULT_UNSUPPORTED :
                return enRes.getString(R.string.sensor_cal_not_support);
            case SensorCalUtils.CAL_INTERNAL_ERROR :
                return enRes.getString(R.string.sensor_cal_internal_error);

        }
        return "";
    }

    private void StartCountDown(String caseName){

        if (mCaseFreqMap.containsKey(caseName)){
            LogUtils.logi( "caseName : " + caseName);
            countDownThread thread = new countDownThread(caseName);
            thread.start();
        }
    }

    private class countDownThread extends Thread{

        String caseName;

        public countDownThread(String caseName){
            super();
            this.caseName = caseName;
            LogUtils.logi( "caseName : " + caseName);
        }

        @Override
        public void run() {
            Looper.prepare();
            SensorCountDown down = new SensorCountDown(caseName);
            down.start();
            mCountDownMap.put(caseName, down);
            Looper.loop();
            LogUtils.logi( "Start CountDown for case : " + caseName);
        }
    };

    private class SensorCountDown extends CountDownTimer{

        String caseName;

        public SensorCountDown(String caseName) {
            super(1000*60*60*24, mCaseFreqMap.get(caseName));
            this.caseName = caseName;
            LogUtils.logi( "caseName :" + caseName);
        }

        @Override
        public void onFinish() {
            LogUtils.logi( "CountDown Finish for case : " + caseName);
        }

        @Override
        public void onTick(long millisUntilFinished) {
            LogUtils.logi( "onTick: " + caseName);
            synchronized(mSensorDataWaitLock) {
                if (mSensorDataMap.containsKey(caseName)){
                    SensorData data = mSensorDataMap.get(caseName);

                    //If Sensor data don't handle.
                    if (data != null && !data.isHandled()){
                        Message message = mHandler.obtainMessage();
                        message.what = PARSE_DATA;
                        Bundle bundle = new Bundle();
                        bundle.putString("caseName", caseName);
                        bundle.putSerializable("data", data);
                        message.setData(bundle);
                        mHandler.sendMessage(message);
                        data.setHandled(true);
                    }
                }
            }
        }

    };

    Handler mHandler = new Handler() {
        public void handleMessage(android.os.Message msg) {
            switch (msg.what) {
                case PARSE_DATA:
                    LogUtils.logi("Parse data per freq");
                    Bundle bundle = msg.getData();
                    String caseName = bundle.getString("caseName");
                    SensorData event = (SensorData) bundle.get("data");
                    parseData(caseName,event.getSensorEvent());
                    break;
                default:
                    break;
            }
        }
    };

    class SensorData implements Serializable{
        private static final long serialVersionUID = 1L;

        SensorEvent event;
        boolean handled;

        SensorData(SensorEvent event, boolean handled){
            this.event = event;
            this.handled = handled;
        }

        boolean isHandled(){
            return handled;
        }

        void setHandled(boolean flag){
            handled = flag;
        }

        SensorEvent getSensorEvent(){
            return event;
        }
    }

    private String formatData(float data){
        return NumericUtils.formatDecimals(data, DECIMALS);
    }
}
