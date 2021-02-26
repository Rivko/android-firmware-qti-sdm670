/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.savedataui;

import java.util.ArrayList;
import java.util.Calendar;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnCancelListener;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.CheckBox;
import android.widget.DatePicker;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.Spinner;
import android.widget.TextView;

import com.qualcomm.qti.sta.R;
import com.qualcomm.qti.sta.data.DateTextHandler;
import com.qualcomm.qti.sta.data.FieldData;
import com.qualcomm.qti.sta.savedataui.RepeatFieldData.RepeatType;
import com.qualcomm.qti.sta.savedataui.RepeatFieldData.TillType;

/**
 * Handler class for handling repeat field
 */
public class RepeatHandler {
    private static final String LOG_TAG = "RepeatHandler";

    private static final int POSITION_SAME_DATE = 0;
    private static final int POSITION_SAME_DAY = 1;

    private static final int POSITION_TILL_DATE = 1;
    private static final int POSITION_EVENTS = 2;

    private final int[] weekCheckboxIds = { R.id.sunday_check, R.id.monday_check,
            R.id.tuesday_check, R.id.wednesday_check, R.id.thrusday_check, R.id.friday_check,
            R.id.saturday_check };

    /**
     * Ui listener for ui notifications
     */
    interface RepeatUIListener {
        /**
         * Notification when user clicks on repeat field in event screen
         */
        public void onRepeatFieldClicked();

        /**
         * Notification when user clicks on repeat type from the dialog
         *
         * @param repeatType
         *            selected repeat type
         */
        public void onRepeatTypeSelected(RepeatType repeatType);

        /**
         * Notification when user changes the repeat data
         */
        public void onRepeatDataChanged();
    }

    private final DateTextHandler dateTexthandler;
    private final RepeatUIListener uiListener;
    private final Context context;
    private final LayoutInflater layoutInflater;
    private RepeatFieldData fieldData;
    private View rowView = null;

    /**
     * Constructor for the class
     *
     * @param context
     *            application context
     * @param dateTextHandler
     *            date text handler
     * @param inflater
     *            layout inflater
     * @param listener
     *            ui listener
     */
    public RepeatHandler(Context context, DateTextHandler dateTextHandler,
             LayoutInflater inflater, RepeatUIListener listener) {
        this.context = context;

        layoutInflater = inflater;
        uiListener = listener;
        this.dateTexthandler = dateTextHandler;
    }
    /**
     * @param fieldData
     *            field data
     * @param repeatAfter
     *             repeat after information
     * @param repeatDays
     *            till days in case of range
     * @param endDate
     *            end date to initialize till date
     */
    public void initialize(FieldData fieldData, int repeatDays, Calendar endDate) {
        if (fieldData instanceof RepeatFieldData) {
            this.fieldData = (RepeatFieldData) fieldData;
        } else {
            this.fieldData = new RepeatFieldData(fieldData.getType(), fieldData.getText(),
                    repeatDays, endDate);
        }
    }

    /**
     * gets the field data
     *
     * @return field data
     */
    public FieldData getFieldData() {
        return fieldData;
    }

    /**
     * Sets the repeat information in view
     *
     * @param rowView
     *            repeat view
     * @param startDate
     *            event start date
     */
    public void setInRow(View rowView, Calendar startDate) {
        this.rowView = rowView;
        updateSummaryText(startDate);

        View pickCalendarView = rowView.findViewById(R.id.summary_layout);
        pickCalendarView.setOnClickListener(new OnClickListener() {
            /**
             * {@inheritDoc}
             */
            @Override
            public void onClick(View view) {
                uiListener.onRepeatFieldClicked();
            }
        });
    }

    private String[] getRepeatTypeTitles(ArrayList<RepeatType> repeatTypeList) {
        String[] repeatTypeTitles = new String[repeatTypeList.size()];
        for (int i = 0; i < repeatTypeTitles.length; i++) {
            RepeatType repeatType = repeatTypeList.get(i);
            int titleId = repeatType.getTitleId();
            repeatTypeTitles[i] = context.getResources().getString(titleId);
        }
        return repeatTypeTitles;
    }

