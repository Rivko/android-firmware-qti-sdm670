/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.savedataui;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Locale;

import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.provider.CalendarContract;
import android.text.format.Time;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import com.qualcomm.qti.sta.R;
import com.qualcomm.qti.sta.data.CategoryData;
import com.qualcomm.qti.sta.data.DateTextHandler;
import com.qualcomm.qti.sta.data.FieldData;
import com.qualcomm.qti.sta.data.RecognizedData;
import com.qualcomm.qti.sta.data.Utils;
import com.qualcomm.qti.sta.data.Utils.CategoryType;
import com.qualcomm.qti.sta.engine.FieldType;
import com.qualcomm.qti.sta.savedataui.RepeatFieldData.RepeatType;
import com.qualcomm.qti.sta.savedataui.RepeatFieldData.TillType;

/**
 * Class for add calendar event activity
 */

public class EventActivity extends SaveDataActivity {
    private static final String LOG_TAG = "EventActivity";
    private int intentDateTimeAt = -1;
    private int intentDateTimeIndex = 0;
    private DateTextHandler dateTextHandler = null;

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_contact_event);

        View avatarIcon = findViewById(R.id.avatar_icon);
        avatarIcon.setVisibility(View.GONE);

        initializeBaseData(CategoryType.EVENT);
        Intent activityIntent = getIntent();
        intentDateTimeAt = activityIntent.getIntExtra(Utils.INTENT_KEY_DATETIME_AT, -1);
        intentDateTimeIndex = activityIntent.getIntExtra(Utils.INTENT_KEY_DATETIME_INDEX, 0);
        dateTextHandler = new DateTextHandler(getResources());
    }



    /**
     * {@inheritDoc}
     */
    @Override
    protected void onSavePressed(ArrayList<FieldData> fieldDataList) {
        ContentResolver contentResolver = getContentResolver();
        ContentValues contentValues = new ContentValues();

        for (FieldData fieldData : fieldDataList) {
            switch (fieldData.getType()) {
            case EVENT_DETAILS:
                contentValues.put(CalendarContract.Events.DESCRIPTION, fieldData.getText());
                break;
            case EVENT_END_DATE:
                try {
                    int format = (((EventFieldList) fieldListHandler).isAllDay()) ? DateTextHandler.FMT_EEE_MDY
                            : DateTextHandler.FMT_EEE_MDY_HMA;
                    long endMills = dateTextHandler.toMilliSeconds(fieldData.getText(), format);
                    contentValues.put(CalendarContract.Events.DTEND, endMills);
                } catch (java.text.ParseException e) {
                    e.printStackTrace();
                    Toast.makeText(this, "Exception: " + e.getMessage(), Toast.LENGTH_SHORT).show();
                    return;
                }
                break;
            case EVENT_LOCATION:
                contentValues.put(CalendarContract.Events.EVENT_LOCATION, fieldData.getText());
                break;
            case EVENT_START_DATE:
                try {
                    int format = (((EventFieldList) fieldListHandler).isAllDay()) ? DateTextHandler.FMT_EEE_MDY
                            : DateTextHandler.FMT_EEE_MDY_HMA;
                    long startMills = dateTextHandler.toMilliSeconds(fieldData.getText(), format);

                    contentValues.put(CalendarContract.Events.DTSTART, startMills);
                } catch (java.text.ParseException e) {
                    e.printStackTrace();
                    Toast.makeText(this, "Exception: " + e.getMessage(), Toast.LENGTH_SHORT).show();
                    return;
                }
                break;
            case EVENT_TITLE:
                contentValues.put(CalendarContract.Events.TITLE, fieldData.getText());
                break;

            case EVENT_REPEAT:
                String ruleString = getRepeatRule(fieldData);
                if(ruleString != null) {
                    Log.d(LOG_TAG, "Recurring string: " + ruleString);
                    contentValues.put(CalendarContract.Events.RRULE, ruleString);
                }
                break;
            default:
                break;

            }
        }

        if (((EventFieldList) fieldListHandler).isAllDay()) {
            contentValues.put(CalendarContract.Events.ALL_DAY, 1);
        }
        contentValues.put(CalendarContract.Events.CALENDAR_ID, 1);
        contentValues.put(CalendarContract.Events.EVENT_TIMEZONE, Time.getCurrentTimezone());
        contentResolver.insert(CalendarContract.Events.CONTENT_URI, contentValues);

        Toast.makeText(this, R.string.message_calendar_saved, Toast.LENGTH_SHORT).show();
        exiting = true;
        new Handler().postDelayed(exitRunnable, INSTRUCTION_READ_TIME);
    }

    private String getRepeatRule(FieldData fieldData) {
        RepeatFieldData repeatFieldData = (RepeatFieldData) fieldData;
        RepeatType repeatType = repeatFieldData.getRepeatType();
        String ruleString = null;
        if (repeatType == RepeatType.EVERY_DAY) {
            ruleString = "FREQ=DAILY";

        } else if (repeatType == RepeatType.EVERY_WEEK) {
            ruleString = "FREQ=WEEKLY;WKST=SU;BYDAY=";

            int daysAdded = 0;
            String[] dayNames = getResources().getStringArray(R.array.two_letter_day_names);
            boolean[] week = repeatFieldData.getWeekInfo();
            for (int i = 0; i < week.length; i++) {
                if (week[i]) {
                    ruleString += (daysAdded == 0) ? dayNames[i] : "," + dayNames[i];
                    daysAdded++;
                }
            }

        } else if (repeatType == RepeatType.EVERY_MONTH) {
            EventFieldList fieldList = (EventFieldList) fieldListHandler;
            Calendar startDate = fieldList.getStartDate();
            if(repeatFieldData.isSameDay()) {
                int dayNo = startDate.get(Calendar.DAY_OF_WEEK);
                int weekNo = startDate.get(Calendar.WEEK_OF_MONTH);
                String[] dayNames = getResources().getStringArray(R.array.two_letter_day_names);
                ruleString = "FREQ=MONTHLY;WKST=SU;BYDAY=" + dayNames[dayNo - 1] + ";BYSETPOS=" + weekNo;

            } else {
                ruleString = "FREQ=MONTHLY;BYMONTHDAY=" + startDate.get(Calendar.DAY_OF_MONTH);
            }


        } else if (repeatType == RepeatType.EVERY_YEAR) {
            ruleString = "FREQ=YEARLY";
        }

        if(ruleString != null) {
            TillType tillType = repeatFieldData.getTillType();
            if (tillType == TillType.REPEAT_TILL_DATE) {
                Calendar tillDate = repeatFieldData.getTillDate();

                String preformat = "yyyyMMdd";
                SimpleDateFormat preDateFormatter = new SimpleDateFormat(preformat, Locale.US);
                String preText = preDateFormatter.format(tillDate.getTime());

                ruleString += ";UNTIL=" + preText;

            } else if (tillType == TillType.REPEAT_NO_OF_EVENTS) {
                ruleString += ";COUNT=" + repeatFieldData.getEvents();
            }
            int everyCount = repeatFieldData.getEveryCount();
            if (everyCount > 1) {
                ruleString += ";INTERVAL=" + everyCount;
            }
        }
        return ruleString;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onDonePressed(ArrayList<FieldData> fieldDataList) {
        boolean allDay = ((EventFieldList) fieldListHandler).isAllDay();
        Intent resultIntent = new Intent();
        for (FieldData fieldData : fieldDataList) {
            FieldType type = fieldData.getType();
            if (type == FieldType.EVENT_START_DATE || type == FieldType.EVENT_END_DATE) {
                try {
                    long millSeconds = dateTextHandler.toMilliSeconds(fieldData.getText(),
                            (allDay) ? DateTextHandler.FMT_EEE_MDY : DateTextHandler.FMT_EEE_MDY_HMA);
                    resultIntent.putExtra(type.getIntentKey(), millSeconds);
                } catch (ParseException e) {
                    Log.d(LOG_TAG, "date rejected: " + fieldData.getText());
                }

            } else if(type == FieldType.EVENT_ALL_DAY) {
                resultIntent.putExtra(type.getIntentKey(), allDay);

            } else if(type == FieldType.EVENT_REPEAT) {
                String ruleString = getRepeatRule(fieldData);
                if(ruleString != null) {
                    Log.d(LOG_TAG, "Recurring string: " + ruleString);
                    resultIntent.putExtra(type.getIntentKey(), ruleString);
                }
            } else {
                resultIntent.putExtra(type.getIntentKey(), fieldData.getText());
            }
        }
        resultIntent.putExtra(FieldType.EVENT_ALL_DAY.getIntentKey(), allDay);

        setResult(RESULT_OK, resultIntent);
        finish();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected FieldListHandler createListhandler(ViewGroup primaryView, ViewGroup secondaryView,
            CategoryData categoryData, ArrayList<RecognizedData> ocrDataList) {
        return new EventFieldList(this, primaryView, secondaryView, categoryData, ocrDataList,
                dateTextHandler, intentDateTimeAt, intentDateTimeIndex);
    }
}
