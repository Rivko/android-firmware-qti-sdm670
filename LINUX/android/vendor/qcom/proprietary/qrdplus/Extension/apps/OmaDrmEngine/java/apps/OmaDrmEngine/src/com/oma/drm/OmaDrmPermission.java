/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

package com.oma.drm;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

import android.content.Context;
import android.os.Parcel;
import android.os.Parcelable;

public class OmaDrmPermission implements Parcelable {

    private static int PLAY = 1;
    private static int DISPLAY = 7;
    public int mType = -1;
    public int mCount = -1;
    public long mStartDate = -1l;
    public long mEnddate = -1l;
    public long mInterval = -1l;

    OmaDrmPermission(int type, int count, long startdate, long enddate,
            long interval) {
        mType = type;
        mCount = count;
        mStartDate = startdate;
        mEnddate = enddate;
        mInterval = interval;
    }

    public OmaDrmPermission(Parcel in) {
        mType = in.readInt();
        mCount = in.readInt();
        mStartDate = in.readLong();
        mEnddate = in.readLong();
        mInterval = in.readLong();
    }

    public static final Parcelable.Creator<OmaDrmPermission> CREATOR = new Parcelable.Creator<OmaDrmPermission>() {

        public OmaDrmPermission createFromParcel(Parcel in) {
            return new OmaDrmPermission(in);
        }

        public OmaDrmPermission[] newArray(int size) {
            return new OmaDrmPermission[size];
        }
    };

    public ArrayList<String> getPermissionAsList(Context context) {
        ArrayList<String> permissionList = new ArrayList<String>();
        if (mType > 0) {
            String type = "";
            if (mType == PLAY) {
                type = context.getResources().getString(R.string.drm_text_play);
            } else if (mType == DISPLAY) {
                type = context.getResources().getString(
                        R.string.drm_text_display);
            }
            permissionList.add(context.getResources().getString(
                    R.string.drm_text_license)
                    + " " + type);
        }
        if (mCount >= 0) {
            permissionList.add(context.getResources().getString(
                    R.string.drm_text_count)
                    + " " + mCount);
        }
        if (mStartDate > 0) {
            permissionList.add(context.getResources().getString(
                    R.string.drm_text_date_start)
                    + " " + parseMilliToDate(mStartDate));
        }
        if (mEnddate > 0) {
            permissionList.add(context.getResources().getString(
                    R.string.drm_text_date_end)
                    + " " + parseMilliToDate(mEnddate));
        }
        if (mInterval > 0) {
            permissionList.add(context.getResources().getString(
                    R.string.drm_text_date_end)
                    + " " + parseMilliToDate(mInterval));
        }
        return permissionList;
    }

    String parseMilliToDate(long timeInMilli) {
        Date date = new Date(timeInMilli * 1000L);
        DateFormat formatter = new SimpleDateFormat("dd/MM/yyyy hh:mm:ss");
        return formatter.format(date);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(mType);
        dest.writeInt(mCount);
        dest.writeLong(mStartDate);
        dest.writeLong(mEnddate);
        dest.writeLong(mInterval);
    }
}
