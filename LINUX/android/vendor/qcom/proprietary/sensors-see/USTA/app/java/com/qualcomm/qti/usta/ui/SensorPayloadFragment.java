/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file SensorPayloadFragment.java
  @brief
   SensorPayloadFragment class implementation
============================================================================*/
package com.qualcomm.qti.usta.ui;

import android.app.Fragment;
import android.content.Context;
import android.content.res.Resources;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.TypedValue;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.TextView;


import com.qualcomm.qti.usta.R;
import com.qualcomm.qti.usta.core.AccessModifier;
import com.qualcomm.qti.usta.core.DataType;
import com.qualcomm.qti.usta.core.USTALog;
import com.qualcomm.qti.usta.core.NestedMsgPayload;
import com.qualcomm.qti.usta.core.SensorContext;
import com.qualcomm.qti.usta.core.SensorPayloadField;

import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import static android.text.InputType.TYPE_CLASS_NUMBER;
import static android.text.InputType.TYPE_NUMBER_FLAG_DECIMAL;
import static android.text.InputType.TYPE_NUMBER_FLAG_SIGNED;
import static android.text.InputType.TYPE_TEXT_FLAG_CAP_CHARACTERS;
import static android.util.TypedValue.COMPLEX_UNIT_DIP;


public class SensorPayloadFragment extends Fragment {

  private SensorContext sensorContext;

  private int sensorHandle;
  private int sensorReqHandle;
  private int sensorPayloadWidgetID = 0;
  private int sensorMaxMainPayloadFields;
  private int RIDStopReq;
  public static boolean isClientDisableRequestEnabled = false;
  Map hashTable;
  public static SensorPayloadFragment CreateSensorPayloadFragmentInstance(int sensorHandle, int reqMsgID, SensorContext sensorContextInstance) {

    SensorPayloadFragment payloadFragmentInstance = new SensorPayloadFragment();

    payloadFragmentInstance.sensorHandle = sensorHandle;
    payloadFragmentInstance.sensorReqHandle = reqMsgID;
    payloadFragmentInstance.sensorContext = sensorContextInstance;

    return payloadFragmentInstance;
  }