    /**
     * Shows the repeat type dialog
     */
    public void showTypeDialog() {
        AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(context);
        dialogBuilder.setTitle(R.string.repeat_title);

        final ArrayList<RepeatType> repeatTypeList = new ArrayList<RepeatType>();
        RepeatFieldData.loadSupportedRepeatTypes(repeatTypeList);
        String[] repeatTypeTitles = getRepeatTypeTitles(repeatTypeList);

        int selection = 0;
        for (int i = 0; i < repeatTypeList.size(); i++) {
            RepeatType repeatType = repeatTypeList.get(i);
            if (repeatType == fieldData.getRepeatType()) {
                selection = i;
                break;
            }
        }

        dialogBuilder.setSingleChoiceItems(repeatTypeTitles, selection,
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int index) {
                        dialog.dismiss();
                        RepeatType repeatType = repeatTypeList.get(index);
                        uiListener.onRepeatTypeSelected(repeatType);
                    }
                });
        Dialog dialog = dialogBuilder.create();
        dialog.show();
    }

    private void attachWeeklyUi(View view, Calendar startDate, Calendar endDate) {
        ViewGroup parentView = (ViewGroup) view.findViewById(R.id.repeat_dialog_view);
        View weeklyView = layoutInflater.inflate(R.layout.repeat_weekly, parentView, true);

        if (fieldData.getRepeatType() == RepeatType.EVERY_WEEK) {
            boolean[] week = fieldData.getWeekInfo();
            for (int i = 0; i < week.length; i++) {
                if (week[i]) {
                    CheckBox checkbox = (CheckBox) weeklyView.findViewById(weekCheckboxIds[i]);
                    checkbox.setChecked(true);
                }
            }

        } else {
            int weekdayStart = startDate.get(Calendar.DAY_OF_WEEK) - 1;
            int weekdayEnd = endDate.get(Calendar.DAY_OF_WEEK) - 1;

            for (int i = weekdayStart; i <= weekdayEnd; i++) {
                CheckBox checkbox = (CheckBox) weeklyView.findViewById(weekCheckboxIds[i]);
                checkbox.setChecked(true);
            }
        }
    }

    private void attachMonthlyUi(View view, Calendar startDate, Calendar endDate) {
        int weekdayStart = startDate.get(Calendar.DAY_OF_WEEK) - 1;
        int weekdayEnd = endDate.get(Calendar.DAY_OF_WEEK) - 1;
        if (weekdayStart != weekdayEnd && fieldData.getRepeatType() != RepeatType.EVERY_MONTH) {
            Log.d(LOG_TAG, "No options for range");
            return; // return if it is range
        }

        ViewGroup parentView = (ViewGroup) view.findViewById(R.id.repeat_dialog_view);
        View layoutView = layoutInflater.inflate(R.layout.repeat_monthly, parentView, true);
        ViewGroup radioGroup = (ViewGroup) layoutView.findViewById(R.id.month_options);

        int position = (fieldData.getRepeatType() == RepeatType.EVERY_MONTH && fieldData
                .isSameDay()) ? POSITION_SAME_DAY : POSITION_SAME_DATE;
        RadioButton button = (RadioButton) radioGroup.getChildAt(position);
        button.setChecked(true);

        String occurOnText = dateTexthandler.getMonthOccurrenceText(startDate);
        String monthDayPrefix = context.getResources().getString(R.string.repeat_month_same_day);

        RadioButton monthDay = (RadioButton) radioGroup.findViewById(R.id.month_day);
        monthDay.setText(monthDayPrefix + " " + occurOnText);
    }

    private void updateTillTypeUi(final View dialogLayout, TillType tillType) {
        View dateLayout = dialogLayout.findViewById(R.id.till_date_layout);
        View eventsLayout = dialogLayout.findViewById(R.id.till_events);

        if (tillType == TillType.REPEAT_TILL_DATE) {
            updateDateText(dialogLayout);

            dateLayout.setVisibility(View.VISIBLE);
            eventsLayout.setVisibility(View.GONE);

        } else if (tillType == TillType.REPEAT_NO_OF_EVENTS) {
            EditText tillEventEditText = (EditText) eventsLayout;
            tillEventEditText.setText("" + fieldData.getEvents());
            eventsLayout.setVisibility(View.VISIBLE);
            dateLayout.setVisibility(View.GONE);

        } else {
            dateLayout.setVisibility(View.GONE);
            eventsLayout.setVisibility(View.GONE);
        }

        // add click listener to date button
        dateLayout.setOnClickListener(new OnClickListener() {
            /**
             * {@inheritDoc}
             */
            @Override
            public void onClick(View view) {
                showDatePicker(dialogLayout);
            }
        });
    }

    private void updateDateText(View dialogLayout) {
        String dateText = dateTexthandler.toFomattedText(fieldData.getTillDate(),
                DateTextHandler.FMT_MMM_DY);
        TextView tillEventEditText = (TextView) dialogLayout.findViewById(R.id.date_text);
        tillEventEditText.setText(dateText);
    }

    private void showDatePicker(final View parentDialog) {
        Calendar tillDate = fieldData.getTillDate();
        AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(context);
        dialogBuilder.setTitle(R.string.repeat_title_pick_date);

        final View dialogLayout = layoutInflater.inflate(R.layout.dialog_date, null);
        final DatePicker datePickerView = (DatePicker) dialogLayout.findViewById(R.id.date_picker);

        datePickerView.updateDate(tillDate.get(Calendar.YEAR), tillDate.get(Calendar.MONTH),
                tillDate.get(Calendar.DAY_OF_MONTH));

        dialogBuilder.setView(dialogLayout);

        dialogBuilder.setPositiveButton(R.string.date_picker_ok,
                new DialogInterface.OnClickListener() {
                    /**
                     * {@inheritDoc}
                     */
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        dialog.dismiss();
                        fieldData.setTillDate(datePickerView.getYear(), datePickerView.getMonth(),
                                datePickerView.getDayOfMonth());
                        updateDateText(parentDialog);
                    }
                });
        dialogBuilder.setNegativeButton(R.string.date_picker_cancel,
                new DialogInterface.OnClickListener() {
                    /**
                     * {@inheritDoc}
                     */
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        dialog.dismiss();
                    }
                });
        Dialog dialog = dialogBuilder.create();
        dialog.setOnCancelListener(new OnCancelListener() {

            /**
             * {@inheritDoc}
             */
            @Override
            public void onCancel(DialogInterface dialog) {
                dialog.dismiss();
            }
        });
        dialog.show();
    }

    private void updateDataDialogUI(final View dialogLayout, RepeatType selectedRepeatType,
            Calendar startDate, Calendar endDate) {
        // update After Every
        EditText afterEveryEditView = (EditText) dialogLayout.findViewById(R.id.every_number);
        String text = "" + fieldData.getEveryCount();
        afterEveryEditView.setText(text);
        afterEveryEditView.setSelection(text.length());

        // update tilltype
        Spinner tillOption = (Spinner) dialogLayout.findViewById(R.id.till_spinner);
        TillType tillType = fieldData.getTillType();
        updateTillTypeUi(dialogLayout, tillType);

        if (tillType == TillType.REPEAT_TILL_DATE) {
            tillOption.setSelection(1);

        } else if (tillType == TillType.REPEAT_NO_OF_EVENTS) {
            tillOption.setSelection(2);
        }

        // update dialog category ui (daily, weekly, monthly, yearly)
        if (selectedRepeatType == RepeatType.EVERY_WEEK) {
            attachWeeklyUi(dialogLayout, startDate, endDate);

        } else if (selectedRepeatType == RepeatType.EVERY_MONTH) {
            attachMonthlyUi(dialogLayout, startDate, endDate);
        }
        tillOption.setOnItemSelectedListener(new OnItemSelectedListener() {
            /**
             * {@inheritDoc}
             */
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                if (position == POSITION_TILL_DATE) {
                    updateTillTypeUi(dialogLayout, TillType.REPEAT_TILL_DATE);
                } else if (position == POSITION_EVENTS) {
                    updateTillTypeUi(dialogLayout, TillType.REPEAT_NO_OF_EVENTS);
                } else {
                    updateTillTypeUi(dialogLayout, TillType.REPEAT_FOREVER);
                }
            }

            /**
             * {@inheritDoc}
             */
            @Override
            public void onNothingSelected(AdapterView<?> parent) {
            }
        });

        TextView dialogTypeTextView = (TextView) dialogLayout.findViewById(R.id.every_type);
        int tillId = selectedRepeatType.getTillId();
        dialogTypeTextView.setText(tillId);
    }

    /**
     * Shows the Repeat data type dialog
     *
     * @param selectedRepeatType
     *            selected repeat type by the user
     * @param startDate
     *            start date of the vent
     * @param endDate
     *            end date of the event
     */
    public void showDataDialog(final RepeatType selectedRepeatType, Calendar startDate,
            Calendar endDate) {
        AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(context);
        dialogBuilder.setTitle(R.string.repeat_title);

        final View dialogLayout = layoutInflater.inflate(R.layout.dialog_repeat, null);
        dialogBuilder.setView(dialogLayout);

        updateDataDialogUI(dialogLayout, selectedRepeatType, startDate, endDate);

        dialogBuilder.setPositiveButton(R.string.repeat_button_ok,
                new DialogInterface.OnClickListener() {
                    /**
                     * {@inheritDoc}
                     */
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        updateData(selectedRepeatType, dialogLayout);
                        uiListener.onRepeatDataChanged();
                    }
                });
        dialogBuilder.setNegativeButton(R.string.repeat_button_cancel,
                new DialogInterface.OnClickListener() {
                    /**
                     * {@inheritDoc}
                     */
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        dialog.dismiss();
                    }
                });

        Dialog dialog = dialogBuilder.create();
        dialog.show();
    }

    private void updateData(RepeatType selectedRepeatType, View dialogLayout) {
        int everyNo = 1;
        TillType tillType = TillType.REPEAT_FOREVER;
        Calendar date = fieldData.getTillDate();// tilldate already updated
        int events = 0;

        // get after every
        EditText afterEveryEditText = (EditText) dialogLayout.findViewById(R.id.every_number);
        String afterEveryText = afterEveryEditText.getText().toString();
        try {
            everyNo = Integer.parseInt(afterEveryText);
        } catch (NumberFormatException exp) {
            Log.d(LOG_TAG, "After every text box not numeric");
        }

        if (selectedRepeatType == RepeatType.ONE_TIME) {
            fieldData.setOneTimeRepeatType();
        }

        // get till type
        Spinner tillOption = (Spinner) dialogLayout.findViewById(R.id.till_spinner);
        int position = tillOption.getSelectedItemPosition();
        if (position == POSITION_EVENTS) {
            tillType = TillType.REPEAT_NO_OF_EVENTS;
            EditText eventsEditText = (EditText) dialogLayout.findViewById(R.id.till_events);
            String eventsText = eventsEditText.getText().toString();
            try {
                events = Integer.parseInt(eventsText);
            } catch (NumberFormatException exp) {
                Log.d(LOG_TAG, "Events text box not numeric");
            }

        } else if (position == POSITION_TILL_DATE) {
            tillType = TillType.REPEAT_TILL_DATE;
            // tilldate already updated
        }

        // get data specific stuff
        if (selectedRepeatType == RepeatType.EVERY_WEEK) {
            boolean[] week = new boolean[7];
            for (int i = 0; i < week.length; i++) {
                CheckBox checkbox = (CheckBox) dialogLayout.findViewById(weekCheckboxIds[i]);
                if (checkbox.isChecked()) {
                    week[i] = true;
                }
            }
            fieldData.setWeeklyRepeatType(week, everyNo, tillType, date, events);

        } else if (selectedRepeatType == RepeatType.EVERY_MONTH) {
            ViewGroup radioGroup = (ViewGroup) dialogLayout.findViewById(R.id.month_options);
            RadioButton sameDayButton = (RadioButton) radioGroup.getChildAt(POSITION_SAME_DAY);
            boolean sameDay = (sameDayButton.isChecked()) ? true : false;
            fieldData.setMonthlyRepeatType(sameDay, everyNo, tillType, date, events);
        } else {
            fieldData.setRepeatType(selectedRepeatType, everyNo, tillType, date, events);
        }
    }

    /**
     * Updates the repeat summary text on event screen
     *
     * @param startDate
     *            start date of the event
     */
    public void updateSummaryText(Calendar startDate) {
        String summaryText = "";

        RepeatType repeatType = fieldData.getRepeatType();
        if (repeatType == RepeatType.ONE_TIME) {
            summaryText = fieldData.getOneTimeSummary(context.getResources());

        } else if (repeatType == RepeatType.EVERY_DAY) {
            summaryText = fieldData.getDailySummary(context.getResources());

        } else if (repeatType == RepeatType.EVERY_WEEK) {
            summaryText = fieldData.getWeeklySummary(context.getResources(), dateTexthandler);

        } else {
            if (startDate != null) {
                String dayString = dateTexthandler.getFullDayName(startDate);
                if (repeatType == RepeatType.EVERY_MONTH) {

                    int dayNumber = startDate.get(Calendar.DATE);
                    String weekString = dateTexthandler.getWeekNumber(startDate);
                    summaryText = fieldData.getMonthlySummary(context.getResources(), dayString,
                            weekString, dayNumber);

                } else if (repeatType == RepeatType.EVERY_YEAR) {
                    summaryText = fieldData.getYearlySummary(context.getResources(), dayString);
                }
            }
        }

        if (repeatType != RepeatType.ONE_TIME) {
            TillType tillType = fieldData.getTillType();
            if (tillType == TillType.REPEAT_TILL_DATE) {
                String dateText = dateTexthandler.toFomattedText(fieldData.getTillDate(),
                        DateTextHandler.FMT_MMM_DY);
                summaryText += " " + fieldData.getTillDateSummary(context.getResources(), dateText);

            } else if (tillType == TillType.REPEAT_NO_OF_EVENTS) {
                summaryText += " " + fieldData.getTillEventsSummary(context.getResources());
            }
        }
        TextView summaryView = (TextView) rowView.findViewById(R.id.summary_text);
        summaryView.setText(summaryText);

        fieldData.setValue(summaryText);
    }

    /**
     * Resets the Repeat handler data
     */
    public void reset() {
        fieldData.setOneTimeRepeatType();
        updateSummaryText(null);
    }
}
