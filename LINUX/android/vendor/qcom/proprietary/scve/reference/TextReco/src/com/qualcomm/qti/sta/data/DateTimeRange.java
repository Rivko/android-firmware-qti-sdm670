/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.data;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Locale;

import android.util.Log;

/**
 * class for handling date-time range
 */
public class DateTimeRange {
    private static final String LOG_TAG = "DateTimeRange";
    public static final int LIB_FORMAT_LENGTH = 35;// YYYYMMDDWWWHHMMSS-YYYYMMDDWWWHHMMSS
    private static final int DAYS_IN_WEEK = 7;

    private static class DateTime {
        private static final int YEAR = 0;
        private static final int MONTH = 1;
        private static final int DAY = 2;
        private static final int HOUR = 3;
        private static final int MINUTE = 4;
        private static final int DATE_PARTS = 5;

        private static final int INDEX_YEAR = 0;
        private static final int INDEX_MONTH = 4;
        private static final int INDEX_DATE = 6;
        private static final int INDEX_DAY_NAME = 8;
        private static final int INDEX_HOUR = 11;
        private static final int INDEX_MINUTE = 13;
        private static final int INDEX_SECOND = 15;
        private static final int COUNT = LIB_FORMAT_LENGTH / 2;

        private String weekday;
        private int[] part = new int[DATE_PARTS];

        private DateTime(String dateTimeText) {
            part[YEAR] = getDateParameter(dateTimeText, INDEX_YEAR, INDEX_MONTH, -1);
            if(part[YEAR] == -1) { // check for two digit **20
                String dateText = dateTimeText.substring(INDEX_YEAR, INDEX_MONTH).trim();
                if(!dateText.equals("****")) {
                    String numericText = dateText.replace("**", "20");
                    part[YEAR] = getDateParameter(numericText, 0, numericText.length(), -1);
                }
            }

            part[MONTH] = getDateParameter(dateTimeText, INDEX_MONTH, INDEX_DATE, -1);
            part[DAY] = getDateParameter(dateTimeText, INDEX_DATE, INDEX_DAY_NAME, -1);
            weekday = dateTimeText.substring(INDEX_DAY_NAME, INDEX_HOUR);

            part[HOUR] = getDateParameter(dateTimeText, INDEX_HOUR, INDEX_MINUTE, -1);
            part[MINUTE] = getDateParameter(dateTimeText, INDEX_MINUTE, INDEX_SECOND, -1);
        }

        private String getFormattedDate() {
            String formattedDate = getPadded(part[YEAR], INDEX_MONTH - INDEX_YEAR);
            formattedDate += getPadded(part[MONTH], INDEX_DATE - INDEX_MONTH);
            formattedDate += getPadded(part[DAY], INDEX_DAY_NAME - INDEX_DATE);

            formattedDate += weekday;
            formattedDate += getPadded(part[HOUR], INDEX_MINUTE - INDEX_HOUR);
            formattedDate += getPadded(part[MINUTE], INDEX_SECOND - INDEX_MINUTE);
            formattedDate += getPadded(0, COUNT - INDEX_SECOND);

            return formattedDate;
        }

        private String getPadded(int value, int length) {
            String textPadded = "";
            if (value != -1) {
                String format = "%0" + length + "d";
                textPadded = String.format(Locale.US, format, value);
            } else {
                for (int i = 0; i < length; i++) {
                    textPadded += "*";
                }
            }
            return textPadded;
        }

        private String getFormattedDate(String dateFormat, int daysTill) {
            Calendar calendar = Calendar.getInstance();
            calendar.set(part[YEAR], part[MONTH] - 1, part[DAY], part[HOUR], part[MINUTE], 0);

            if (daysTill > 0) {
                calendar.add(Calendar.DATE, daysTill);
            }
            SimpleDateFormat dateFormatter = new SimpleDateFormat(dateFormat, Locale.US);
            return dateFormatter.format(calendar.getTime());
        }

        private void updateYearMonthDay(int year, int month, int day) {
            part[YEAR] = year;
            part[MONTH] = month;
            part[DAY] = day;
        }

        private int getDateParameter(String source, int start, int end, int defaultValue) {
            String dateText = source.substring(start, end).trim();
            try {
                if (!dateText.isEmpty()) {
                    return Integer.parseInt(dateText);
                }
            } catch (NumberFormatException exp) {
                // return default value is ok
            }
            return defaultValue;
        }

