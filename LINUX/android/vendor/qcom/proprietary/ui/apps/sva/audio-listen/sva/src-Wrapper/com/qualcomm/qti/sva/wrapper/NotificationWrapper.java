/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.qti.sva.wrapper;

import android.app.Notification;
import android.app.PendingIntent;
import android.content.Context;

public class NotificationWrapper {
    private Notification mNotification;

    /**
     * Constructs a Notification object with the information needed to
     * have a status bar icon without the standard expanded view.
     *
     * @param icon          The resource id of the icon to put in the status bar.
     * @param tickerText    The text that flows by in the status bar when the notification first
     *                      activates.
     * @param when          The time to show in the time field.  In the System.currentTimeMillis
     *                      timebase.
     */
    public NotificationWrapper(int icon, CharSequence tickerText, long when) {
        mNotification = new Notification(icon, tickerText, when);
    }

    public Notification getNotificationInstance() {
        return mNotification;
    }

    public void setLatestEventInfo(Context context,
                                   CharSequence contentTitle,
                                   CharSequence contentText, PendingIntent contentIntent) {
        mNotification.setLatestEventInfo(context, contentTitle, contentText, contentIntent);
    }
}
