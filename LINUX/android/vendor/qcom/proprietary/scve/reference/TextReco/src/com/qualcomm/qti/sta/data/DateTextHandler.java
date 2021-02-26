/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.data;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;

import android.content.res.Resources;

import com.qualcomm.qti.sta.R;

/**
 * Class to handle the date time related resources and manipulations at one
 * place. this should be constructed one per activity if needed.
 */
public class DateTextHandler {
    public static final int FMT_EEE_MDY_HMA = 0;
    public static final int FMT_MDY_HMA = 1;
    public static final int FMT_EEE_MDY = 2;
    public static final int FMT_MMM_DY = 3;
    public static final int FMT_EEE_MMM_D = 4;
    public static final int FMT_HMA = 5;
    public static final int FMT_MONTH_SMALL = 6;
    public static final int FMT_MONTH_FULL = 7;
    public static final int FMT_YEAR = 8;
    public static final int FMT_WEEKDAY = 9;
    private static final int FMT_COUNT = 10;

    private final String[] format = new String[FMT_COUNT];

    private final String separator;
    private final String[] shortDayNames;
    private final String[] fullDayNames;
    private final String[] numberNames;

    /**
     * Constructor for the class
     *
     * @param resources
     *            application resources
     */
    public DateTextHandler(Resources resources) {
        format[FMT_EEE_MDY_HMA] = resources.getString(R.string.event_date_time_format_long);
        format[FMT_MDY_HMA] = resources.getString(R.string.event_date_time_format_small);
        format[FMT_MMM_DY] = resources.getString(R.string.event_date_format_small);
        format[FMT_EEE_MMM_D] = resources.getString(R.string.event_date_format_small_no_year);
        format[FMT_EEE_MDY] = resources.getString(R.string.event_date_format_long);
        format[FMT_HMA] = resources.getString(R.string.event_time_format);
        format[FMT_MONTH_SMALL] = resources.getString(R.string.event_date_format_month_small);
        format[FMT_MONTH_FULL] = resources.getString(R.string.event_date_format_month_full);
        format[FMT_YEAR] = resources.getString(R.string.event_date_format_year);
        format[FMT_WEEKDAY] = resources.getString(R.string.event_date_format_weekday);

        separator = resources.getString(R.string.event_date_separator);
        shortDayNames = resources.getStringArray(R.array.short_day_names);
        fullDayNames = resources.getStringArray(R.array.complete_day_names);
        numberNames = resources.getStringArray(R.array.number_texts);
    }

    /**
     * Parses a date range text creates the DateTimeRange object
     *
     * @param dateText
     *            range date text
     * @return DateTimeRange object
     */
    public DateTimeRange parseRange(String dateText) {
        return new DateTimeRange(dateText, separator);
    }

    /**
     * gets the "From" text from the range
     *
     * @param dateTimeRange
     *            date time range object
     * @return "From" date text
     */
    public String getFromText(DateTimeRange dateTimeRange) {
        return dateTimeRange.getFromFormattedDate(format[FMT_EEE_MDY_HMA]);
    }

    /**
     * gets the from text "To" the range
     *
     * @param dateTimeRange
     *            date time range object
     * @return "To" date text
     */
    public String getToText(DateTimeRange dateTimeRange) {
        return dateTimeRange.getToFormattedDate(format[FMT_EEE_MDY_HMA]);
    }

    /**
     * gets the day's full name from specified calendar date
     *
     * @param date
     *            calendar date
     * @return day's full name
     */
    public String getFullDayName(Calendar date) {
        int dayNumber = date.get(Calendar.DAY_OF_WEEK);
        return fullDayNames[dayNumber - 1];
    }

    /**
     * gets the day's short name
     *
     * @param index
     *            index of the day in week (starting from sunday)
     * @return day's short name
     */
    public String getShortDayName(int index) {
        return shortDayNames[index];
    }

    /**
     * gets the week number string in the month
     *
     * @param date
     *            calendar date
     * @return week number string
     */
    public String getWeekNumber(Calendar date) {
        int weekNumber = date.get(Calendar.WEEK_OF_MONTH);
        return numberNames[weekNumber - 1];
    }

    /**
     * gets the month occurrence string
     *
     * @param date
     *            calendar date
     * @return month occurence string
     */
    public String getMonthOccurrenceText(Calendar date) {
        return getWeekNumber(date) + " " + getFullDayName(date);
    }

    /**
     * converts the date to tect based on specified format
     *
     * @param date
     *            calendar date
     * @param formatId
     *            format which needs to be used
     * @return formatted text
     */
    public String toFomattedText(Calendar date, int formatId) {
        SimpleDateFormat dateFormatter = new SimpleDateFormat(format[formatId], Locale.US);
        return dateFormatter.format(date.getTime());
    }