        private boolean isWeekdayNull() {
            return weekday.equals("***");
        }

        private void setWeekdayNull() {
            weekday = "***";
        }

        private int getWeekDayNumber() {
            final String[] weekdayName = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
            for (int i = 0; i < weekdayName.length; i++) {
                if (weekday.equals(weekdayName[i])) {
                    return i + 1;
                }
            }

            return -1;
        }
    }

    private final DateTime fromDate;
    private final DateTime toDate;
    private final String dateSeparator;

    /**
     * Constructor for the date-time range
     *
     * @param dateTimeRange
     *            date time range in the text format
     *            YYYYMMDDWWWHHMMSS-YYYYMMDDWWWHHMMSS
     * @param dateSeparator
     *            date separator
     * @param defaultCalendar
     *            default calendar values
     */
    /*package*/ DateTimeRange(String dateTimeRange, String dateSeparator) {
        this.dateSeparator = dateSeparator;
        String[] dates = dateTimeRange.split("\\" + dateSeparator);
        fromDate = new DateTime(dates[0]);
        toDate = new DateTime(dates[1]);
    }

    /**
     * Checks if the day is same
     *
     * @return true if same, false otherwise
     */
    public boolean isDaySame() {
        return (fromDate.part[DateTime.YEAR] == toDate.part[DateTime.YEAR]
                && fromDate.part[DateTime.MONTH] == toDate.part[DateTime.MONTH] && fromDate.part[DateTime.DAY] == toDate.part[DateTime.DAY]) ? true
                : false;
    }

    /**
     * Checks if the all years are current
     *
     * @return true if same, false otherwise
     */
    public boolean areYearsCurrent() {
        Calendar calendar = Calendar.getInstance();
        int currentYear = calendar.get(Calendar.YEAR);
        return (toDate.part[DateTime.YEAR] == fromDate.part[DateTime.YEAR] && currentYear == toDate.part[DateTime.YEAR]);
    }

    /**
     * check if the range does not have hour-minute information
     *
     * @return true if range has hour-minute info, false otherwise
     */
    public boolean isHourMinutesNull() {
        return (toDate.part[DateTime.HOUR] <= 0 && toDate.part[DateTime.MINUTE] <= 0
                && fromDate.part[DateTime.HOUR] <= 0 && fromDate.part[DateTime.MINUTE] <= 0);
    }

    /**
     * check if the range does not have year-month-date information
     *
     * @return true if range has year-month-date info, false otherwise
     */
    public boolean isYearMonthDayNull() {
        return (toDate.part[DateTime.YEAR] <= 0 && toDate.part[DateTime.MONTH] <= 0
                && toDate.part[DateTime.DAY] <= 0 && fromDate.part[DateTime.YEAR] <= 0
                && fromDate.part[DateTime.MONTH] <= 0 && fromDate.part[DateTime.DAY] <= 0);
    }

    private void correctHourMinute() {
        if (fromDate.part[DateTime.MINUTE] == -1) {
            fromDate.part[DateTime.MINUTE] = toDate.part[DateTime.MINUTE];
            if (fromDate.part[DateTime.MINUTE] == -1) { //both -1 take current
                fromDate.part[DateTime.MINUTE] = 0;
                toDate.part[DateTime.MINUTE] = 0;
            }
        }
        if (toDate.part[DateTime.MINUTE] == -1) {// fromDate part[DateTime.MINUTE] cannot be -1 here
            toDate.part[DateTime.MINUTE] = fromDate.part[DateTime.MINUTE];
        }

        if (fromDate.part[DateTime.HOUR] == -1) {
            if (toDate.part[DateTime.HOUR] == -1) { //both -1 take whole day
                fromDate.part[DateTime.HOUR] = 0;
                fromDate.part[DateTime.MINUTE] = 0;

                toDate.part[DateTime.HOUR] = 0;
                toDate.part[DateTime.MINUTE] = 0;

            } else {
                fromDate.part[DateTime.HOUR] = toDate.part[DateTime.HOUR] - 1;
                if (fromDate.part[DateTime.HOUR] < 0) {
                    fromDate.part[DateTime.HOUR] = 0;
                    fromDate.part[DateTime.MINUTE] = 0;
                }
            }
        }
        if (toDate.part[DateTime.HOUR] == -1) {
            toDate.part[DateTime.HOUR] = fromDate.part[DateTime.HOUR] + 1;
            toDate.part[DateTime.MINUTE] = fromDate.part[DateTime.MINUTE];
            if (toDate.part[DateTime.HOUR] > 24) {
                toDate.part[DateTime.HOUR] = fromDate.part[DateTime.HOUR];// let the user correct this end case
            }
        }
    }

