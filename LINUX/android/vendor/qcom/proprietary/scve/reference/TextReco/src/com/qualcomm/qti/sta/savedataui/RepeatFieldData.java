/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.savedataui;

import java.util.ArrayList;
import java.util.Calendar;

import android.content.res.Resources;

import com.qualcomm.qti.sta.R;
import com.qualcomm.qti.sta.data.DateTextHandler;
import com.qualcomm.qti.sta.data.FieldData;
import com.qualcomm.qti.sta.engine.FieldType;

/**
 * Class for handling Repeat field data
 */
public class RepeatFieldData extends FieldData {
    private static final int MINIMUM_AFTER_EVERY_VALUE = 1;
    private static final int MINIMUM_EVENTS = 1;

    private static final int REPEAT_DATE_OFFSET = 7;

    /**
     * enum for repeat type
     */
    enum RepeatType {
        ONE_TIME(R.string.repeat_one_time, 0), EVERY_DAY(R.string.repeat_daily,
                R.string.repeat_till_day), EVERY_WEEK(R.string.repeat_weekly,
                R.string.repeat_till_week), EVERY_MONTH(R.string.repeat_monthly,
                R.string.repeat_till_month), EVERY_YEAR(R.string.repeat_yearly,
                R.string.repeat_till_year);

        private final int titleId;
        private final int tillId;

        private RepeatType(int titleId, int tillId) {
            this.titleId = titleId;
            this.tillId = tillId;
        }

        /**
         * gets the title resource id for the ui
         *
         * @return title resource id
         */
        int getTitleId() {
            return titleId;
        }

        /**
         * gets the till resource id for the ui
         *
         * @return till resource id
         */
        int getTillId() {
            return tillId;
        }
    }

    /**
     * enum for the repeat till type
     */
    enum TillType {
        REPEAT_FOREVER, REPEAT_TILL_DATE, REPEAT_NO_OF_EVENTS;
    }

    private RepeatType repeatType = RepeatType.ONE_TIME;
    private int everyCount = MINIMUM_AFTER_EVERY_VALUE;
    private TypeData typeData = null;

    private class TypeData {
        private TillType tillType = TillType.REPEAT_FOREVER;
        private Calendar tillDate = null;
        private int tillEvents = MINIMUM_EVENTS;

        private boolean[] week = null;
        private boolean sameDay = false;

        private void setTillDate(Calendar date) {
            tillType = TillType.REPEAT_TILL_DATE;
            tillDate = date;
        }

        private void setTillEvents(int events) {
            tillType = TillType.REPEAT_NO_OF_EVENTS;
            tillEvents = events;
        }
    }

    /**
     * Constructor for the class
     *
     * @param fieldType
     *            field type
     * @param value
     *            field text
     * @param repeatAfter
     *            repeat after information
     * @param tillDays
     *            till days in case of range
     * @param eventdate
     *            end date to initialize till date
     */
    RepeatFieldData(FieldType fieldType, String value, int tillDays,
            Calendar eventdate) {
        super(fieldType, value);
        typeData = new TypeData();

        if (tillDays > 0) {
            if (tillDays <= 7) {
                this.repeatType = RepeatType.EVERY_WEEK;
                typeData.week = new boolean[7];
                setTillType(TillType.REPEAT_NO_OF_EVENTS, null, 1);

                int startDayNo = eventdate.get(Calendar.DAY_OF_WEEK);
                for (int i = startDayNo; i <= (startDayNo + tillDays); i++) {
                    int dayNo = (i - 1) % 7;
                    typeData.week[dayNo] = true;
                }
                setDefaultTillDate(eventdate, REPEAT_DATE_OFFSET);

            } else {
                repeatType = RepeatType.EVERY_DAY;
                typeData.tillType = TillType.REPEAT_TILL_DATE;
                setDefaultTillDate(eventdate, tillDays);

            }
        } else {
            repeatType = RepeatType.ONE_TIME;
            setDefaultTillDate(eventdate, REPEAT_DATE_OFFSET);
        }
    }

