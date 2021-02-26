/*============================================================================
@file QSensorDialog.java

@brief
The dialog box used by the user to select the sensor data stream rate for either
a particular sensor, or all sensors.

Copyright (c) 2011-2013, 2015-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.ui.stream;

import android.app.Dialog;
import android.graphics.Color;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.content.Context;
import android.view.View.OnFocusChangeListener;
import android.util.Log;

import com.qualcomm.qti.sensors.qsensortest.R;
import com.qualcomm.qti.sensors.ui.qsensortest.TabControl;

public class SensorDialog extends Dialog {
       public View.OnClickListener onClickListenerBatch = null;
       int wearCurrReportRate,wearCurrStreamRate;
       public EditText wearReportRateField;
       public EditText wearStreamRateField;
       public static boolean isReportText = true;
       Color color = new Color();
  public SensorDialog(View.OnClickListener onClickListener, int currStreamRate,
      int currReportRate, boolean supportsBatching, Context mContext) {
        super(mContext);
         onClickListenerBatch = onClickListener;
         wearCurrReportRate = currReportRate;
         wearCurrStreamRate = currStreamRate;
    if (TabControl.EnableWearQSTP) {
        this.setContentView(R.layout.wear_stream_rate_dialog);
        Button delayNormalButton = (Button) this.findViewById(R.id.wear_delay_button_normal);
        delayNormalButton.setOnClickListener(onClickListener);
        Button delayUIButton = (Button) this.findViewById(R.id.wear_delay_button_ui);
        delayUIButton.setOnClickListener(onClickListener);
        Button delayGameButton = (Button) this.findViewById(R.id.wear_delay_button_game);
        delayGameButton.setOnClickListener(onClickListener);
        Button delayFastestButton = (Button) this.findViewById(R.id.wear_delay_button_fastest);
        delayFastestButton.setOnClickListener(onClickListener);
        Button dialogCancelButton = (Button) this.findViewById(R.id.wear_delay_button_cancel);
        dialogCancelButton.setOnClickListener(onClickListener);
        Button dialogBatchButton = (Button) this.findViewById(R.id.wear_delay_button_batch);
        dialogBatchButton.setOnClickListener(onClickListener);
    } else {
        this.setContentView(R.layout.stream_rate_dialog);
        this.setTitle("Sensor Stream Rate");

        String currentRate = currStreamRate == -1 ? "" : String.valueOf(currStreamRate);
        EditText streamRateField = (EditText) this.findViewById(R.id.delay_field_sample);
        streamRateField.setText(currentRate);

        currentRate = currReportRate == -1 ? "" : String.valueOf(currReportRate);
        EditText reportRateField = (EditText) this.findViewById(R.id.delay_field_report);
        reportRateField.setText(currentRate);

        Button delayNormalButton = (Button) this.findViewById(R.id.delay_button_normal);
        delayNormalButton.setOnClickListener(onClickListener);

        Button delayUIButton = (Button) this.findViewById(R.id.delay_button_ui);
        delayUIButton.setOnClickListener(onClickListener);

        Button delayGameButton = (Button) this.findViewById(R.id.delay_button_game);
        delayGameButton.setOnClickListener(onClickListener);

        Button delayFastestButton = (Button) this.findViewById(R.id.delay_button_fastest);
        delayFastestButton.setOnClickListener(onClickListener);

        Button dialogSubmitButton = (Button) this.findViewById(R.id.delay_button_submit);
        dialogSubmitButton.setOnClickListener(onClickListener);

        Button dialogCancelButton = (Button) this.findViewById(R.id.delay_button_cancel);
        dialogCancelButton.setOnClickListener(onClickListener);

        Button dialogFlushButton = (Button) this.findViewById(R.id.delay_button_flush);

        boolean sdkSupport = android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.KITKAT;
        if (!sdkSupport || !supportsBatching) {
           reportRateField = (EditText) this.findViewById(R.id.delay_field_report);
           reportRateField.setEnabled(false);

           TextView reportRateText = (TextView) this.findViewById(R.id.delay_field_report_text);
           reportRateText.setEnabled(false);
      }

      if (!sdkSupport) {
          dialogFlushButton.setEnabled(false);
      }

      dialogFlushButton.setOnClickListener(onClickListener);
   }
  }
  void fieldReportDilog() {
       this.setContentView(R.layout.wear_batch_rate_dialog);
       String currentRate = wearCurrStreamRate == -1 ? "" : String.valueOf(wearCurrStreamRate);
       currentRate = wearCurrReportRate == -1 ? "" : String.valueOf(wearCurrReportRate);
       wearReportRateField = (EditText) this.findViewById(R.id.wear_delay_field_report);
       wearReportRateField.setText(currentRate);
       wearReportRateField.requestFocus();
       wearReportRateField.setRawInputType(0);
       wearReportRateField.setOnFocusChangeListener(new OnFocusChangeListener() {
           @Override
           public void onFocusChange(View v, boolean hasFocus)
           {
               isReportText = hasFocus;
               if (isReportText) {
                   wearReportRateField.setTextColor(color.RED);
                   wearStreamRateField.setTextColor(color.GREEN);
               } else {
                   wearReportRateField.setTextColor(color.GREEN);
                   wearStreamRateField.setTextColor(color.RED);
               }

           }
       });
       wearStreamRateField = (EditText) this.findViewById(R.id.wear_delay_field_sample);
       currentRate = wearCurrStreamRate == -1 ? "" : String.valueOf(wearCurrStreamRate);
       wearStreamRateField.setText(currentRate);
       wearStreamRateField.requestFocus();
       wearStreamRateField.setRawInputType(0);
       wearStreamRateField.setOnFocusChangeListener(new OnFocusChangeListener() {
           @Override
           public void onFocusChange(View v, boolean hasFocus)
           {
               isReportText = !hasFocus;
               if (isReportText) {
                   wearReportRateField.setTextColor(color.RED);
                   wearStreamRateField.setTextColor(color.GREEN);
               } else {
                   wearReportRateField.setTextColor(color.GREEN);
                   wearStreamRateField.setTextColor(color.RED);
               }
           }
       });

       Button zeroButton = (Button) this.findViewById(R.id.wear_delay_button_zero);
       zeroButton.setOnClickListener(onClickListenerBatch);
       Button oneButton = (Button) this.findViewById(R.id.wear_delay_button_one);
       oneButton.setOnClickListener(onClickListenerBatch);
       Button twoButton = (Button) this.findViewById(R.id.wear_delay_button_two);
       twoButton.setOnClickListener(onClickListenerBatch);
       Button threeButton = (Button) this.findViewById(R.id.wear_delay_button_three);
       threeButton.setOnClickListener(onClickListenerBatch);
       Button fourButton = (Button) this.findViewById(R.id.wear_delay_button_four);
       fourButton.setOnClickListener(onClickListenerBatch);
       Button fiveButton = (Button) this.findViewById(R.id.wear_delay_button_five);
       fiveButton.setOnClickListener(onClickListenerBatch);
       Button sixButton = (Button) this.findViewById(R.id.wear_delay_button_six);
       sixButton.setOnClickListener(onClickListenerBatch);
       Button sevenButton = (Button) this.findViewById(R.id.wear_delay_button_seven);
       sevenButton.setOnClickListener(onClickListenerBatch);
       Button eightButton = (Button) this.findViewById(R.id.wear_delay_button_eight);
       eightButton.setOnClickListener(onClickListenerBatch);
       Button nineButton = (Button) this.findViewById(R.id.wear_delay_button_nine);
       nineButton.setOnClickListener(onClickListenerBatch);
       Button wearDialogSubmitButton = (Button) this.findViewById(R.id.wear_delay_button_submit);
       wearDialogSubmitButton.setOnClickListener(onClickListenerBatch);
       Button deletButton = (Button) this.findViewById(R.id.wear_delay_button_delet);
       deletButton.setOnClickListener(onClickListenerBatch);
  }
}
