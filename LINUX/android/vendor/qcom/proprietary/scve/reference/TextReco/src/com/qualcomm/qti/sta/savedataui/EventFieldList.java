/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.savedataui;

import java.text.ParseException;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnCancelListener;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.DatePicker;
import android.widget.TextView;
import android.widget.TimePicker;
import android.widget.Toast;

import com.qualcomm.qti.sta.R;
import com.qualcomm.qti.sta.data.CategoryData;
import com.qualcomm.qti.sta.data.DateTextHandler;
import com.qualcomm.qti.sta.data.DateTimeData;
import com.qualcomm.qti.sta.data.DateTimeRange;
import com.qualcomm.qti.sta.data.FieldData;
import com.qualcomm.qti.sta.data.RecognizedData;
import com.qualcomm.qti.sta.data.Utils.CategoryType;
import com.qualcomm.qti.sta.engine.FieldType;
import com.qualcomm.qti.sta.savedataui.RepeatFieldData.RepeatType;
import com.qualcomm.qti.sta.savedataui.RepeatHandler.RepeatUIListener;

/**
 * Class for handling event field list
 */
public class EventFieldList extends FieldListHandler implements RepeatUIListener {
    private static final String LOG_TAG = "EventFieldList";

    private static final int DEFAULT_START_HOUR = 8;
    private static final int DEFAULT_END_HOUR = 17;

    private boolean allDayEvent = false;
    private final DateTextHandler dateTextHandler;

    private class PreviousTime {
        private int startHour = DEFAULT_START_HOUR;
        private int startMinute = 0;
        private int endHour = DEFAULT_END_HOUR;
        private int endMinute = 0;

        private void set(int startHour, int startMinute, int endHour, int endMinute) {
            this.startHour = startHour;
            this.startMinute = startMinute;
            this.endHour = endHour;
            this.endMinute = endMinute;
        }
    }

    private PreviousTime previousTime = null;

    private FieldRow startDateRow = null;
    private FieldRow endDateRow = null;
    private Calendar startDate = null;
    private Calendar endDate = null;
    private int repeatForDays = 0;

    private RepeatHandler repeatHandler = null;

    /**
     * Constructor for the class
     *
     * @param context
     *            Application context
     * @param primaryView
     *            primary list view container
     * @param secondaryView
     *            secondary list view container
     * @param categoryData
     *            previous category data
     * @param dataList
     *            sta data list
     * @param dateTextHandler
     *        Handler to perform date related tasks
     * @param defaultDateAt
     *            default Date data list index
     * @param defaultDateIndex
     *            default Date index of the date in data-list to be set
     */
    public EventFieldList(Context context, ViewGroup primaryView, ViewGroup secondaryView,
            CategoryData categoryData, ArrayList<RecognizedData> dataList,
            DateTextHandler dateTextHandler, int defaultDateAt, int defaultDateIndex) {
        super(context, primaryView, secondaryView);

        initializeCategoryTypeData(CategoryType.EVENT);

        this.dateTextHandler = dateTextHandler;
        loadEventFields(categoryData, dataList, defaultDateAt, defaultDateIndex);
        constructFieldsList();
    }

    /**
     * get the all day status
     *
     * @return all day status
     */
    public boolean isAllDay() {
        return allDayEvent;
    }

    /**
     * gets the event start date
     *
     * @return start date
     */
    public Calendar getStartDate() {
        return startDate;
    }

    private void loadEventFields(CategoryData categoryData, ArrayList<RecognizedData> dataList,
            int defaultDateAt, int defaultDateIndex) {
        fieldOptions.add(noneText);
        boolean newEventDate = true;

        // load previous data
        if (categoryData != null) {
            for (FieldData fieldData : categoryData.data) {
                if (fieldData.getType() == FieldType.EVENT_START_DATE) {
                    newEventDate = false;
                }
                insertKnownField(fieldData);
            }
        }
        if (defaultDateAt != -1) {
            DateTimeData data = (DateTimeData) dataList.get(defaultDateAt);
            handleDate(data, newEventDate, defaultDateIndex);
            newEventDate = false;
        }
        // look into ocr data
        int categoryId = 0;
        if (categoryData != null) {
            categoryId = categoryData.dataId;
        }
        for (RecognizedData data : dataList) {
            if (data.id > categoryId) {// load new data
                FieldType fieldType = FieldType.nativeTypeToEventFieldType(data.nativeType);
                if (fieldType == FieldType.EVENT_START_DATE) {
                    handleDate((DateTimeData) data, newEventDate, 0);
                    newEventDate = false;

                } else {
                    if (fieldType == FieldType.UNKNOWN) { // general text
                        insertUnknownField(data.getText());
                    } else if (fieldType != FieldType.NONE) {
                        FieldData fieldData = new FieldData(fieldType, data.getText());
                        insertKnownField(fieldData);
                    }
                    fieldOptions.add(data.getText());
                }
            } else {
                if (FieldType.isDateText(data.nativeType)) {
                    handleDate((DateTimeData) data, false, 0);
                } else {
                    fieldOptions.add(data.getText());
                }
            }
        }
    }