  public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceData) {

    return createPayloadViewLayout(sensorContext.getSensors().get(sensorHandle).getSensorPayloadField(sensorReqHandle));
  }

  private View createPayloadViewLayout(Vector<SensorPayloadField> sensorPayloadFields) {

    if (sensorPayloadFields == null) {

      return null;
    }
    hashTable = new HashMap();
    // Level 1 Main Layout
    LinearLayout level_1_Layout = new LinearLayout(getActivity());
    LinearLayout.LayoutParams mainLayoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.MATCH_PARENT);
    level_1_Layout.setOrientation(LinearLayout.VERTICAL);
    level_1_Layout.setLayoutParams(mainLayoutParams);

    // Text View - Payload:
    TextView textViewPayload = new TextView(getActivity());
    LinearLayout.LayoutParams textViewLayoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
    textViewLayoutParams.setMargins(convertDP2Px(16), convertDP2Px(0), convertDP2Px(16), convertDP2Px(0));
    textViewPayload.setLayoutParams(textViewLayoutParams);
    textViewPayload.setText(R.string.payload);

    level_1_Layout.addView(textViewPayload);

    // Payload Fields
    for (int payLoadFieldIndex = 0; payLoadFieldIndex < sensorPayloadFields.size(); payLoadFieldIndex++) {
      sensorPayloadFields.get(payLoadFieldIndex).resetValueToWidgetIDValueTable();
      addPayloadFieldToLayout(level_1_Layout, sensorPayloadFields, payLoadFieldIndex);
    }
    int widgetID = sensorPayloadWidgetID;

    LinearLayout level_2_Layout = new LinearLayout(getActivity());
    LinearLayout.LayoutParams level_2_LayoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.MATCH_PARENT);
    level_2_Layout.setOrientation(LinearLayout.HORIZONTAL);
    level_1_Layout.setLayoutParams(level_2_LayoutParams);

    //Request Button
    int RIDSendReq = ++widgetID;
    addButton(level_2_Layout, "Send Request", RIDSendReq, sendReqBtnListener(), true);

    //Stop Button
    RIDStopReq = ++widgetID;
    String stopStreaming ;
    if(isClientDisableRequestEnabled == false)
      stopStreaming = "Stop Request";
    else
      stopStreaming = "Disable Request";

    addButton(level_2_Layout, stopStreaming , RIDStopReq, stopReqBtnListener(), sensorContext.getSensors().get(sensorHandle).isSensorStreaming(sensorReqHandle));

    level_1_Layout.addView(level_2_Layout);
    sensorMaxMainPayloadFields = widgetID;

    sensorContext.getSensors().get(sensorHandle).setMaxMainPayloadFields(sensorReqHandle, sensorPayloadWidgetID);


    if(sensorContext.getSensors().get(sensorHandle).getMaxMainPayloadFields(sensorReqHandle) > sensorContext.getSensors().get(sensorHandle).getTotalPayloadFieldCount(sensorReqHandle)) {
      USTALog.i("No repeated Fields in this message");
    }
    else {
      USTALog.i("repeated Fields are present in this message");
//      updateRepeatedFields();
    }
    return level_1_Layout;
  }


  private View.OnClickListener sendReqBtnListener() {

    return new View.OnClickListener() {

      @Override
      public void onClick(View v) {

        if (0 == sensorContext.sendRequest(sensorHandle, sensorReqHandle, getActivity(), "sensorLog.txt")) {

          getActivity().findViewById(RIDStopReq).setEnabled(true);
        }
      }
    };
  }

  private void addButton(LinearLayout layout, String label, int widgetID, View.OnClickListener onClickListener, boolean inButtonSetFlag) {

    Button reqButton = new Button(getActivity());
    LinearLayout.LayoutParams buttonLayoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);

    buttonLayoutParams.setMargins(convertDP2Px(32), convertDP2Px(16), convertDP2Px(16), convertDP2Px(16));

    reqButton.setLayoutParams(buttonLayoutParams);
    reqButton.setId(widgetID);
    reqButton.setText(label);
    reqButton.setTextSize(COMPLEX_UNIT_DIP, (float) 16.0);
    reqButton.setOnClickListener(onClickListener);
    reqButton.setEnabled(inButtonSetFlag);
    layout.addView(reqButton);
  }

  private View.OnClickListener stopReqBtnListener() {

    return new View.OnClickListener() {

      @Override
      public void onClick(View v) {

        sensorContext.stopRequest(sensorHandle, sensorReqHandle);

        getActivity().findViewById(RIDStopReq).setEnabled(false);
      }
    };
  }

  private View.OnClickListener repeateButtonListener(final LinearLayout inLayout , final int addWidgetID, final SensorPayloadField payloadField) {

    return new View.OnClickListener() {

      @Override
      public void onClick(View v) {
        hashTable.put(sensorPayloadWidgetID, payloadField);
        if(isDataTypeNumerical(payloadField.getDataType())){
          addEditText(inLayout,payloadField.getDataType(), sensorPayloadWidgetID++);
        }
        if (payloadField.getDataType() == DataType.ENUM) {
          addSpinner(inLayout, sensorPayloadWidgetID++);
        }
        if (payloadField.getDataType() == DataType.BOOLEAN) {
          addCheckBox(inLayout , sensorPayloadWidgetID++);
        }
        sensorContext.getSensors().get(sensorHandle).setTotalPayloadFieldCount(sensorReqHandle, sensorPayloadWidgetID);
      }
    };
  }

  private int convertDP2Px(int densityIndependentPixels) {

    Resources r = getActivity().getResources();

    return (int) TypedValue.applyDimension(
            TypedValue.COMPLEX_UNIT_DIP,
            densityIndependentPixels,
            r.getDisplayMetrics()
    );

  }

  private boolean isDataTypeNumerical(DataType dataType) {

    return (dataType == DataType.FLOAT) || (dataType == DataType.SIGNED_INTEGER32) || (dataType == DataType.SIGNED_INTEGER64)
            || (dataType == DataType.UNSIGNED_INTEGER32) || (dataType == DataType.UNSIGNED_INTEGER64) || (dataType == DataType.STRING);

  }

  private void addNestedMessagesToPayload(LinearLayout level_1_Layout, NestedMsgPayload[] nestedMsgArray, int nestedMsgIndex) {

    TextView textViewPayload = new TextView(getActivity());
    LinearLayout.LayoutParams textViewLayoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
    textViewLayoutParams.setMargins(convertDP2Px(16), convertDP2Px(0), convertDP2Px(16), convertDP2Px(0));
    textViewPayload.setLayoutParams(textViewLayoutParams);
    textViewPayload.setText(nestedMsgArray[nestedMsgIndex].getNestedMsgName());

    level_1_Layout.addView(textViewPayload);

    Vector<SensorPayloadField> sensorPayloadFields ;
    sensorPayloadFields = new Vector<>();
    for (int payLoadFieldIndex = 0; payLoadFieldIndex < nestedMsgArray[nestedMsgIndex].getFieldCount(); payLoadFieldIndex++) {
      sensorPayloadFields.add(nestedMsgArray[nestedMsgIndex].getFiledAt(payLoadFieldIndex));
    }

    for (int payLoadFieldIndex = 0; payLoadFieldIndex < sensorPayloadFields.size(); payLoadFieldIndex++) {
      addPayloadFieldToLayout(level_1_Layout, sensorPayloadFields, payLoadFieldIndex);
    }

  }

  private void addPayloadFieldToLayout(LinearLayout level_1_Layout, Vector<SensorPayloadField> sensorPayloadFields, int payLoadFieldIndex) {

    SensorPayloadField payloadField = sensorPayloadFields.get(payLoadFieldIndex);
    if(payloadField.isUserDefined()){

      LinearLayout level_2_Layout = new LinearLayout(getActivity());
      LinearLayout.LayoutParams fieldLayoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
      fieldLayoutParams.setMargins(convertDP2Px(16), convertDP2Px(16), convertDP2Px(0), convertDP2Px(0));
      level_2_Layout.setLayoutParams(fieldLayoutParams);
      level_2_Layout.setOrientation(LinearLayout.VERTICAL);

      for (int nestedMsgIndex = 0; nestedMsgIndex < payloadField.getNestedMsgSize(); nestedMsgIndex++) {
        addNestedMessagesToPayload(level_2_Layout, payloadField.getNestedMsgs(), nestedMsgIndex);
      }

      level_1_Layout.addView(level_2_Layout);
    }
    else{
      // It is primitive data type..
      hashTable.put(sensorPayloadWidgetID, payloadField);
      if(payloadField.getAccessModifier() == AccessModifier.REPEATED) {
        // So Create horizontal layout. which has filedName  and corresponding user editable box / spinner
        // Level 2 Field Layout
        LinearLayout level_2_Layout = new LinearLayout(getActivity());
        LinearLayout.LayoutParams fieldLayoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        fieldLayoutParams.setMargins(convertDP2Px(16), convertDP2Px(16), convertDP2Px(0), convertDP2Px(0));
        level_2_Layout.setLayoutParams(fieldLayoutParams);
        level_2_Layout.setOrientation(LinearLayout.VERTICAL);
        level_1_Layout.addView(level_2_Layout);

        LinearLayout level_3_Layout = new LinearLayout(getActivity());
        LinearLayout.LayoutParams fieldLayout3Params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        fieldLayout3Params.setMargins(convertDP2Px(16), convertDP2Px(16), convertDP2Px(0), convertDP2Px(0));
        level_3_Layout.setLayoutParams(fieldLayout3Params);
        level_3_Layout.setOrientation(LinearLayout.HORIZONTAL);
        level_2_Layout.addView(level_3_Layout);
        addTextView(level_3_Layout, payloadField.getFieldName());

        if (isDataTypeNumerical(payloadField.getDataType())) {
          addEditText(level_3_Layout, payloadField.getDataType(), sensorPayloadWidgetID);
        }

        if (payloadField.getDataType() == DataType.ENUM) {
          addSpinner(level_3_Layout, sensorPayloadWidgetID);
        }

        if (payloadField.getDataType() == DataType.BOOLEAN) {
          addCheckBox(level_3_Layout , sensorPayloadWidgetID);
        }
        sensorPayloadWidgetID++;
        addButton(level_3_Layout,"+",sensorPayloadWidgetID,repeateButtonListener(level_2_Layout , sensorPayloadWidgetID ,payloadField), true);
        sensorPayloadWidgetID++;
      }
      else {
        // So Create horizontal layout. which has filedName  and corresponding user editable box / spinner
        // Level 2 Field Layout
        LinearLayout level_2_Layout = new LinearLayout(getActivity());
        LinearLayout.LayoutParams fieldLayoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        fieldLayoutParams.setMargins(convertDP2Px(16), convertDP2Px(16), convertDP2Px(0), convertDP2Px(0));
        level_2_Layout.setLayoutParams(fieldLayoutParams);
        level_2_Layout.setOrientation(LinearLayout.HORIZONTAL);
        level_1_Layout.addView(level_2_Layout);
        addTextView(level_2_Layout, payloadField.getFieldName());

        if (isDataTypeNumerical(payloadField.getDataType())) {
          addEditText(level_2_Layout, payloadField.getDataType(), sensorPayloadWidgetID);
        }

        if (payloadField.getDataType() == DataType.ENUM) {
          addSpinner(level_2_Layout, sensorPayloadWidgetID);
        }

        if (payloadField.getDataType() == DataType.BOOLEAN) {
          addCheckBox(level_2_Layout , sensorPayloadWidgetID);
        }
        sensorPayloadWidgetID++;
      }
    }
  }

  private void addTextView(LinearLayout layout, String text) {

    TextView textViewSampleRate = new TextView(getActivity());
    LinearLayout.LayoutParams textViewSampleRateParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);

    textViewSampleRateParams.setMargins(convertDP2Px(16), convertDP2Px(0), convertDP2Px(0), convertDP2Px(16));
    textViewSampleRateParams.weight = (float) 0.5;
    textViewSampleRate.setLayoutParams(textViewSampleRateParams);
    if(text.equals("sample_rate") || text.equals("resampled_rate"))
      text = text + "(Hz)";
    textViewSampleRate.setText(text);

    layout.addView(textViewSampleRate);
  }

  private void addSpinner(LinearLayout layout, final int widgetID) {

    Spinner spinner = new Spinner(getActivity());
    LinearLayout.LayoutParams spinnerParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);

    spinnerParams.weight = (float) 0.5;
    spinner.setLayoutParams(spinnerParams);
    spinner.setId(widgetID);

    String[] spinnerList = ((SensorPayloadField) hashTable.get(widgetID)).getEnumValues().clone();

    for (int index = 0; index < spinnerList.length; index++) {
      spinnerList[index] = spinnerList[index].toLowerCase();
    }

    ArrayAdapter<String> spinnerAdapter = new ArrayAdapter<>(getActivity().getApplicationContext(), android.R.layout.simple_list_item_1, spinnerList);
    spinner.setAdapter(spinnerAdapter);