    private Calendar getCorrectedFromDate() {
        Calendar calendar = Calendar.getInstance();

        int fromYear = fromDate.part[DateTime.YEAR];
        int fromMonth = fromDate.part[DateTime.MONTH];
        int fromDay = fromDate.part[DateTime.DAY];
        int fromDayOffset = 0;

        if (fromYear == -1) {
            fromYear = (toDate.part[DateTime.YEAR] != -1) ? toDate.part[DateTime.YEAR] : calendar
                    .get(Calendar.YEAR);
        }
        if (fromMonth == -1) {
            fromMonth = (toDate.part[DateTime.MONTH] != -1) ? toDate.part[DateTime.MONTH]
                    : calendar.get(Calendar.MONTH) + 1;
        }

        if (fromDay == -1) {
            fromDay = calendar.get(Calendar.DATE);
            if (!fromDate.isWeekdayNull()) {
                int currentWeekDayNo = calendar.get(Calendar.DAY_OF_WEEK);
                int comingWeekDayNo = fromDate.getWeekDayNumber();
                fromDayOffset = (comingWeekDayNo - currentWeekDayNo);
                if (fromDayOffset < 0) {
                    fromDayOffset += DAYS_IN_WEEK;
                }

            } else if (toDate.part[DateTime.DAY] != -1) {
                fromDay = toDate.part[DateTime.DAY];

            } else if (!toDate.isWeekdayNull()) {
                int currentWeekDayNo = calendar.get(Calendar.DAY_OF_WEEK);
                int comingWeekDayNo = toDate.getWeekDayNumber();
                fromDayOffset = (comingWeekDayNo - currentWeekDayNo);
                if (fromDayOffset < 0) {
                    fromDayOffset += DAYS_IN_WEEK;
                }
            }
        }

        calendar.set(fromYear, fromMonth - 1, fromDay);
        calendar.add(Calendar.DATE, fromDayOffset);
        return calendar;
    }

    private Calendar getCorrectedToDate(Calendar fromCalendar) {
        Calendar calendar = Calendar.getInstance();

        int toYear = toDate.part[DateTime.YEAR];
        int toMonth = toDate.part[DateTime.MONTH];
        int toDay = toDate.part[DateTime.DAY];
        int toDayOffset = 0;

        if (toYear == -1) {
            toYear = fromCalendar.get(Calendar.YEAR);
        }
        if (toMonth == -1) {
            toMonth = fromCalendar.get(Calendar.MONTH) + 1;
        }
        if (toDay == -1) {
            if (!toDate.isWeekdayNull()) {
                int fromWeekDayNo = fromCalendar.get(Calendar.DAY_OF_WEEK);
                int comingWeekDayNo = toDate.getWeekDayNumber();
                toDayOffset = (comingWeekDayNo - fromWeekDayNo);
                if (toDayOffset < 0) {
                    toDayOffset += DAYS_IN_WEEK;
                }
            }
            toDay = fromCalendar.get(Calendar.DATE);
        }

        calendar.set(toYear, toMonth - 1, toDay);
        calendar.add(Calendar.DATE, toDayOffset);
        return calendar;
    }

