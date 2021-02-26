/******************************************************************************
 * ---------------------------------------------------------------------------
 *  Copyright (C) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 *  ---------------------------------------------------------------------------
 *******************************************************************************/

package com.qti.ultrasound.penpairing;

import com.qti.ultrasound.penpairing.customviews.BarChartView;
import com.qti.ultrasound.penpairing.customviews.BarChartView.BarConstants;

import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.RelativeLayout;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.TreeMap;
import android.widget.NumberPicker;
import android.view.LayoutInflater;
import android.view.View;
import android.content.Context;
import android.view.ViewGroup;
import android.app.AlertDialog;
import android.content.DialogInterface;

public class TesterActivity extends SWCalibrationActivity {

    private static final int POINT_SETTING_MIN = 0;

    private static final int POINT_SETTING_MAX = 4;

    private static final String SW_CALIB_TESTER_CFG_PATH = "/data/usf/sw_calib/usf_sw_calib_tester.cfg";

    private static final int MSG_IND_TYPE = 0;

    private static final int MSG_IND_MIC = 1;

    private static final int MSG_IND_POINT_INDEX = 2;

    private static final int MSG_IND_VALUE = 3;

    /*
     * MSG_TYPE constants. These values MUST match the ones
     * defined in the sw_calib daemon.
     */
    private static final int MSG_TYPE_MIN_SCALE_POWER = 0;

    private static final int MSG_TYPE_MAX_SCALE_POWER = 1;

    private static final int MSG_TYPE_THRESHOLD_POWER = 2;

    private static final int MSG_TYPE_MIN_SCALE_QUALITY = 3;

    private static final int MSG_TYPE_MAX_SCALE_QUALITY = 4;

    private static final int MSG_TYPE_THRESHOLD_QUALITY = 5;

    private static final int MSG_TYPE_POINT_POSITION = 6;

    private static final int MSG_TYPE_MEASUREMENT_POWER = 7;

    private static final int MSG_TYPE_MEASUREMENT_QUALITY = 8;

    private static final int MSG_TYPE_INTERFERENCE = 9;

    private static final int TESTER_CONTROL_THREAD_TIMEOUT_MSEC = Integer.MAX_VALUE; // no timeout

    private static final int NO_SERIES_FOUND_YET = -1;

    private static final int INVALID_SERIES_NUM = -2;

    private static final int X_INDEX = 0;

    private static final int Y_INDEX = 1;

    private BarChartView mSignalPowerChart = null;

    private TreeMap<Integer, Float> mSignalPowerMeasurementBuffer = new TreeMap<Integer, Float>();

    private BarChartView mSignalQualityChart = null;

    private TreeMap<Integer, Float> mSignalQualityMeasurementBuffer = new TreeMap<Integer, Float>();

    private String mOriginalCfgPath;

    private TreeMap<Integer, PointPosition> mPointPositionArray = new TreeMap<Integer, TesterActivity.PointPosition>();

    private int mCurrentPointPosition = 0;

    private Button mSettingsButton = null;