//    if(widgetID < sensorMaxMainPayloadFields) {
//      String fieldValue = ((SensorPayloadField) hashTable.get(widgetID)).getValue();
//
//      if (fieldValue != null) {
//
//        String[] spinnerListStr = ((SensorPayloadField) hashTable.get(widgetID)).getEnumValues();
//        int index;
//
//        for (index = 0; index < spinnerListStr.length; index++) {
//
//          if (fieldValue.equals(spinnerListStr[index])) {
//
//            break;
//          }
//        }
//
//        spinner.setSelection(index);
//      }
//    }

    spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
      @Override
      public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
          ((SensorPayloadField) hashTable.get(widgetID)).addValueToWidgetIDValueTable(String.valueOf(id) , widgetID);
      }

      @Override
      public void onNothingSelected(AdapterView<?> parent) {

      }
    });

    layout.addView(spinner);
  }

  private void addCheckBox(LinearLayout layout,final int widgetID) {

    CheckBox checkBox = new CheckBox(getActivity());
    LinearLayout.LayoutParams checkBoxParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);

    checkBoxParams.weight = (float) 0.5;
    checkBoxParams.setMargins(convertDP2Px(0), convertDP2Px(0), convertDP2Px(16), convertDP2Px(0));
    checkBox.setLayoutParams(checkBoxParams);
    checkBox.setId(widgetID);
