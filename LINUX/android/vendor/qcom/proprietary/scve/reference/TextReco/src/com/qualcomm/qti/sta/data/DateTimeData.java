/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */package com.qualcomm.qti.sta.data;

import java.util.ArrayList;

/**
 * Data class for handling dateTime data
 */
public class DateTimeData extends RecognizedData {
    private int MAX_REPEAT_LEN = 5;
    private int repeatDays = 0;
    private boolean mergeable = true;
    private ArrayList<String> dates = null;

    /**
     * Constructor for the class
     *
     * @param rect
     *            rectangular area where the associated text is recognized
     * @param dataId
     *            id of the data
     * @param type
     *            recognized data type
     * @param text
     *            recognized text
     * @param dates
     *            dates associated with data
     */
    public DateTimeData(RecognizedRect rect, int type, int dataId, String text, ArrayList<String> dates) {
        super(rect, type, dataId, text);

        this.dates = dates;
    }

    /**
     * gets the number of dates associated
     *
     * @return number of dates associated
     */
    public int getDateCount() {
        return dates.size();
    }

    /**
     * gets the date at index
     *
     * @param index
     *            index of the date
     * @return date
     */
    public String getDateAt(int index) {
        if (index >= 0 && index < dates.size()) {
            return dates.get(index);
        }
        return null;
    }

    /**
     * Creates the repeat information from the available data
     *
     * @param dateTextHandler
     *            date text handler
     */
    public void createRepeatInfo(DateTextHandler dateTextHandler) {
        if (dates.size() > 1) {
            String firstDate = dates.get(0);
            if (dateTextHandler.isDateNull(firstDate)) {
                mergeable = false;
                int repeatLength = dates.size() - 1;
                if (repeatLength > MAX_REPEAT_LEN) {
                    repeatLength = MAX_REPEAT_LEN;
                }
                repeatDays = repeatLength;
                dates.clear();
                dates.add(firstDate);
            }
        }
    }

    /**
     * Verifies/corrects the dates
     *
     * @param dateTextHandler
     *            date text handler
     */
    public void verify(DateTextHandler dateTextHandler) {
        for (int i = 0; i < dates.size(); i++) {
            String dateTime = dates.remove(i);
            DateTimeRange dateRange = dateTextHandler.parseRange(dateTime);
            dateRange.verify();
            dateTime = dateRange.toSTADateTimeText();
            dates.add(i, dateTime);
        }
    }

    /**
     * Sets the date time
     *
     * @param dateTime
     *            new date time
     */
    public void setDateTime(String dateTime) {
        dates.clear();
        dates.add(dateTime);
    }

    /**
     * Sets the dates
     *
     * @param mergedDates
     *            dates to be set
     */
    public void setDates(ArrayList<String> mergedDates) {
        dates = mergedDates;
    }

    /**
     * check if the datetime is merge-able
     *
     * @return true if mergeable, false otherwise
     */
    public boolean isMergable() {
        return mergeable;
    }

    /**
     * Gets the repeat information
     *
     * @return repeat information
     */
    public int getRepeatDays() {
        return repeatDays;
    }

    /**
     * gets the first datetime
     *
     * @return first datetime
     */
    public String getDateTime() {
        return dates.get(0);
    }
}
