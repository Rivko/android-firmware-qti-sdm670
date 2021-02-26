/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file RegistryOneOfSpinnerFragment.java
  @brief
   RegistryOneOfSpinnerFragment class implementation
============================================================================*/
package com.qualcomm.qti.usta.ui;

import android.app.Fragment;
import android.content.res.Resources;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.TypedValue;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import static android.text.InputType.TYPE_CLASS_NUMBER;
import static android.text.InputType.TYPE_CLASS_TEXT;
import static android.text.InputType.TYPE_NUMBER_FLAG_DECIMAL;

public class RegistryOneOfSpinnerFragment extends Fragment {

  private int spinnerPosition;

  public static RegistryOneOfSpinnerFragment CreateOneOfSpinnerFragmentInstance(int position) {

    RegistryOneOfSpinnerFragment oneOfSpinnerFragmentInstance = new RegistryOneOfSpinnerFragment();
    oneOfSpinnerFragmentInstance.spinnerPosition = position;

    return oneOfSpinnerFragmentInstance;
  }


  public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceData) {

    return createOneOfSpinnerViewLayout(spinnerPosition);
  }

  private View createOneOfSpinnerViewLayout(int position){

    if(position == -1)
      return null;

    LinearLayout level_1_Layout = new LinearLayout(getActivity());
    LinearLayout.LayoutParams mainLayoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.MATCH_PARENT);
    level_1_Layout.setOrientation(LinearLayout.VERTICAL);
    level_1_Layout.setLayoutParams(mainLayoutParams);

    switch(position){
      case 0:
        setUpStrField(level_1_Layout, position);
        break;
      case 1:
        setUpFltField(level_1_Layout, position);
        break;
      case 2:
        setUpSintField(level_1_Layout, position);
        break;
    }

    return level_1_Layout;
  }

  private void setUpStrField(LinearLayout level_1_Layout, int position){

    LinearLayout level_2_Layout = new LinearLayout(getActivity());
    LinearLayout.LayoutParams mainLayoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.MATCH_PARENT);
    level_2_Layout.setOrientation(LinearLayout.HORIZONTAL);
    level_2_Layout.setLayoutParams(mainLayoutParams);
    level_1_Layout.addView(level_2_Layout);

    addTextView(level_2_Layout, "str");

    addEditText(level_2_Layout, position);

  }

  private void setUpFltField(LinearLayout level_1_Layout, int position){

    LinearLayout level_2_Layout = new LinearLayout(getActivity());
    LinearLayout.LayoutParams mainLayoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.MATCH_PARENT);
    level_2_Layout.setOrientation(LinearLayout.HORIZONTAL);
    level_2_Layout.setLayoutParams(mainLayoutParams);
    level_1_Layout.addView(level_2_Layout);

    addTextView(level_2_Layout, "flt");

    addEditText(level_2_Layout, position);

  }

  private void setUpSintField(LinearLayout level_1_Layout, int position){

    LinearLayout level_2_Layout = new LinearLayout(getActivity());
    LinearLayout.LayoutParams mainLayoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.MATCH_PARENT);
    level_2_Layout.setOrientation(LinearLayout.HORIZONTAL);
    level_2_Layout.setLayoutParams(mainLayoutParams);
    level_1_Layout.addView(level_2_Layout);

    addTextView(level_2_Layout, "sint");

    addEditText(level_2_Layout, position);

  }

  private int convertDP2Px(int densityIndependentPixels) {

    Resources r = getActivity().getResources();

    return (int) TypedValue.applyDimension(
            TypedValue.COMPLEX_UNIT_DIP,
            densityIndependentPixels,
            r.getDisplayMetrics()
    );

  }
  private void addTextView(LinearLayout layout, String text) {

    TextView textView = new TextView(getActivity());
    LinearLayout.LayoutParams textViewParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);

    textViewParams.setMargins(convertDP2Px(16), convertDP2Px(0), convertDP2Px(0), convertDP2Px(16));
    textViewParams.weight = (float) 0.5;
    textView.setLayoutParams(textViewParams);
    textView.setText(text);

    layout.addView(textView);
  }

  private void addEditText(LinearLayout layout, final int position) {

    EditText editText = new EditText(getActivity());

    switch(position) {
      case 0:
        editText.setInputType(TYPE_CLASS_TEXT);
        break;
      case 1:
        editText.setInputType(TYPE_NUMBER_FLAG_DECIMAL);
        break;
      case 2:
        editText.setInputType(TYPE_CLASS_NUMBER);
        break;
    }

    LinearLayout.LayoutParams editTextParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
    editTextParams.weight = (float) 0.5;
    editTextParams.setMargins(convertDP2Px(0), convertDP2Px(0), convertDP2Px(16), convertDP2Px(0));
    editText.setLayoutParams(editTextParams);


    editText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
      @Override
      public void onFocusChange(View v, boolean hasFocus) {

      }
    });

    TextWatcher editTextTextWatcher = new TextWatcher() {
      @Override
      public void beforeTextChanged(CharSequence s, int start, int count, int after) {

      }

      @Override
      public void onTextChanged(CharSequence s, int start, int before, int count) {

        if (s.length() != 0) {
          RegistryTab.oneOfFieldInput = s.toString();
        } else
          RegistryTab.oneOfFieldInput = null;
      }

      @Override
      public void afterTextChanged(Editable s) {

      }
    };

    editText.addTextChangedListener(editTextTextWatcher);

    layout.addView(editText);
  }

}
