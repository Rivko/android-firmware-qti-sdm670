/*============================================================================
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file SensorLowLatency.java
  @brief
  Sensor Low Latency Tab view creation
============================================================================*/
package com.qualcomm.qti.usta.ui;

import android.content.Context;
import android.os.Bundle;
import android.os.Environment;
import android.app.Fragment;
import android.app.FragmentManager;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.Spinner;
import android.widget.TextView;
import com.qualcomm.qti.usta.R;
import com.qualcomm.qti.usta.core.SensorLowLatencyJNI;
import com.qualcomm.qti.usta.core.USTALog;
import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.math.BigInteger;
import java.util.StringTokenizer;
import java.util.Vector;

public class SensorLowLatency extends Fragment {
    private View sensorLowLatView;
    private TextView suidLowTextView;
    private TextView suidHighTextView;
    private TextView flagTextView;
    private Spinner sensorLowLatSpinnerList;
    private EditText periodEditText;
    private Vector<String> lowLatSensorsNames;
    private Vector<String> lowLatLowSuids;
    private Vector<String> lowLatHighSuids;
    private Vector<Integer> lowLatFlags;
    private Vector<Boolean> lowLatEnabled;
    private Vector<Integer> lowLatStreamHandle;
    private Button enableButton;
    private String samplePeriodUs;
    private static int commonIndex;
    long[] sensorSuid;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        lowLatSensorsNames = new Vector<String>();
        lowLatLowSuids = new Vector<String>();
        lowLatHighSuids = new Vector<String>();
        lowLatStreamHandle = new Vector<Integer>();
        lowLatFlags = new Vector<Integer>();
        lowLatEnabled = new Vector<Boolean>();
        sensorSuid = new long[2];
        sensorLowLatView = inflater.inflate(R.layout.low_lat_tab, container, false);
        suidLowTextView = (TextView) sensorLowLatView.findViewById(R.id.low_lat_sensor_suid_low);
        suidHighTextView = (TextView) sensorLowLatView.findViewById(R.id.low_lat_sensor_suid_high);
        flagTextView = (TextView) sensorLowLatView.findViewById(R.id.low_lat_flag);
        readLowLatInputFile();
        setLowLatSensorsListSpinner();
        setPeriodEditText();
        setEnableButton();
        return sensorLowLatView;
    }

    @Override
    public void onCreate(Bundle savedInstanceData) {
        super.onCreate(savedInstanceData);
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        for (int enableCount = 0; enableCount < lowLatStreamHandle.size(); enableCount++) {
            if (lowLatEnabled.elementAt(enableCount) == true) {
                SensorLowLatencyJNI.lowLatDisable(lowLatStreamHandle.elementAt(enableCount));
                USTALog.i("Disabled stream with handle 0x" + Long.toHexString(lowLatStreamHandle.elementAt(enableCount)));
            }
        }
        commonIndex = 0;
        samplePeriodUs = null;
        lowLatSensorsNames.clear();
        lowLatLowSuids.clear();
        lowLatHighSuids.clear();
        lowLatStreamHandle.clear();
        lowLatFlags.clear();
        lowLatEnabled.clear();
    }

    private void readLowLatInputFile() {
        String line;
        StringTokenizer token;
        int index = 0;
        try {
            FileInputStream fileInputStream = new FileInputStream(Environment.getExternalStorageDirectory() + "/sensors_low_lat_data.txt");
            BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(fileInputStream));
            while ((line = bufferedReader.readLine()) != null) {
                token = new StringTokenizer(line, ":");
                while (token.hasMoreTokens()) {
                    String tempStr;
                    switch (index) {
                        case 0:
                            tempStr = token.nextToken();
                            lowLatSensorsNames.add(tempStr);
                            if (tempStr.endsWith("-Cal")) {
                                lowLatFlags.add(6);
                            } else {
                                lowLatFlags.add(2);
                            }
                            index++;
                            break;
                        case 1:
                            tempStr = token.nextToken();
                            lowLatLowSuids.add(tempStr.substring(0, 16));
                            lowLatHighSuids.add(tempStr.substring(16, tempStr.length()));
                            lowLatEnabled.add(false);
                            lowLatStreamHandle.add(0);
                            index = 0;
                            break;
                    }
                }
            }
            bufferedReader.close();
            fileInputStream.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void setLowLatSensorsListSpinner() {
        commonIndex = 0;
        sensorLowLatSpinnerList = (Spinner) sensorLowLatView.findViewById(R.id.low_lat_sensor_list_spinner);
        ArrayAdapter<String> sensorLowLatSpinnerListAdapter = new ArrayAdapter<>(getActivity().getApplicationContext(), android.R.layout.simple_list_item_1, lowLatSensorsNames);
        sensorLowLatSpinnerList.setAdapter(sensorLowLatSpinnerListAdapter);
        sensorLowLatSpinnerList.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                commonIndex = sensorLowLatSpinnerList.getSelectedItemPosition();
                updateLowLatSuids();
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
            }
        });
        sensorLowLatSpinnerList.setSelection(commonIndex);
    }

    private void updateLowLatSuids() {
        suidLowTextView.setText("0x" + lowLatLowSuids.elementAt(commonIndex).toString());
        suidHighTextView.setText("0x" + lowLatHighSuids.elementAt(commonIndex).toString());
        flagTextView.setText(lowLatFlags.elementAt(commonIndex).toString());
        if (lowLatEnabled.elementAt(commonIndex).booleanValue() == true) {
            enableButton.setText("Disable");
        } else {
            enableButton.setText("Enable");
        }
    }

    private void setEnableButton() {
        enableButton = (Button) sensorLowLatView.findViewById(R.id.low_lat_button);
        enableButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                int streamHandle;
                final ScrollView logScrollView = (ScrollView) sensorLowLatView.findViewById(R.id.scroll_view_log);
                TextView logTextView = (TextView) logScrollView.findViewById(R.id.text_view_log);
                logTextView.append("");
                if (lowLatEnabled.elementAt(commonIndex).booleanValue() == true) {
                    lowLatEnabled.set(commonIndex, false);
                    enableButton.setText("Enable");
                    SensorLowLatencyJNI.lowLatDisable(lowLatStreamHandle.elementAt(commonIndex));
                    logTextView.append("Sensor:" + lowLatSensorsNames.elementAt(commonIndex) + " with stream handle:0x" + Integer.toHexString(lowLatStreamHandle.elementAt(commonIndex)) + " disabled.\n");
                    USTALog.i("DISABLED:Sensor:" + lowLatSensorsNames.elementAt(commonIndex) + " handle: 0x" + Integer.toHexString(lowLatStreamHandle.elementAt(commonIndex)) + " suid_low:0x" + lowLatLowSuids.elementAt(commonIndex) + " suid_high:0x" + lowLatLowSuids.elementAt(commonIndex) + " Flag:" + lowLatFlags.elementAt(commonIndex) + "\n");
                    lowLatStreamHandle.set(commonIndex, 0);
                } else {
                    sensorSuid[0] = new BigInteger(lowLatLowSuids.elementAt(commonIndex), 16).longValue();
                    sensorSuid[1] = new BigInteger(lowLatHighSuids.elementAt(commonIndex), 16).longValue();
                    if (samplePeriodUs == null) {
                        samplePeriodUs = Integer.toString(SensorLowLatencyJNI.defaultSamplePeriodUs);
                    }
                    lowLatEnabled.set(commonIndex, true);
                    enableButton.setText("Disable");
                    streamHandle = SensorLowLatencyJNI.lowLatEnable(sensorSuid, Integer.decode(samplePeriodUs), lowLatFlags.elementAt(commonIndex));
                    lowLatStreamHandle.set(commonIndex, streamHandle);
                    logTextView.append("Sensor:" + lowLatSensorsNames.elementAt(commonIndex) + " with stream handle:0x" + Integer.toHexString(streamHandle) + " enabled.\n");
                    USTALog.i("ENABLED:Sensor:" + lowLatSensorsNames.elementAt(commonIndex) + " handle: 0x" + Integer.toHexString(streamHandle) + " suid_low:0x" + Long.toHexString(sensorSuid[0]) + " suid_high:0x" + Long.toHexString(sensorSuid[1]) + " Flag:" + lowLatFlags.elementAt(commonIndex) + " samplePeriodUs:" + Integer.decode(samplePeriodUs) + "\n");
                }
                logScrollView.postDelayed(new Runnable() {
                    public void run() {
                        logScrollView.fullScroll(ScrollView.FOCUS_DOWN);
                    }
                }, 0);
            }
        });
    }

    private void setPeriodEditText() {
        periodEditText = (EditText) sensorLowLatView.findViewById(R.id.low_lat_period_input_text);
        periodEditText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (v.getId() == R.id.low_lat_period_input_text && !hasFocus) {
                    InputMethodManager imm = (InputMethodManager) getActivity().getApplicationContext().getSystemService(Context.INPUT_METHOD_SERVICE);
                    imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
                }
            }
        });

        TextWatcher periodEditTextWatcher = new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                if (s.length() != 0) {
                    samplePeriodUs = s.toString();
                } else {
                    samplePeriodUs = null;
                }
            }

            @Override
            public void afterTextChanged(Editable s) {
            }
        };
        periodEditText.addTextChangedListener(periodEditTextWatcher);
    }
}