//    if(widgetID < sensorMaxMainPayloadFields) {
//      String fieldValue = ((SensorPayloadField) hashTable.get(widgetID)).getValue();
//
//      if (fieldValue != null) {
//
//        if (fieldValue.equals("true")) {
//
//          checkBox.setChecked(true);
//        } else {
//
//          checkBox.setChecked(false);
//        }
//      }
//    }

    checkBox.setOnClickListener(new View.OnClickListener() {
      public void onClick(View v) {
        if (((CheckBox) v).isChecked()) {
          ((SensorPayloadField) hashTable.get(widgetID)).addValueToWidgetIDValueTable(String.valueOf(true) , widgetID);
        } else {
          ((SensorPayloadField) hashTable.get(widgetID)).addValueToWidgetIDValueTable(null, widgetID);
        }
      }
    });

    layout.addView(checkBox);
  }

  private void addEditText(LinearLayout layout, DataType dataType, final int widgetID) {

    EditText editText = new EditText(getActivity());
    LinearLayout.LayoutParams editTextParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);

    editTextParams.weight = (float) 0.5;
    editTextParams.setMargins(convertDP2Px(0), convertDP2Px(0), convertDP2Px(16), convertDP2Px(0));
    editText.setLayoutParams(editTextParams);
    editText.setId(widgetID);

    if ((dataType == DataType.UNSIGNED_INTEGER32) || (dataType == DataType.UNSIGNED_INTEGER64)) {
      editText.setInputType(TYPE_TEXT_FLAG_CAP_CHARACTERS);
    } else if ((dataType == DataType.SIGNED_INTEGER32) || (dataType == DataType.SIGNED_INTEGER64)) {
      editText.setInputType(TYPE_CLASS_NUMBER | TYPE_NUMBER_FLAG_SIGNED);
    } else if (dataType == DataType.FLOAT) {
      editText.setInputType(TYPE_CLASS_NUMBER | TYPE_NUMBER_FLAG_DECIMAL);
    }

    editText.setTextSize(COMPLEX_UNIT_DIP, (float) 16.0);

