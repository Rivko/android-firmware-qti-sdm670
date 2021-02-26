/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.qti.poweroffalarm;

import org.json.JSONException;
import org.json.JSONObject;

public class PowerOffAlarmPersistData {
    public long snoozeTime;

    public long alarmTime;

    public int alarmStatus;

    private static final String PERSIST_ALARM_DATA_FILE = "/persist/alarm/data";

    private static final String SNOOZE_TIME = "snooze_time";

    private static final String ALARM_TIME = "alarm_time";

    private static final String ALARM_STATUS = "alarm_status";

    public PowerOffAlarmPersistData() {
        try {
            String dataString  = PowerOffAlarmUtils.readPowerOffAlarmFile(
                    PERSIST_ALARM_DATA_FILE);

            if (dataString != null) {
                JSONObject data = new JSONObject(dataString);
                this.snoozeTime = data.getLong(SNOOZE_TIME);
                this.alarmTime = data.getLong(ALARM_TIME);
                this.alarmStatus = data.getInt(ALARM_STATUS);
            } else {
                this.snoozeTime = PowerOffAlarmUtils.DEFAULT_ALARM_TIME;
                this.alarmTime = PowerOffAlarmUtils.DEFAULT_ALARM_TIME;
                this.alarmStatus = PowerOffAlarmUtils.ALARM_STATUS_NONE;
            }

        } catch (Exception e){
            this.snoozeTime = PowerOffAlarmUtils.DEFAULT_ALARM_TIME;
            this.alarmTime = PowerOffAlarmUtils.DEFAULT_ALARM_TIME;
            this.alarmStatus = PowerOffAlarmUtils.ALARM_STATUS_NONE;
            e.printStackTrace();
        }

    }

    public long getSnoozeTime() {
        return this.snoozeTime;
    }

    public void setSnoozeTime(long time) {
        this.snoozeTime = time;
    }

    public long getAlarmTime() {
        return this.alarmTime;
    }

    public void setAlarmTime(long time) {
        this.alarmTime = time;
    }

    public int getAlarmStatus() {
        return this.alarmStatus;
    }

    public void setAlarmStatus(int status) {
        this.alarmStatus = status;
    }

    public void writeDataToFile() {
       PowerOffAlarmUtils.writePowerOffAlarmFile(PERSIST_ALARM_DATA_FILE,
               toString());
    }

    @Override
    public String toString() {
        String dataString = null;
        try {
            JSONObject json = new JSONObject();
            json.put(SNOOZE_TIME, this.snoozeTime);
            json.put(ALARM_TIME, this.alarmTime);
            json.put(ALARM_STATUS, this.alarmStatus);
            dataString = json.toString();
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return dataString;
    }
}