    private void setDefaultTillDate(Calendar eventdate, int daysToAdd) {
        if (eventdate == null) {
            return;
        }
        int year = eventdate.get(Calendar.YEAR);
        int month = eventdate.get(Calendar.MONTH);
        int day = eventdate.get(Calendar.DAY_OF_MONTH);

                typeData.tillDate = Calendar.getInstance();
                typeData.tillDate.set(year, month, day);

        if (daysToAdd > 0) {
            typeData.tillDate.add(Calendar.DAY_OF_MONTH, daysToAdd);
        }
    }

    /**
     * method to load all supported repeat types
     *
     * @param repeatTypeList
     *            repeat type array list
     */
    public static void loadSupportedRepeatTypes(ArrayList<RepeatType> repeatTypeList) {
        if (repeatTypeList != null) {
            for (RepeatType repeatType : RepeatType.values()) {
                repeatTypeList.add(repeatType);
            }
        }
    }

    /**
     * Gets the one time summary
     *
     * @param resources
     *            application resources
     * @return summary
     */
    public String getOneTimeSummary(Resources resources) {
        return resources.getString(R.string.repeat_summary_one_time);
    }

    /**
     * Gets the daily text summary
     *
     * @param resources
     *            application resources
     * @return summary
     */
    public String getDailySummary(Resources resources) {
        if (everyCount > MINIMUM_AFTER_EVERY_VALUE) {
            String baseText = resources.getString(R.string.repeat_summary_daily_every);
            return String.format(baseText, everyCount);
        } else {
            return resources.getString(R.string.repeat_summary_daily);
        }
    }

    /**
     * Gets the weekly text summary
     *
     * @param resources
     *            application resources
     * @param dateTextHandler
     *            date text handler
     * @return summary
     */
    public String getWeeklySummary(Resources resources, DateTextHandler dateTextHandler) {
        String dayString = dateTextHandler.getDayStringForWeek(typeData.week);

        if (everyCount > MINIMUM_AFTER_EVERY_VALUE) {
            String baseText = resources.getString(R.string.repeat_summary_weekly_every);
            return String.format(baseText, everyCount, dayString);
        } else {
            String baseText = resources.getString(R.string.repeat_summary_weekly);
            return String.format(baseText, dayString);
        }
    }

    /**
     * Gets the monthly text summary
     *
     * @param resources
     *            application resources
     * @param dayName
     *            day name
     * @param weekNo
     *            week number
     * @param dayNumber
     *            day number
     * @return summary
     */
    public String getMonthlySummary(Resources resources, String dayName, String weekNo,
            int dayNumber) {
        String summaryText = "";
        if (typeData.sameDay) {
            String dayDescription = weekNo + " " + dayName;
            if (everyCount > MINIMUM_AFTER_EVERY_VALUE) {
                String baseText = resources.getString(R.string.repeat_summary_monthly_day_every);
                summaryText = String.format(baseText, everyCount, dayDescription);
            } else {
                String baseText = resources.getString(R.string.repeat_summary_monthly_day);
                summaryText = String.format(baseText, dayDescription);
            }
        } else {
            if (everyCount > MINIMUM_AFTER_EVERY_VALUE) {
                String baseText = resources.getString(R.string.repeat_summary_monthly_date_every);
                summaryText = String.format(baseText, everyCount, dayNumber);
            } else {
                String baseText = resources.getString(R.string.repeat_summary_monthly_date);
                summaryText = String.format(baseText, dayNumber);
            }
        }
        return summaryText;
    }

    /**
     * Gets the yearly text summary
     *
     * @param resources
     *            application resources
     * @param dayName
     *            day name
     * @return summary
     */
    public String getYearlySummary(Resources resources, String dayName) {
        if (everyCount > MINIMUM_AFTER_EVERY_VALUE) {
            String baseText = resources.getString(R.string.repeat_summary_yearly_every);
            return String.format(baseText, everyCount, dayName);
        } else {
            String baseText = resources.getString(R.string.repeat_summary_yearly);
            return String.format(baseText, dayName);
        }
    }

    /**
     * gets the till date summary
     *
     * @param resources
     *            application resources
     * @param dateText
     *            date text
     * @return summary
     */
    public String getTillDateSummary(Resources resources, String dateText) {
        String baseText = resources.getString(R.string.repeat_summary_till_date);
        return String.format(baseText, dateText);
    }