    /**
     * gets the comma separated days text as specified in week boolean array
     *
     * @param week
     *            week boolean array, true means day is to be consider, false
     *            otherwise
     * @return comma separated days text
     */
    public String getDayStringForWeek(boolean week[]) {
        int dayNumber = 0;
        int firstDayIndex = -1;
        String dayString = "";
        for (int i = 0; i < week.length; i++) {
            if (week[i]) {
                if (dayNumber == 0) {
                    dayString = shortDayNames[i];
                    firstDayIndex = i;

                } else {
                    dayString += ", " + shortDayNames[i];
                }
                dayNumber++;
            }
        }
        if (dayNumber == 1) {
            dayString = fullDayNames[firstDayIndex];
        }
        return dayString;
    }

    /**
     * gets the action title from the date Text specified
     *
     * @param dateText
     *            date text
     * @return action title
     */
    public String getActionTitle(DateTimeData data, String dateTime) {
        String actionTitle = "";
        DateTimeRange dateTimeRange = new DateTimeRange(dateTime, separator);
        if (dateTimeRange.isDaySame()) {// single day or repeat
            if (data.getRepeatDays() == 0) {// single day
            actionTitle = dateTimeRange.getFromFormattedDate(format[FMT_MMM_DY]);
                if (!dateTimeRange.isHourMinutesNull()) {
                String timeText = dateTimeRange.getFromFormattedDate(format[FMT_HMA]) + " - "
                        + dateTimeRange.getToFormattedDate(format[FMT_HMA]);

                if (dateTimeRange.areYearsCurrent()) {
                    actionTitle = dateTimeRange.getFromFormattedDate(format[FMT_EEE_MMM_D]);
                }
                actionTitle = actionTitle + "  " + timeText;
            }
            } else {// repeat
                actionTitle = getTitleForRepeatInfo(data, dateTimeRange);
            }

        } else {// range
            String dateFormat = (dateTimeRange.isHourMinutesNull()) ? format[FMT_MMM_DY]
                    : format[FMT_MDY_HMA];
            actionTitle = dateTimeRange.getFromFormattedDate(dateFormat) + " - "
                    + dateTimeRange.getToFormattedDate(dateFormat);
        }
        return actionTitle;
    }

    private String getTitleForRepeatInfo(DateTimeData data, DateTimeRange dateTimeRange) {
        String actionTitle = dateTimeRange.getFromFormattedDate(format[FMT_WEEKDAY]) + " - "
                + dateTimeRange.getFromFormattedDate(format[FMT_WEEKDAY], data.getRepeatDays());

        if (!dateTimeRange.isHourMinutesNull()) {
            String timeText = dateTimeRange.getFromFormattedDate(format[FMT_HMA]) + " - "
                    + dateTimeRange.getToFormattedDate(format[FMT_HMA]);
            actionTitle = actionTitle + ": " + timeText;
        }
        return actionTitle;
    }

    /**
     * Converts dateText into milliseconds
     *
     * @param dateText
     *            date text which needs to be converted
     * @param formatId
     *            date text format
     * @return time milliseconds
     * @throws java.text.ParseException
     *             exception in case the method fails to parse the date text
     */
    public long toMilliSeconds(String dateText, int formatId) throws java.text.ParseException {
        SimpleDateFormat simpleDateFormat = new SimpleDateFormat(format[formatId], Locale.US);
        Date date = simpleDateFormat.parse(dateText);
        Calendar calendar = Calendar.getInstance();
        calendar.setTime(date);
        return calendar.getTimeInMillis();
    }

    /**
     * Converts the dateText into date based on format specified.
     *
     * @param dateText
     *            date text which needs to be converted
     * @param formatId
     *            date text format
     * @return parsed out date
     * @throws ParseException
     *             Exception in case of method failed to parse successfully
     */
    public Calendar toCalendar(String dateText, int formatId) throws ParseException {
        SimpleDateFormat simpleDateFormat = new SimpleDateFormat(format[formatId], Locale.US);
        Date date = simpleDateFormat.parse(dateText);
        Calendar calendar = Calendar.getInstance();
        calendar.setTime(date);

        return calendar;
    }

    /**
     * Check if the date is null or not
     *
     * @param date
     *            date which needs to be checked
     * @return true if date is null, false otherwise
     */
    public boolean isDateNull(String date) {
        DateTimeRange datetimeRange = new DateTimeRange(date, separator);
        return datetimeRange.isDateNull();
    }

    /**
     * Gets the number days between two dates
     *
     * @param date1
     *            date one
     * @param date2
     *            date two
     * @return date difference in days
     */
    public int getDayDiff(String date1, String date2) {
        DateTimeRange datetimeRange1 = new DateTimeRange(date1, separator);
        DateTimeRange datetimeRange2 = new DateTimeRange(date2, separator);

        return datetimeRange2.getFromDayDiff(datetimeRange1);
    }
}