    /**
     * corrects/verifies the associated date
     */
    public void verify() {
        Calendar fromCalendar = getCorrectedFromDate();
        Calendar toCalendar = getCorrectedToDate(fromCalendar);
        fromDate.updateYearMonthDay(fromCalendar.get(Calendar.YEAR),
                fromCalendar.get(Calendar.MONTH) + 1, fromCalendar.get(Calendar.DATE));
        toDate.updateYearMonthDay(toCalendar.get(Calendar.YEAR),
                toCalendar.get(Calendar.MONTH) + 1, toCalendar.get(Calendar.DATE));

        fromDate.setWeekdayNull();
        toDate.setWeekdayNull();
        correctHourMinute();

        // check if two dates are reported same
        if (isDaySame() && fromDate.part[DateTime.HOUR] == toDate.part[DateTime.HOUR]
                && fromDate.part[DateTime.MINUTE] == toDate.part[DateTime.MINUTE]) {
            if (fromDate.part[DateTime.HOUR] > 0) {
                fromDate.part[DateTime.HOUR]--;
            }
        }

        orderDatePart(DateTime.YEAR);
        if (fromDate.part[DateTime.YEAR] == toDate.part[DateTime.YEAR]) {
        orderDatePart(DateTime.MONTH);
            if (fromDate.part[DateTime.MONTH] == toDate.part[DateTime.MONTH]) {
        orderDatePart(DateTime.DAY);
                if (fromDate.part[DateTime.DAY] == toDate.part[DateTime.DAY]) {
        orderHourMinute();
    }
            }
        }
    }

    /**
     * Merges date range specified in argument to the current one
     *
     * @param dateTimeRange
     *            date time range to be merged
     */
    public boolean merge(DateTimeRange dateTimeRange) {
        if (isMergable(dateTimeRange)) {
            mergeDateParts(dateTimeRange, DateTime.YEAR);
            mergeDateParts(dateTimeRange, DateTime.MONTH);
            mergeDateParts(dateTimeRange, DateTime.DAY);

            mergeDateParts(dateTimeRange, DateTime.HOUR);
            mergeDateParts(dateTimeRange, DateTime.MINUTE);

            mergeWeekDay(dateTimeRange);
            return true;
        }
        return false;
    }

    private boolean isMergable(DateTimeRange dateTimeRange) {
        int total = 0;
        for (int i = 0; i < DateTime.DATE_PARTS; i++) {
            total = 0;
            if (dateTimeRange.fromDate.part[i] != -1) {
                total++;
            }
            if (dateTimeRange.toDate.part[i] != -1) {
                total++;
            }
            if (fromDate.part[i] != -1) {
                total++;
            }
            if (toDate.part[i] != -1) {
                total++;
            }
            if (total > 2) {
                return false;
            }
        }

        total = 0;
        if (!dateTimeRange.fromDate.isWeekdayNull()) {
            total++;
        }
        if (!dateTimeRange.toDate.isWeekdayNull()) {
            total++;
        }
        if (!fromDate.isWeekdayNull()) {
            total++;
        }
        if (!toDate.isWeekdayNull()) {
            total++;
        }
        return (total > 2) ? false : true;
    }

    private void orderDatePart(int type) {
        if (fromDate.part[type] == -1 || toDate.part[type] == -1) {
            return;
        }
        if (fromDate.part[type] > toDate.part[type]) {
            int temp = fromDate.part[type];
            fromDate.part[type] = toDate.part[type];
            toDate.part[type] = temp;
        }
    }

    private void orderHourMinute() {
        if (fromDate.part[DateTime.HOUR] == -1 || toDate.part[DateTime.HOUR] == -1) {
            return;
        }

        if (fromDate.part[DateTime.HOUR] > toDate.part[DateTime.HOUR]) {
            int tempHour = fromDate.part[DateTime.HOUR];
            int tempMinute = fromDate.part[DateTime.MINUTE];

            fromDate.part[DateTime.HOUR] = toDate.part[DateTime.HOUR];
            fromDate.part[DateTime.MINUTE] = toDate.part[DateTime.MINUTE];

            toDate.part[DateTime.HOUR] = tempHour;
            toDate.part[DateTime.MINUTE] = tempMinute;

        } else if (fromDate.part[DateTime.HOUR] == toDate.part[DateTime.HOUR]
                && fromDate.part[DateTime.MINUTE] > toDate.part[DateTime.MINUTE]) {
            int tempMinute = fromDate.part[DateTime.MINUTE];
            fromDate.part[DateTime.MINUTE] = toDate.part[DateTime.MINUTE];
            toDate.part[DateTime.MINUTE] = tempMinute;
        }
    }