    private void handleDate(DateTimeData data, boolean insert, int index) {
        String rawText = data.getDateAt(index);
        DateTimeRange dateTimeRange = dateTextHandler.parseRange(rawText);
        String startDateText = dateTextHandler.getFromText(dateTimeRange);
        String endDateText = dateTextHandler.getToText(dateTimeRange);

        if (insert) {
            insertKnownField(new FieldData(FieldType.EVENT_START_DATE, startDateText));
            insertKnownField(new FieldData(FieldType.EVENT_END_DATE, endDateText));
            updateDateData(data);
        }
    }

    private void updateDateUI(FieldRow fieldRow, ViewGroup viewGroup) {
        fieldRow.view = layoutInflater.inflate(R.layout.title_summary_item, viewGroup, false);
        setCaptionInRow(fieldRow.view, fieldRow.data);
        setDateInRow(fieldRow.view, fieldRow.data);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected FieldRow handleCategoryField(FieldData fieldData,
            HashMap<FieldType, FieldRow> fieldsHashMap, ViewGroup viewGroup) {
        FieldRow fieldRow = null;
        FieldType fieldType = fieldData.getType();
        if (fieldType == FieldType.EVENT_START_DATE) {
            try {
                startDate = dateTextHandler.toCalendar(fieldData.getText(),
                        DateTextHandler.FMT_EEE_MDY_HMA);
            } catch (ParseException e) {
                Log.d(LOG_TAG, "exception while fetching calendar dates");
            }
            fieldRow = new FieldRow(fieldData);
            updateDateUI(fieldRow, viewGroup);
            startDateRow = fieldRow;

        } else if (fieldType == FieldType.EVENT_END_DATE) {
            try {
                endDate = dateTextHandler.toCalendar(fieldData.getText(),
                        DateTextHandler.FMT_EEE_MDY_HMA);

            } catch (ParseException e) {
                Log.d(LOG_TAG, "exception while fetching calendar dates");
            }

            fieldRow = new FieldRow(fieldData);
            endDateRow = fieldRow;
            updateDateUI(fieldRow, viewGroup);

        } else if (fieldType == FieldType.EVENT_ALL_DAY) {
            fieldRow = new FieldRow(fieldData);
            fieldRow.view = layoutInflater.inflate(R.layout.check_item, viewGroup, false);
            setAllDayInRow(fieldRow.view, fieldRow.data);

        } else if (fieldType == FieldType.EVENT_REPEAT) {
            repeatHandler = new RepeatHandler(context, dateTextHandler, layoutInflater, this);
            repeatHandler.initialize(fieldData, repeatForDays, endDate);
            fieldRow = new FieldRow(repeatHandler.getFieldData());
            fieldRow.view = layoutInflater.inflate(R.layout.title_summary_item, viewGroup, false);
            setCaptionInRow(fieldRow.view, fieldRow.data);
            repeatHandler.setInRow(fieldRow.view, startDate);

        }
        return fieldRow;
    }

    private void updateDateData(DateTimeData data) {
        if (startDate == null) {
            createDummyDates();
            Toast.makeText(context, R.string.message_no_dates, Toast.LENGTH_SHORT).show();
        }

        repeatForDays = 0;
        allDayEvent = (startDate.get(Calendar.HOUR) == 0 && endDate.get(Calendar.HOUR) == 0
                && startDate.get(Calendar.MINUTE) == 0 && endDate.get(Calendar.MINUTE) == 0);

        if (data.getRepeatDays() != 0) {
            repeatForDays = data.getRepeatDays();

        } else {
            boolean isSameDay = (startDate.get(Calendar.YEAR) == endDate.get(Calendar.YEAR)
                    && startDate.get(Calendar.MONTH) == endDate.get(Calendar.MONTH) && startDate
                    .get(Calendar.DAY_OF_MONTH) == endDate.get(Calendar.DAY_OF_MONTH));
            if (!isSameDay) {// for ranges
                int startYear = startDate.get(Calendar.YEAR);
                int startMonth = startDate.get(Calendar.MONTH);
                int startDay = startDate.get(Calendar.DATE);

                int startHour = endDate.get(Calendar.HOUR_OF_DAY);
                int startMinute = endDate.get(Calendar.MINUTE);

                Calendar lastDayDate = (Calendar) endDate.clone();
                lastDayDate.set(Calendar.HOUR_OF_DAY, startHour);
                lastDayDate.set(Calendar.MINUTE, startMinute);

                long diff = lastDayDate.getTimeInMillis() - startDate.getTimeInMillis();
                repeatForDays = (int) (diff / (24 * 60 * 60 * 1000));

                endDate.set(startYear, startMonth, startDay);
                String dateTextVlaue = dateTextHandler.toFomattedText(endDate,
                        DateTextHandler.FMT_EEE_MDY_HMA);
                endDateRow.data.setValue(dateTextVlaue);

                TextView dateButtonView = (TextView) endDateRow.view
                        .findViewById(R.id.summary_text);
                dateButtonView.setText(dateTextVlaue);
            }
        }
    }

    private void setDateInRow(final View rowView, final FieldData data) {
        TextView dateButtonView = (TextView) rowView.findViewById(R.id.summary_text);
        dateButtonView.setText(data.getText());

        View pickCalendarView = rowView.findViewById(R.id.summary_layout);
        pickCalendarView.setOnClickListener(new OnClickListener() {
            /**
             * {@inheritDoc}
             */
            @Override
            public void onClick(View view) {
                showDateTimePicker(data);
            }
        });
    }

    private void updateStartDate(Calendar calendar, int formatId) {
        startDate = calendar;
        updateFormattedText(startDateRow.view, startDateRow.data, startDate, formatId);
    }

    private void updateEndDate(Calendar calendar, int formatId) {
        endDate = calendar;
        updateFormattedText(endDateRow.view, endDateRow.data, endDate, formatId);
    }

    private void updateFormattedText(View view, FieldData data, Calendar calendar, int formatId) {
        String newValue = dateTextHandler.toFomattedText(calendar, formatId);
        data.setValue(newValue);

        TextView dateButtonView = (TextView) view.findViewById(R.id.summary_text);
        dateButtonView.setText(newValue);
    }

    private void showDateTimePicker(final FieldData data) {
        AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(context);
        dialogBuilder.setTitle(R.string.title_pick_datetime);

        final View dialogLayout = layoutInflater.inflate(R.layout.dialog_date_time, null);
        final DatePicker datePickerView = (DatePicker) dialogLayout.findViewById(R.id.date_picker);
        final TimePicker timePickerView = (TimePicker) dialogLayout.findViewById(R.id.time_picker);

        timePickerView.setVisibility((allDayEvent) ? View.GONE : View.VISIBLE);

        Calendar calendar = (data.getType() == FieldType.EVENT_START_DATE) ? startDate : endDate;
        datePickerView.updateDate(calendar.get(Calendar.YEAR), calendar.get(Calendar.MONTH),
                calendar.get(Calendar.DAY_OF_MONTH));
        timePickerView.setCurrentHour(calendar.get(Calendar.HOUR_OF_DAY));
        timePickerView.setCurrentMinute(calendar.get(Calendar.MINUTE));

        dialogBuilder.setView(dialogLayout);

        dialogBuilder.setPositiveButton(R.string.date_picker_ok,
                new DialogInterface.OnClickListener() {
                    /**
                     * {@inheritDoc}
                     */
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        dialog.dismiss();

                        Calendar calendar = Calendar.getInstance();
                        calendar.set(datePickerView.getYear(), datePickerView.getMonth(),
                                datePickerView.getDayOfMonth(), timePickerView.getCurrentHour(),
                                timePickerView.getCurrentMinute(), 0);

                        if (allDayEvent) {
                            updateStartDate(calendar, DateTextHandler.FMT_EEE_MDY);
                            updateEndDate(calendar, DateTextHandler.FMT_EEE_MDY);

                        } else if (data.getType() == FieldType.EVENT_START_DATE) {
                            updateStartDate(calendar, DateTextHandler.FMT_EEE_MDY_HMA);
                        } else {
                            updateEndDate(calendar, DateTextHandler.FMT_EEE_MDY_HMA);
                        }
                        repeatHandler.reset();
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

    private void createDummyDates() {
        startDate = Calendar.getInstance();
        startDate.set(Calendar.MINUTE, 0);
        int currentHour = startDate.get(Calendar.HOUR_OF_DAY);
        int toHour = currentHour + 1;
        if (toHour > 24) {
            toHour = currentHour;
        }

        endDate = Calendar.getInstance();
        endDate.set(Calendar.HOUR_OF_DAY, toHour);
        endDate.set(Calendar.MINUTE, 0);

        String dateText = dateTextHandler
                .toFomattedText(startDate, DateTextHandler.FMT_EEE_MDY_HMA);
        startDateRow = new FieldRow(FieldType.EVENT_START_DATE, dateText);
        updateDateUI(startDateRow, primaryView);
        primaryFieldMap.put(FieldType.EVENT_START_DATE, startDateRow);

        dateText = dateTextHandler.toFomattedText(endDate, DateTextHandler.FMT_EEE_MDY_HMA);
        endDateRow = new FieldRow(FieldType.EVENT_END_DATE, dateText);
        updateDateUI(endDateRow, primaryView);
        primaryFieldMap.put(FieldType.EVENT_END_DATE, endDateRow);
    }

    private void setAllDayInRow(View rowView, final FieldData data) {
        // update caption
        TextView captionView = (TextView) rowView.findViewById(R.id.field_caption);
        String caption = context.getResources().getString(data.getType().getCaptionId());
        captionView.setText(caption);

        // update value
        CheckBox allDayCheckBox = (CheckBox) rowView.findViewById(R.id.all_day_value);
        if (!data.getText().isEmpty() && data.getText().equals("true")) {
            allDayEvent = true;
        }
        data.setValue("" + allDayEvent);
        allDayCheckBox.setChecked(allDayEvent);
        if (allDayEvent) {
            updateDateTexts();
        }

        // set up click listener
        allDayCheckBox.setOnClickListener(new OnClickListener() {
            /**
             * {@inheritDoc}
             */
            @Override
            public void onClick(View view) {
                CheckBox allDayCheckBox = (CheckBox) view;
                allDayEvent = allDayCheckBox.isChecked();
                updateDateTexts();
                data.setValue("" + allDayEvent);
            }
        });
    }

    private void updateDateTexts() {
        int formatId;
        if (allDayEvent) {
            formatId = DateTextHandler.FMT_EEE_MDY;
            // save previous hour min
            if (previousTime == null) {
                previousTime = new PreviousTime();
            } else {
                previousTime.set(startDate.get(Calendar.HOUR_OF_DAY),
                        startDate.get(Calendar.MINUTE), endDate.get(Calendar.HOUR_OF_DAY),
                        endDate.get(Calendar.MINUTE));
            }

            startDate.set(Calendar.HOUR_OF_DAY, DEFAULT_START_HOUR);
            startDate.set(Calendar.MINUTE, 0);

            endDate.setTimeInMillis(startDate.getTimeInMillis());

        } else {
            formatId = DateTextHandler.FMT_EEE_MDY_HMA;
            // restore previous hour minutes
            if (previousTime != null) {
                startDate.set(Calendar.HOUR_OF_DAY, previousTime.startHour);
                startDate.set(Calendar.MINUTE, previousTime.startMinute);

                endDate.set(Calendar.HOUR_OF_DAY, previousTime.endHour);
                endDate.set(Calendar.MINUTE, previousTime.endMinute);
            }
        }
        updateStartDate(startDate, formatId);
        updateEndDate(endDate, formatId);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onRepeatTypeSelected(RepeatType repeatType) {
        if (repeatType == RepeatType.EVERY_DAY || repeatType == RepeatType.EVERY_MONTH
                || repeatType == RepeatType.EVERY_WEEK || repeatType == RepeatType.EVERY_YEAR) {
            repeatHandler.showDataDialog(repeatType, startDate, endDate);

        } else {
            repeatHandler.reset();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onRepeatFieldClicked() {
        repeatHandler.showTypeDialog();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onRepeatDataChanged() {
        repeatHandler.updateSummaryText(startDate);
    }
}
