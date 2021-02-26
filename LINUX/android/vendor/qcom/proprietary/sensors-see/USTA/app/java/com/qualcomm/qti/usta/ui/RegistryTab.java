/*============================================================================
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file Registry.java
  @brief
   Registry class implementation
============================================================================*/

package com.qualcomm.qti.usta.ui;


import android.app.Fragment;
import android.app.FragmentManager;
import android.content.Context;
import android.graphics.Color;
import android.graphics.Typeface;
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
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import com.qualcomm.qti.usta.R;
import com.qualcomm.qti.usta.core.ModeType;
import com.qualcomm.qti.usta.core.SensorContext;
import com.qualcomm.qti.usta.core.SensorContextJNI;
import com.qualcomm.qti.usta.core.USTALog;

import java.util.Vector;

public class RegistryTab  extends Fragment implements Runnable{

  private SensorContext sensorContext;
  private View registryTabView;
  private int registrySensorHandle;
  private TextView suidLowTextView;
  private TextView suidHighTextView;
  private EditText registryReadGroupNameEditText;
  private Button registryReadButton;
  String registryReadGroupName;
  private EditText registryWriteMainGroupNameEditText;
  String registryWriteMainGroupName;
  private EditText registryWriteGroupNameEditText;
  String registryWriteGroupName;
  private EditText registryWriteVersionEditText;
  String registryWriteVersionNumber;
  private Spinner oneOfSpinnerList;
  private Button registryWriteButton;
  public static String oneOfFieldInput;
  private int oneOfSpinnnerPosition;
  private Thread registryReadOutputThread;
  private TextView registryReadTextView;
  boolean isThreadRunning = true;

  @Override
  public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
    registryTabView =  inflater.inflate(R.layout.registry_tab, container, false);

    sensorContext = SensorContext.getSensorContext(ModeType.USTA_MODE_TYPE_UI);

    if(sensorContext == null){
      USTALog.e("sensorContext is null ");
      return null;
    }

