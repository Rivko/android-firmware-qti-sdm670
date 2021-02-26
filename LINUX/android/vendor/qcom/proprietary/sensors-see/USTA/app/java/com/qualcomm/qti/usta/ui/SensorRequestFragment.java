/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file SensorRequestFragment.java
  @brief
   SensorRequestFragment class implementation
============================================================================*/
package com.qualcomm.qti.usta.ui;

import android.app.Fragment;
import android.app.FragmentManager;
import android.content.Context;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import com.qualcomm.qti.usta.R;
import com.qualcomm.qti.usta.core.SensorContextJNI;
import com.qualcomm.qti.usta.core.USTALog;
import com.qualcomm.qti.usta.core.ModeType;
import com.qualcomm.qti.usta.core.SensorContext;

import java.util.Vector;


public class SensorRequestFragment extends Fragment {

  private SensorContext sensorContext;
  private View sensorRequestFragmentView;

  private int sensorHandle;
  private int sensorReqHandle;
  private CheckBox flushOnlyCB;
  private CheckBox maxBatchCB;
  private Spinner sensorListSpinner;
  private Spinner sensorReqMsgSpinner;
  private ArrayAdapter<String> sensorReqMsgSpinnerAdapter;
  private Spinner clientProcessorSpinner;
  private Spinner wakeupDeliverySpinner;
  private EditText batchPeriodEditText;
  private EditText flushPeriodEditText;
  public static int previousSpinnerPosition = -1;

  @Override
  public void onCreate(Bundle savedInstanceData) {
    super.onCreate(savedInstanceData);
    sensorContext = SensorContext.getSensorContext(ModeType.USTA_MODE_TYPE_UI);
    setHasOptionsMenu(true);
  }

  @Override
  public void onPrepareOptionsMenu(Menu menu) {
    menu.findItem(R.id.disable_qmi_connection_id).setEnabled(false);
  }