    private void mergeDateParts(DateTimeRange dateTimeRange, int type) {
        if (dateTimeRange.fromDate.part[type] != -1) {
            if (fromDate.part[type] == -1) {
                fromDate.part[type] = dateTimeRange.fromDate.part[type];

            } else if (toDate.part[type] == -1) {
                toDate.part[type] = dateTimeRange.fromDate.part[type];
            }
        }
        if (dateTimeRange.toDate.part[type] != -1) {
            if (fromDate.part[type] == -1) {
                fromDate.part[type] = dateTimeRange.toDate.part[type];

            } else {
                toDate.part[type] = dateTimeRange.toDate.part[type];
            }
        }
    }

    private void mergeWeekDay(DateTimeRange dateTimeRange) {
        if (!dateTimeRange.fromDate.isWeekdayNull()) {
            if (fromDate.isWeekdayNull()) {
                fromDate.weekday = dateTimeRange.fromDate.weekday;

            } else if (toDate.isWeekdayNull()) {
                toDate.weekday = dateTimeRange.fromDate.weekday;
            }
        }
        if (!dateTimeRange.toDate.isWeekdayNull()) {
            if (fromDate.isWeekdayNull()) {
                fromDate.weekday = dateTimeRange.toDate.weekday;

            } else {
                toDate.weekday = dateTimeRange.toDate.weekday;
            }
        }
    }

    /**
     * Gets the date-time-range string in OCR defined format
     *
     * @return date-time-range string in OCR defined format
     */
    public String toSTADateTimeText() {
        return fromDate.getFormattedDate() + dateSeparator + toDate.getFormattedDate();
    }

    /**
     * gets the "from" date-time string in specified date format
     *
     * @param dateFormat
     *            date format
     * @return date-time string
     */
    /*package*/ String getFromFormattedDate(String dateFormat) {
        return fromDate.getFormattedDate(dateFormat, 0);
    }

    /**
     * gets the "from" date-time string in specified date format
     *
     * @param dateFormat
     *            date format
     * @return date-time string
     */
    /* package */String getFromFormattedDate(String dateFormat, int daysTill) {
        return fromDate.getFormattedDate(dateFormat, daysTill);
    }

    /**
     * gets the "to" date-time string in specified date format
     *
     * @param dateFormat
     *            date format
     * @return date-time string
     */
    /*package*/ String getToFormattedDate(String dateFormat) {
        return toDate.getFormattedDate(dateFormat, 0);
    }

    /**
     * gets the from days difference.
     *
     * @param dateTimeRange
     *            second date range
     * @return difference in days
     */
    /* package */int getFromDayDiff(DateTimeRange dateTimeRange) {
        if (!fromDate.isWeekdayNull() && !dateTimeRange.fromDate.isWeekdayNull()) {
            return (fromDate.getWeekDayNumber() - dateTimeRange.fromDate.getWeekDayNumber());
        }
        return -1;
    }

    /**
     * Checks if the days are of coming week
     *
     * @param tillDays
     *            days to be added to the date when checking
     * @return true if the days are of coming week, false otherwise
     */
    /* package */boolean areDaysOfComingWeek(int tillDays) {
        Calendar calendar = Calendar.getInstance();
        int currentMonth = calendar.get(Calendar.MONTH) + 1;
        int currentDay = calendar.get(Calendar.DAY_OF_MONTH);

        calendar.set(fromDate.part[DateTime.YEAR], fromDate.part[DateTime.MONTH] - 1,
                fromDate.part[DateTime.DAY]);
        int fromMonth = calendar.get(Calendar.MONTH) + 1;
        int fromDay = calendar.get(Calendar.DAY_OF_MONTH);

        if (fromMonth == currentMonth && fromDay >= currentDay) {
            int dayDiff = fromDay - currentDay;
            if (dayDiff < 7) {
                return true;
            }
        }

        return false;
    }

    /**
     * gets the from day
     *
     * @param tillDays
     *            days to be added to thef rom date
     * @return days
     */
    /* package */int getFromDay(int tillDays) {
        if (tillDays > 0) {
            Calendar calendar = Calendar.getInstance();
            calendar.set(fromDate.part[DateTime.YEAR], fromDate.part[DateTime.MONTH] - 1,
                    fromDate.part[DateTime.DAY]);

            calendar.add(Calendar.DATE, tillDays);
            return calendar.get(Calendar.DATE);
        }
        return fromDate.part[DateTime.DAY];
    }

    /**
     * check if the date null
     *
     * @return true if the date is null, false otherwise
     */
    public boolean isDateNull() {
        return (fromDate.part[DateTime.DAY] == -1);
    }
}