    /**
     * gets the till events summary
     *
     * @param resources
     *            application resources
     * @return summary
     */
    public String getTillEventsSummary(Resources resources) {
        String summary = "";
        if (typeData.tillEvents == 1) {
            summary = resources.getString(R.string.repeat_summary_till_one_event);

        } else {
        String baseText = resources.getString(R.string.repeat_summary_till_events);
            summary = String.format(baseText, typeData.tillEvents);
        }
        return summary;
    }

    /**
     * gets the repeat type
     *
     * @return repeat type
     */
    public RepeatType getRepeatType() {
        return repeatType;
    }

    /**
     * Sets the one time repeat type
     */
    public void setOneTimeRepeatType() {
        repeatType = RepeatType.ONE_TIME;
    }

    /**
     * Sets repeat type
     *
     * @param repeatType
     *            repeat type
     * @param afterEvery
     *            after every number
     * @param tillType
     *            till type
     * @param date
     *            till date
     * @param events
     *            till number of events
     */
    public void setRepeatType(RepeatType repeatType, int everyCount, TillType tillType,
            Calendar date, int events) {
        this.repeatType = repeatType;
        this.everyCount = everyCount;
        setTillType(tillType, date, events);
    }

    /**
     * Sets repeat type for weekly
     *
     * @param week
     *            week days information
     * @param afterEvery
     *            after every number
     * @param tillType
     *            till type
     * @param date
     *            till date
     * @param events
     *            till number of events
     */
    public void setWeeklyRepeatType(boolean[] week, int afterEvery, TillType tillType,
            Calendar date, int events) {
        repeatType = RepeatType.EVERY_WEEK;
        typeData.week = week;
        everyCount = afterEvery;
        setTillType(tillType, date, events);
    }

    /**
     * Sets repeat type for monthly
     *
     * @param sameDay
     *            true if the repeat is on same day, false otherwise
     * @param afterEvery
     *            after every number
     * @param tillType
     *            till type
     * @param date
     *            till date
     * @param events
     *            till number of events
     */
    public void setMonthlyRepeatType(boolean sameDay, int afterEvery, TillType tillType,
            Calendar date, int events) {
        repeatType = RepeatType.EVERY_MONTH;
        typeData.sameDay = sameDay;
        everyCount = afterEvery;
        setTillType(tillType, date, events);
    }

    private void setTillType(TillType tillType, Calendar date, int events) {
        if (tillType == TillType.REPEAT_NO_OF_EVENTS) {
            typeData.setTillEvents(events);

        } else if (tillType == TillType.REPEAT_TILL_DATE) {
            typeData.setTillDate(date);
        } else {
            typeData.tillType = TillType.REPEAT_FOREVER;
        }
    }

    /**
     * gets the week-days information
     *
     * @return week-days information
     */
    public boolean[] getWeekInfo() {
        return typeData.week;
    }

    /**
     * Checks if the monthly data is for day or date
     *
     * @return true if data is for same day, false otherwise
     */
    public boolean isSameDay() {
        if (repeatType == RepeatType.EVERY_MONTH) {
            return typeData.sameDay;
        }
        return false;
    }

    /**
     * gets the till type associated with field data
     *
     * @return till type
     */
    public TillType getTillType() {
        return typeData.tillType;
    }

    /**
     * gets the number of events
     *
     * @return events
     */
    public int getEvents() {
        return typeData.tillEvents;
    }

    /**
     * gets the every number
     *
     * @return every number
     */
    public int getEveryCount() {
        return everyCount;
    }

    /**
     * gets the till date
     *
     * @return till date
     */
    public Calendar getTillDate() {
        return typeData.tillDate;
    }

    /**
     * Sets till date
     *
     * @param year
     *            year for the date
     * @param month
     *            month of the date
     * @param dayOfMonth
     *            day of the date
     */
    public void setTillDate(int year, int month, int dayOfMonth) {
        typeData.tillDate.clear();
        typeData.tillDate.set(year, month, dayOfMonth);
    }
}