    private static boolean mIsFirstPointVisible = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_tester);

        mIsFirstPointVisible = false;
        mSettingsButton = (Button)findViewById(R.id.button_settings);
    }

    @Override
    protected int getTimeoutForControlThread() {
        return TESTER_CONTROL_THREAD_TIMEOUT_MSEC;
    }


    @Override
    protected void onPairingSuccessful(int penId) {
        super.onPairingSuccessful(penId);
        setOnScreenText("Pen detected (series ID " + String.valueOf(penId) + ")",
                R.id.sw_calib_text_view);
    }

    @Override
    protected void onPairingTimeout(int penId) {
      // First phase - check if there is pairing without validation
      if (penId != NO_SERIES_FOUND_YET && penId != INVALID_SERIES_NUM ) {
        Log.d(this.toString(), "series found without validation");
        onPairingSuccessful(penId);
      }
      // Second phase - manual pairing
      else{
        LayoutInflater inflater = (LayoutInflater)getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View v = inflater.inflate(R.layout.activity_manual, null);
        final NumberPicker picker = (NumberPicker)v.findViewById(R.id.series_picker);

        picker.setDescendantFocusability(ViewGroup.FOCUS_BLOCK_DESCENDANTS);
        picker.setMaxValue(PairingActivity.MAX_SERIES_ID);
        picker.setMinValue(PairingActivity.MIN_SERIES_ID);

        AlertDialog.Builder alert = new AlertDialog.Builder(this);
        alert.setTitle("Manual Pairing").setView(v)
        .setPositiveButton("OK", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
              onPairingSuccessful(picker.getValue());
            }
        }).setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
              alertAndQuit("Pen Pairing Failed", SemiAutomaticActivity.TIMEOUT_MESSAGE);
            }
        });
        alert.show();
      }
    }

    private void showNewPointPosition() {
        if (mCurrentCircle != null) {
            mCurrentCircle.setVisibility(View.INVISIBLE);
        }
        mCurrentCircle = new View(this);
        mCurrentCircle.setBackgroundResource(R.drawable.circle);
        setCircleColor(Color.CYAN, mCurrentCircle);
        LayoutParams lp = new LayoutParams(CALIB_CIRCLE_DIAMETER_DP, CALIB_CIRCLE_DIAMETER_DP);
        lp.width = lp.height = CALIB_CIRCLE_DIAMETER_DP;
        mCurrentCircle.setLayoutParams(lp);

        RelativeLayout root = (RelativeLayout)findViewById(R.id.tester_root_view);
        root.addView(mCurrentCircle);

        mCurrentCircle
                .setX((mPointPositionArray.get(mCurrentPointPosition).mPointPosition[X_INDEX] * mActivityWidth)
                        / MAX_X - CALIB_CIRCLE_DIAMETER_DP / 2f);
        mCurrentCircle
                .setY((mPointPositionArray.get(mCurrentPointPosition).mPointPosition[Y_INDEX] * mActivityHeight)
                        / MAX_Y - CALIB_CIRCLE_DIAMETER_DP / 2f);
        root.invalidate();
    }

    class PointPosition {
        float[] mPointPosition = new float[] {
                -1, -1
        };
        private TreeMap<Integer, BarConstants> mSignalPowerConstantsList = new TreeMap<Integer, BarConstants>();
        private TreeMap<Integer, BarConstants> mSignalQualityConstantsList = new TreeMap<Integer, BarConstants>();
    }

    public void onClickSettingsButton(View view) {
        LayoutInflater inflater = (LayoutInflater)getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View v = inflater.inflate(R.layout.point_settings, null);
        final NumberPicker picker = (NumberPicker)v.findViewById(R.id.point_settings_picker);

        picker.setDescendantFocusability(ViewGroup.FOCUS_BLOCK_DESCENDANTS);
        picker.setMaxValue(POINT_SETTING_MAX);
        picker.setMinValue(POINT_SETTING_MIN);
        picker.setValue(mCurrentPointPosition);

        AlertDialog.Builder alert = new AlertDialog.Builder(this);
        alert.setTitle("Pen Position Settings").setView(v).setPositiveButton("OK",
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Log.d(this.toString(), "user chose point position: " + picker.getValue());
                        mCurrentPointPosition = picker.getValue();
                        // Setting to null so that the charts would be updated
                        mSignalPowerChart.updateConstantValues(mPointPositionArray.get(mCurrentPointPosition)
                                .mSignalPowerConstantsList.values().toArray(
                                        new BarConstants[mPointPositionArray.get(mCurrentPointPosition)
                                        .mSignalPowerConstantsList.size()]));
                        mSignalQualityChart.updateConstantValues(mPointPositionArray
                                .get(mCurrentPointPosition)
                                .mSignalQualityConstantsList.values().toArray(
                                        new BarConstants[mPointPositionArray
                                                .get(mCurrentPointPosition)
                                        .mSignalQualityConstantsList.size()]));
                        showNewPointPosition();
                    }
                }).setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                // No change in pen choice, nothing to do
            }
        });
        alert.show();
    }

    @Override
    protected void onSwCalibSocketEvent(int[] res) {
        int type = res[MSG_IND_TYPE];
        int index = res[MSG_IND_MIC];
        int pointIndex = res[MSG_IND_POINT_INDEX];
        float val = Float.intBitsToFloat(res[MSG_IND_VALUE]);

        if (!mPointPositionArray.containsKey(pointIndex)) {
            mPointPositionArray.put(pointIndex, new PointPosition());
        }

        if (pointIndex >= 0
                && !mPointPositionArray.get(pointIndex).mSignalPowerConstantsList
                        .containsKey(index)) {
            mPointPositionArray.get(pointIndex).mSignalPowerConstantsList.put(index,
                    new BarConstants(String.valueOf(index)));
            mPointPositionArray.get(pointIndex).mSignalQualityConstantsList.put(index,
                    new BarConstants(String.valueOf(index)));
        }

        setOnScreenText("Testing in progress...", R.id.sw_calib_text_view);
        View v = (View)findViewById(R.id.sw_calib_circle_view);
        v.setVisibility(View.INVISIBLE);
         mSettingsButton.setVisibility(View.VISIBLE);
        if (isPointSet(0) && mIsFirstPointVisible == false) {
            // All values of the first point are set, then show it
            showNewPointPosition();
            mIsFirstPointVisible = true;
        }

        switch (type) {
            case MSG_TYPE_MEASUREMENT_POWER:
                handleTypeMeasurementPower(index, val);
                break;
            case MSG_TYPE_MEASUREMENT_QUALITY:
                handleTypeMeasurementQuality(index, val);
                break;
            case MSG_TYPE_MIN_SCALE_POWER:
                mPointPositionArray.get(pointIndex).mSignalPowerConstantsList.get(index).mScaleMin = val;
                break;
            case MSG_TYPE_MAX_SCALE_POWER:
                mPointPositionArray.get(pointIndex).mSignalPowerConstantsList.get(index).mScaleMax = val;
                break;
            case MSG_TYPE_THRESHOLD_POWER:
                mPointPositionArray.get(pointIndex).mSignalPowerConstantsList.get(index).mThreshold = val;
                break;
            case MSG_TYPE_MIN_SCALE_QUALITY:
                mPointPositionArray.get(pointIndex).mSignalQualityConstantsList.get(index).mScaleMin = val;
                break;
            case MSG_TYPE_MAX_SCALE_QUALITY:
                mPointPositionArray.get(pointIndex).mSignalQualityConstantsList.get(index).mScaleMax = val;
                break;
            case MSG_TYPE_THRESHOLD_QUALITY:
                mPointPositionArray.get(pointIndex).mSignalQualityConstantsList.get(index).mThreshold = val;
                break;
            case MSG_TYPE_POINT_POSITION:
                mPointPositionArray.get(pointIndex).mPointPosition[index] = val;
                break;
            case MSG_TYPE_INTERFERENCE:
                setOnScreenText("Interference detected!", R.id.sw_calib_text_view);
                setCircleColor(Color.RED, findViewById(R.id.sw_calib_circle_view));
                break;
        }
    }

    private boolean isPointSet(int index) {
        return ((mPointPositionArray.get(index).mPointPosition[0] != -1)
        && (mPointPositionArray.get(index).mPointPosition[1] != -1));
    }

    private void handleTypeMeasurementPower(int index, float val) {
        if (mSignalPowerChart == null) {
            mSignalPowerChart = new BarChartView(
                    this, mPointPositionArray.get(mCurrentPointPosition)
                    .mSignalPowerConstantsList.values().toArray(
                            new BarConstants[mPointPositionArray.get(mCurrentPointPosition)
                            .mSignalPowerConstantsList.size()]),
                    "Signal Power (dB)");
            RelativeLayout.LayoutParams rlp_left = new RelativeLayout.LayoutParams(
                    LayoutParams.WRAP_CONTENT, LayoutParams.MATCH_PARENT);
            rlp_left.addRule(RelativeLayout.LEFT_OF, R.id.sw_calib_text_view);
            mSignalPowerChart.setBackgroundColor(Color.WHITE);
            ((RelativeLayout)findViewById(R.id.tester_root_view)).addView(mSignalPowerChart, rlp_left);
        }
        mSignalPowerMeasurementBuffer.put(index, val);
        if (mSignalPowerMeasurementBuffer.size() == mPointPositionArray
                .get(mCurrentPointPosition).mSignalPowerConstantsList.size()) {
            // we have values for all mics - update chart
            mSignalPowerChart.postNewChartData(mSignalPowerMeasurementBuffer
                    .values().toArray(new Float[mSignalPowerMeasurementBuffer.size()]));
            mSignalPowerMeasurementBuffer.clear();
        }
    }

    private void handleTypeMeasurementQuality(int index, float val) {
        if (mSignalQualityChart == null) {
            mSignalQualityChart = new BarChartView(
                    this,
                    mPointPositionArray.get(mCurrentPointPosition).mSignalQualityConstantsList
                            .values().toArray(new BarConstants[mPointPositionArray
                                    .get(mCurrentPointPosition).mSignalQualityConstantsList
                                    .size()]),
                    "Signal Quality (peak/RMS)");
            RelativeLayout.LayoutParams rlp_right = new RelativeLayout.LayoutParams(
                    LayoutParams.WRAP_CONTENT, LayoutParams.MATCH_PARENT);
            rlp_right.addRule(RelativeLayout.RIGHT_OF, R.id.sw_calib_text_view);
            mSignalQualityChart.setBackgroundColor(Color.WHITE);

            ((RelativeLayout)findViewById(R.id.tester_root_view)).addView(
                    mSignalQualityChart,
                    rlp_right);
        }
        mSignalQualityMeasurementBuffer.put(index, val);
        if (mSignalQualityMeasurementBuffer.size() == mPointPositionArray
                .get(mCurrentPointPosition).mSignalQualityConstantsList.size()) {
            // we have values for all mics - update chart
            mSignalQualityChart.postNewChartData(mSignalQualityMeasurementBuffer
                    .values().toArray(new Float[mSignalQualityMeasurementBuffer.size()]));
            mSignalQualityMeasurementBuffer.clear();
        }
    }

    protected void changeCfgLink() {
        try {
            Runtime.getRuntime().exec("rm -f " + SW_CALIB_PROXY_PATH);
            Runtime.getRuntime().exec(
                "ln -s " + SW_CALIB_TESTER_CFG_PATH + " " + SW_CALIB_PROXY_PATH);
        } catch (IOException e) {
            Log.e(this.toString(), e.toString());
            alertAndQuit("Software Calibration Failed", "Failed changing cfg link.");
       }
    }

}