    registrySensorHandle = getRegistrySensorHandle();
    if(registrySensorHandle == -1){
      USTALog.e("Registry Sensor is not Enabled ");
      return null;
    }
    setSUIDTextView();
    setUpRegistryReadGroupName();
    setUpRegistryReadButton();
    setUpRegistryWriteMainGroupName();
    setUpRegistryWriteGroupName();
    setUpRegistryWriteVersionNumber();
    setUpOneOfSpinner();
    setupRegistryWriteButton();
    registryReadOutputThread = new Thread(this);
    registryReadTextView = (TextView) registryTabView.findViewById(R.id.reg_read_output_id);
    return registryTabView;
  }

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setHasOptionsMenu(true);
  }

  @Override
  public void onPrepareOptionsMenu(Menu menu) {
    menu.findItem(R.id.disable_qmi_connection_id).setEnabled(false);
  }

  @Override
  public void run() {

    while(isThreadRunning) {
      if (SensorEventDisplayFragment.isDisplaySamplesEnabled == true) {
        String inDisplayString = SensorContextJNI.getSamplesFromNativeWrapper(true);
        if (inDisplayString != null && inDisplayString.length() != 0)
          displayRegistryReadInfo(inDisplayString);
      }
    }
  }
  private void displayRegistryReadInfo(final String inSensorEventData) {
    if (SensorEventDisplayFragment.isDisplaySamplesEnabled == true) {
      if (getActivity() != null) {
        getActivity().runOnUiThread(new Runnable() {
          @Override
          public void run() {

            if (inSensorEventData != null && inSensorEventData.length() != 0) {
              registryReadTextView.setText(inSensorEventData);
              registryReadTextView.setTextColor(Color.BLUE);
              registryReadTextView.setTypeface(null, Typeface.BOLD_ITALIC);
            } else {
              registryReadTextView.setText("");
            }
          }
        });
      }
    }
  }

  private int getRegistrySensorHandle(){
    String registryName = "registry";
    for(int sensorHandle = 0 ; sensorHandle < sensorContext.getSensors().size() ; sensorHandle ++){
      if(sensorContext.getSensors().get(sensorHandle).getSensorName().contains(registryName)){
        return sensorHandle;
      }
    }

    return -1;
  }

  private void setSUIDTextView(){

    suidLowTextView = (TextView) registryTabView.findViewById(R.id.reg_suid_low);
    suidHighTextView = (TextView) registryTabView.findViewById(R.id.reg_suid_high);

    suidLowTextView.setText(sensorContext.getSensors().get(registrySensorHandle).getSensorSUIDLow());
    suidHighTextView.setText(sensorContext.getSensors().get(registrySensorHandle).getSensorSUIDHigh());

  }

  private void setUpRegistryReadGroupName() {

    registryReadGroupNameEditText = (EditText) registryTabView.findViewById(R.id.registry_group_name_input_text);
    registryReadGroupNameEditText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
      @Override
      public void onFocusChange(View v, boolean hasFocus) {

        if (v.getId() == R.id.registry_group_name_input_text && !hasFocus) {
          InputMethodManager imm = (InputMethodManager) getActivity().getApplicationContext().getSystemService(Context.INPUT_METHOD_SERVICE);
          imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
        }
      }
    });

    TextWatcher registryReadGroupNameTextWatcher = new TextWatcher() {
      @Override
      public void beforeTextChanged(CharSequence s, int start, int count, int after) {

      }

      @Override
      public void onTextChanged(CharSequence s, int start, int before, int count) {

        if (s.length() != 0) {
          registryReadGroupName = s.toString();
        } else
          registryReadGroupName = null;
      }

      @Override
      public void afterTextChanged(Editable s) {

      }
    };

    registryReadGroupNameEditText.addTextChangedListener(registryReadGroupNameTextWatcher);
  }

  private void setUpRegistryReadButton(){

    registryReadButton = (Button) registryTabView.findViewById(R.id.reg_read_button);

    registryReadButton.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        if(registryReadOutputThread.getState() == Thread.State.NEW) {
          registryReadOutputThread.start();
        }
        SensorContextJNI.enableStreamingStatusNativeWrapper(registrySensorHandle);
        sensorContext.RegistryReadSendRequest(ModeType.USTA_MODE_TYPE_UI, registrySensorHandle,  registryReadGroupName , getActivity());
      }
    });

  }

  private void setupRegistryReadReqMsgInfo(){

  }

  private void setUpRegistryWriteMainGroupName(){
    registryWriteMainGroupNameEditText = (EditText) registryTabView.findViewById(R.id.registry_write_main_group_name_input_text);
    registryWriteMainGroupNameEditText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
      @Override
      public void onFocusChange(View v, boolean hasFocus) {

        if (v.getId() == R.id.registry_write_main_group_name_input_text && !hasFocus) {
          InputMethodManager imm = (InputMethodManager) getActivity().getApplicationContext().getSystemService(Context.INPUT_METHOD_SERVICE);
          imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
        }
      }
    });

    TextWatcher registryWtiteMainGroupNameTextWatcher = new TextWatcher() {
      @Override
      public void beforeTextChanged(CharSequence s, int start, int count, int after) {

      }

      @Override
      public void onTextChanged(CharSequence s, int start, int before, int count) {

        if (s.length() != 0) {
          registryWriteMainGroupName = s.toString();
        } else
          registryWriteMainGroupName = null;
      }

      @Override
      public void afterTextChanged(Editable s) {

      }
    };

    registryWriteMainGroupNameEditText.addTextChangedListener(registryWtiteMainGroupNameTextWatcher);
  }

  private void setUpRegistryWriteGroupName() {
    registryWriteGroupNameEditText = (EditText) registryTabView.findViewById(R.id.registry_write_group_name_input_text);
    registryWriteGroupNameEditText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
      @Override
      public void onFocusChange(View v, boolean hasFocus) {

        if (v.getId() == R.id.registry_write_group_name_input_text && !hasFocus) {
          InputMethodManager imm = (InputMethodManager) getActivity().getApplicationContext().getSystemService(Context.INPUT_METHOD_SERVICE);
          imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
        }
      }
    });

    TextWatcher registryWtiteGroupNameTextWatcher = new TextWatcher() {
      @Override
      public void beforeTextChanged(CharSequence s, int start, int count, int after) {

      }

      @Override
      public void onTextChanged(CharSequence s, int start, int before, int count) {

        if (s.length() != 0) {
          registryWriteGroupName = s.toString();
        } else
          registryWriteGroupName = null;
      }

      @Override
      public void afterTextChanged(Editable s) {

      }
    };

    registryWriteGroupNameEditText.addTextChangedListener(registryWtiteGroupNameTextWatcher);
  }

  private void setUpRegistryWriteVersionNumber(){

    registryWriteVersionEditText = (EditText) registryTabView.findViewById(R.id.registry_write_version_number_input_text);
    registryWriteVersionEditText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
      @Override
      public void onFocusChange(View v, boolean hasFocus) {

        if (v.getId() == R.id.registry_write_version_number_input_text && !hasFocus) {
          InputMethodManager imm = (InputMethodManager) getActivity().getApplicationContext().getSystemService(Context.INPUT_METHOD_SERVICE);
          imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
        }
      }
    });

    TextWatcher registryWriteVersionTextWatcher = new TextWatcher() {
      @Override
      public void beforeTextChanged(CharSequence s, int start, int count, int after) {

      }

      @Override
      public void onTextChanged(CharSequence s, int start, int before, int count) {

        if (s.length() != 0) {
          registryWriteVersionNumber = s.toString();
        } else
          registryWriteVersionNumber = null;
      }

      @Override
      public void afterTextChanged(Editable s) {

      }
    };

    registryWriteVersionEditText.addTextChangedListener(registryWriteVersionTextWatcher);
  }

  private void setUpOneOfSpinner(){
    final Vector<String> oneOfItems = new Vector<>();
    oneOfItems.add("str");
    oneOfItems.add("flt");
    oneOfItems.add("sint");

    oneOfSpinnerList = (Spinner) registryTabView.findViewById(R.id.one_of_spinner);
    ArrayAdapter<String> oneOfSpinnerAdapter = new ArrayAdapter<>(getActivity().getApplicationContext(), android.R.layout.simple_list_item_1, oneOfItems);
    oneOfSpinnerList.setAdapter(oneOfSpinnerAdapter);
    oneOfSpinnerList.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
      @Override
      public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {

        oneOfSpinnnerPosition = position;
        setupOneOfSpinnerFragment(position);

      }

      @Override
      public void onNothingSelected(AdapterView<?> parent) {

      }
    });
  }

  private void setupOneOfSpinnerFragment(int position) {

    FragmentManager fm = getActivity().getFragmentManager();
    android.app.Fragment oneOfSpinnerFragment = RegistryOneOfSpinnerFragment.CreateOneOfSpinnerFragmentInstance(position);

    if (fm.findFragmentById(R.id.registry_oneOf_fragment) != null) {

      fm.beginTransaction().replace(R.id.registry_oneOf_fragment, oneOfSpinnerFragment).commit();
    } else {

      fm.beginTransaction().add(R.id.registry_oneOf_fragment, oneOfSpinnerFragment).commit();
    }
  }

  private void setupRegistryWriteButton(){
    registryWriteButton = (Button) registryTabView.findViewById(R.id.registry_write_button);
    registryWriteButton.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {

        sensorContext.RegistryWriteSendRequest(ModeType.USTA_MODE_TYPE_UI, registrySensorHandle,  registryWriteMainGroupName ,registryWriteGroupName, registryWriteVersionNumber, oneOfSpinnnerPosition, oneOfFieldInput,  getActivity());
      }
    });

  }


}