  private void setupSensorListSpinner() {

    sensorListSpinner = (Spinner) sensorRequestFragmentView.findViewById(R.id.sensor_list_spinner);
    ArrayAdapter<String> sensorListSpinnerAdapter = new ArrayAdapter<>(getActivity().getApplicationContext(), android.R.layout.simple_list_item_1, sensorContext.getSensorNames());

    sensorListSpinner.setAdapter(sensorListSpinnerAdapter);

    sensorListSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
      @Override
      public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {

        SensorContextJNI.disableStreamingStatusNativeWrapper(previousSpinnerPosition);
        sensorHandle = sensorListSpinner.getSelectedItemPosition();
        previousSpinnerPosition = sensorHandle;
        SensorContextJNI.enableStreamingStatusNativeWrapper(sensorHandle);

        TextView suidLowTextView = (TextView) sensorRequestFragmentView.findViewById(R.id.suid_low);
        suidLowTextView.setText(sensorContext.getSensors().get(sensorHandle).getSensorSUIDLow());

        TextView suidHighTextView = (TextView) sensorRequestFragmentView.findViewById(R.id.suid_high);
        suidHighTextView.setText(sensorContext.getSensors().get(sensorHandle).getSensorSUIDHigh());
        if(sensorContext.getSensorNames().get(sensorHandle).contains("registry")) {
          SensorContext.throwErrorDialog("Registry functionality is implemented in Registry TAB.  Please check in another Tab.", getContext());
        }
        else{
          resetRequestMsgListSpinner();
          Toast.makeText(getActivity().getApplicationContext(), "You have selected: " + sensorContext.getSensorNames().get(sensorHandle), Toast.LENGTH_SHORT).show();
        }

      }

      @Override
      public void onNothingSelected(AdapterView<?> parent) {

      }
    });
  }

  private void resetClientProcessorSpinner() {

    if (clientProcessorSpinner == null) {

      setupClientProcessorSpinner();
    } else {

      clientProcessorSpinner.setSelection(sensorContext.getSensors().get(sensorHandle).getSensorStdFields(sensorReqHandle).getClientProcessor());
    }
  }

  private void resetWakeupDeliverySpinner() {

    if (wakeupDeliverySpinner == null) {

      setupWakeupDeliverySpinner();
    } else {

      wakeupDeliverySpinner.setSelection(sensorContext.getSensors().get(sensorHandle).getSensorStdFields(sensorReqHandle).getWakeupDevilvery());
    }
  }

  private void resetRequestMsgListSpinner() {

    if (sensorReqMsgSpinner == null) {

      setupRequestMsgListSpinner();
    } else {
      sensorContext.getSensors().get(sensorHandle).clearSensorReqMsgNamesWithMsgID();
      sensorContext.getSensors().get(sensorHandle).setSensorReqMsgsNamesWithMsgID();
      @SuppressWarnings("unchecked")
      Vector<String> sensorReqMsgNamesToAdapter = (Vector<String>) sensorContext.getSensors().get(sensorHandle).getSensorReqMsgsNamesWithMsgID().clone();

      int defaultSpinnerPosition = 0;
      for(int reqMsgIndex = 0 ; reqMsgIndex < sensorReqMsgNamesToAdapter.size() ; reqMsgIndex++){
        if(sensorReqMsgNamesToAdapter.get(reqMsgIndex).contains("513")) {
          defaultSpinnerPosition = reqMsgIndex;
          break;
        }
      }

      sensorReqMsgSpinnerAdapter.clear();
      sensorReqMsgSpinnerAdapter.addAll(sensorReqMsgNamesToAdapter);
      sensorReqMsgSpinnerAdapter.notifyDataSetChanged();
//      sensorReqMsgSpinner.setSelection(0);
//      sensorReqHandle = 0;
      sensorReqMsgSpinner.setSelection(defaultSpinnerPosition);
      sensorReqHandle = defaultSpinnerPosition;


      String batchPeriodString = sensorContext.getSensors().get(sensorHandle).getSensorStdFields(sensorReqHandle).getBatchPeriod();
      if (batchPeriodString == null || batchPeriodString.length() == 0)
        batchPeriodEditText.setText(null);
      else
        batchPeriodEditText.setText(batchPeriodString);

      String flushPeriodString = sensorContext.getSensors().get(sensorHandle).getSensorStdFields(sensorReqHandle).getBatchPeriod();
      if (flushPeriodString == null || flushPeriodString.length() == 0)
        flushPeriodEditText.setText(null);
      else
        flushPeriodEditText.setText(flushPeriodString);

      if (flushOnlyCB.isChecked() == false) {
        flushOnlyCB.setChecked(false);
      }
      if (maxBatchCB.isChecked() == false) {
        maxBatchCB.setChecked(false);
      }

      setupSensorPayloadFragment();
    }

    resetClientProcessorSpinner();
    resetWakeupDeliverySpinner();
  }

  private void setupRequestMsgListSpinner() {

    sensorReqMsgSpinner = (Spinner) sensorRequestFragmentView.findViewById(R.id.req_msg_list_spinner);

    sensorContext.getSensors().get(sensorHandle).clearSensorReqMsgNamesWithMsgID();
    sensorContext.getSensors().get(sensorHandle).setSensorReqMsgsNamesWithMsgID();
    Vector<String> sensorReqMsgNames = (Vector<String>) sensorContext.getSensors().get(sensorHandle).getSensorReqMsgsNamesWithMsgID();

    if ((null == sensorReqMsgNames) || sensorReqMsgNames.size() == 0) {
      return;
    }
    int defaultSpinnerPosition = 0;
    for(int reqMsgIndex = 0 ; reqMsgIndex < sensorReqMsgNames.size() ; reqMsgIndex++){
      if(sensorReqMsgNames.get(reqMsgIndex).contains("513")) {
        defaultSpinnerPosition = reqMsgIndex;
        break;
      }
    }

    Vector<String> sensorReqMsgNamesToAdapter = (Vector<String>) sensorReqMsgNames.clone();
    sensorReqMsgSpinnerAdapter = new ArrayAdapter<>(getActivity().getApplicationContext(), android.R.layout.simple_list_item_1, sensorReqMsgNamesToAdapter);
    sensorReqMsgSpinner.setAdapter(sensorReqMsgSpinnerAdapter);
    sensorReqMsgSpinner.setSelection(defaultSpinnerPosition);
    sensorReqHandle = defaultSpinnerPosition;

    sensorReqMsgSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
      @Override
      public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {

        sensorReqHandle = sensorReqMsgSpinner.getSelectedItemPosition();
        setupSensorPayloadFragment();

        resetClientProcessorSpinner();
        resetWakeupDeliverySpinner();
        batchPeriodEditText.setText(sensorContext.getSensors().get(sensorHandle).getSensorStdFields(sensorReqHandle).getBatchPeriod());
        flushPeriodEditText.setText(sensorContext.getSensors().get(sensorHandle).getSensorStdFields(sensorReqHandle).getFlushPeriod());
        flushOnlyCB.setChecked(sensorContext.getSensors().get(sensorHandle).getSensorStdFields(sensorReqHandle).isFlushOnly());
        maxBatchCB.setChecked(sensorContext.getSensors().get(sensorHandle).getSensorStdFields(sensorReqHandle).isMaxBatch());
      }

      @Override
      public void onNothingSelected(AdapterView<?> parent) {

      }
    });
  }

  private void setupClientProcessorSpinner() {

    clientProcessorSpinner = (Spinner) sensorRequestFragmentView.findViewById(R.id.client_processor_spinner);
    ArrayAdapter<String> clientProcessorSpinnerAdapter = new ArrayAdapter<>(getActivity().getApplicationContext(), android.R.layout.simple_list_item_1, sensorContext.getClientProcessors());
    clientProcessorSpinner.setAdapter(clientProcessorSpinnerAdapter);

    clientProcessorSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
      @Override
      public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {

        sensorContext.getSensors().get(sensorHandle).getSensorStdFields(sensorReqHandle).setClientProcessor(clientProcessorSpinner.getSelectedItemPosition());
      }

      @Override
      public void onNothingSelected(AdapterView<?> parent) {

      }
    });
  }

  private void setupWakeupDeliverySpinner() {

    wakeupDeliverySpinner = (Spinner) sensorRequestFragmentView.findViewById(R.id.wakeup_spinner);
    ArrayAdapter<String> wakeupDeliverySpinnerAdapter = new ArrayAdapter<>(getActivity().getApplicationContext(), android.R.layout.simple_list_item_1, sensorContext.getWakeupDelivery());
    wakeupDeliverySpinner.setAdapter(wakeupDeliverySpinnerAdapter);

    wakeupDeliverySpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
      @Override
      public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {

        sensorContext.getSensors().get(sensorHandle).getSensorStdFields(sensorReqHandle).setWakeupDevilvery(wakeupDeliverySpinner.getSelectedItemPosition());
      }

      @Override
      public void onNothingSelected(AdapterView<?> parent) {

      }
    });
  }

  private void setupBatchPeriodEditText() {

    batchPeriodEditText = (EditText) sensorRequestFragmentView.findViewById(R.id.batch_period_input_text);
    batchPeriodEditText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
      @Override
      public void onFocusChange(View v, boolean hasFocus) {

        if (v.getId() == R.id.batch_period_input_text && !hasFocus) {
          InputMethodManager imm = (InputMethodManager) getActivity().getApplicationContext().getSystemService(Context.INPUT_METHOD_SERVICE);
          imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
        }
      }
    });

    TextWatcher batchPeriodTextWatcher = new TextWatcher() {
      @Override
      public void beforeTextChanged(CharSequence s, int start, int count, int after) {

      }

      @Override
      public void onTextChanged(CharSequence s, int start, int before, int count) {

        if (s.length() != 0) {
          sensorContext.getSensors().get(sensorHandle).getSensorStdFields(sensorReqHandle).setBatchPeriod(s.toString());
        } else
          sensorContext.getSensors().get(sensorHandle).getSensorStdFields(sensorReqHandle).setBatchPeriod(null);
      }

      @Override
      public void afterTextChanged(Editable s) {

      }
    };

    batchPeriodEditText.addTextChangedListener(batchPeriodTextWatcher);
  }

  private void setupFlushOnlyCheckBox () {
    flushOnlyCB    =   (CheckBox) sensorRequestFragmentView.findViewById(R.id.flush_only_checkbox);
    flushOnlyCB.setOnClickListener(new View.OnClickListener()
    {
      public void onClick(View v) {
        if (((CheckBox)v).isChecked()) {
          sensorContext.getSensors().get(sensorHandle).getSensorStdFields(sensorReqHandle).setFlushOnly(true);
        } else {
          sensorContext.getSensors().get(sensorHandle).getSensorStdFields(sensorReqHandle).setFlushOnly(false);
        }
      }
    });
  }

  private void setupMaxBatchCheckBox () {
    maxBatchCB    =   (CheckBox) sensorRequestFragmentView.findViewById(R.id.max_batch_checkbox);
    maxBatchCB.setOnClickListener(new View.OnClickListener()
    {
      public void onClick(View v) {
        if (((CheckBox)v).isChecked()) {
          sensorContext.getSensors().get(sensorHandle).getSensorStdFields(sensorReqHandle).setMaxBatch(true);
        } else {
          sensorContext.getSensors().get(sensorHandle).getSensorStdFields(sensorReqHandle).setMaxBatch(false);
        }
      }
    });
  }

  private void setupFlushPeriodEditText() {

    flushPeriodEditText = (EditText) sensorRequestFragmentView.findViewById(R.id.flush_period_input_text);
    flushPeriodEditText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
      @Override
      public void onFocusChange(View v, boolean hasFocus) {

        if (v.getId() == R.id.flush_period_input_text && !hasFocus) {
          InputMethodManager imm = (InputMethodManager) getActivity().getApplicationContext().getSystemService(Context.INPUT_METHOD_SERVICE);
          imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
        }
      }
    });

    TextWatcher flushPeriodTextWatcher = new TextWatcher() {
      @Override
      public void beforeTextChanged(CharSequence s, int start, int count, int after) {

      }

      @Override
      public void onTextChanged(CharSequence s, int start, int before, int count) {

        if (s.length() != 0) {
          sensorContext.getSensors().get(sensorHandle).getSensorStdFields(sensorReqHandle).setFlushPeriod(s.toString());
        } else
          sensorContext.getSensors().get(sensorHandle).getSensorStdFields(sensorReqHandle).setFlushPeriod(null);
      }

      @Override
      public void afterTextChanged(Editable s) {

      }
    };

    flushPeriodEditText.addTextChangedListener(flushPeriodTextWatcher);
  }

  private void setupSensorPayloadFragment() {

    FragmentManager fm = getActivity().getFragmentManager();
    Fragment sensorPayloadFragment = SensorPayloadFragment.CreateSensorPayloadFragmentInstance(sensorHandle, sensorReqHandle, sensorContext);

    if (fm.findFragmentById(R.id.sensor_payload_fragment) != null) {

      fm.beginTransaction().replace(R.id.sensor_payload_fragment, sensorPayloadFragment).commit();
    } else {

      fm.beginTransaction().add(R.id.sensor_payload_fragment, sensorPayloadFragment).commit();
    }
  }

  private void setupSensorEvenPayloadFragment() {

    FragmentManager fm = getActivity().getFragmentManager();
    Fragment sensorEventPayloadFragment = SensorEventDisplayFragment.CreateDisplaySamplesInstance();

    if (fm.findFragmentById(R.id.sensor_event_payload_display_id) != null) {

      fm.beginTransaction().replace(R.id.sensor_event_payload_display_id, sensorEventPayloadFragment).commit();
    } else {

      fm.beginTransaction().add(R.id.sensor_event_payload_display_id, sensorEventPayloadFragment).commit();
    }

  }

  public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceData) {

    if (sensorContext == null) {

      USTALog.e("sensorContext is null so Killing the activity ");

      SensorContext.throwErrorDialog("No Sensors Found! Please check if proto files are loaded.", getContext());

      return null;
    }

    sensorRequestFragmentView = inflater.inflate(R.layout.fragment_sensor_request, container, false);

    setupSensorListSpinner();

    setupBatchPeriodEditText();
    setupFlushPeriodEditText();
    setupFlushOnlyCheckBox();
    setupMaxBatchCheckBox();

    setupSensorEvenPayloadFragment();
    return sensorRequestFragmentView;
  }

}