//    if(widgetID < sensorContext.getSensors().get(sensorHandle).getMaxMainPayloadFields(sensorReqHandle)) {
//      SensorPayloadField payloadField = (SensorPayloadField) hashTable.get(widgetID);
//      Map widgetIDTable = payloadField.getWidgetIDValueTable();
//      String fieldValue = widgetIDTable.get(widgetID).toString();
//      if (fieldValue != null) {
//        editText.setText(fieldValue);
//      }
//    }
//    else {
//      SensorPayloadField payloadField = (SensorPayloadField) hashTable.get(widgetID);
//      Map widgetIDTable = payloadField.getWidgetIDValueTable();
//    }
    editText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
      @Override
      public void onFocusChange(View v, boolean hasFocus) {

        if (v.getId() == R.id.batch_period_input_text && !hasFocus) {
          InputMethodManager imm = (InputMethodManager) getActivity().getApplicationContext().getSystemService(Context.INPUT_METHOD_SERVICE);
          imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
        }
      }
    });

    TextWatcher editTextTextWatcher = new TextWatcher() {
      @Override
      public void beforeTextChanged(CharSequence s, int start, int count, int after) {

      }

      @Override
      public void onTextChanged(CharSequence s, int start, int before, int count) {
        if (s.length() != 0 ) {
          ((SensorPayloadField) hashTable.get(widgetID)).addValueToWidgetIDValueTable(s.toString() ,widgetID );

        }
        else {
            ((SensorPayloadField) hashTable.get(widgetID)).addValueToWidgetIDValueTable(null, widgetID);
        }
      }

      @Override
    public void afterTextChanged(Editable s) {

    }
  };

    editText.addTextChangedListener(editTextTextWatcher);

    layout.addView(editText);
  }
}
