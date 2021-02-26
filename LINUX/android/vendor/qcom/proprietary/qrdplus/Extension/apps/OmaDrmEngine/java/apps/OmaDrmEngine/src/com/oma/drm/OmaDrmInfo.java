/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

package com.oma.drm;

import java.util.ArrayList;

import android.content.Context;
import android.os.Parcel;
import android.os.Parcelable;

public class OmaDrmInfo implements Parcelable {

    private static final String FWD_LCK_ID = "cid:fwlk";
    private static final String MIME_TYPE_PREFIX = "drm+container_based+";

    public String mMimeType;
    public String mUid;
    public long mFileSize = -1l;
    public OmaDrmPermission mPermission;
    public boolean mRightsPresent = false;

    OmaDrmInfo(String mimeType, String uId, long filesize,
            OmaDrmPermission permission) {
        mMimeType = mimeType;
        mUid = uId;
        mFileSize = filesize;
        setPermission(permission);
    }

    public OmaDrmInfo(Parcel in) {
        mMimeType = in.readString();
        mUid = in.readString();
        mFileSize = in.readLong();
        setPermission((OmaDrmPermission) in
                .readParcelable(OmaDrmPermission.class.getClassLoader()));
    }

    public static final Parcelable.Creator<OmaDrmInfo> CREATOR = new Parcelable.Creator<OmaDrmInfo>() {

        public OmaDrmInfo createFromParcel(Parcel in) {
            return new OmaDrmInfo(in);
        }

        public OmaDrmInfo[] newArray(int size) {
            return new OmaDrmInfo[size];
        }
    };

    void setPermission(OmaDrmPermission permission) {
        mPermission = permission;
    }

    public ArrayList<String> getInfoAsList(Context context) {
        ArrayList<String> infoList = new ArrayList<String>();
        if (isForwardLock()) {
            infoList.add(context.getResources().getString(
                    R.string.drm_text_license)
                    + context.getResources()
                            .getString(R.string.drm_text_fwd_lk));
            mRightsPresent = true;
        }
        if (mPermission.mType > -1) {
            infoList.addAll(mPermission.getPermissionAsList(context));
            mRightsPresent = true;
        }
        // List Separator, create one empty list item
        infoList.add("   ");
        if (mMimeType != null) {
            infoList.add(context.getResources().getString(
                    R.string.drm_text_mime_type)
                    + getActualMimeType(mMimeType));
        }
        return infoList;
    }

    public boolean rightsPresent() {
        return mRightsPresent;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(mMimeType);
        dest.writeString(mUid);
        dest.writeLong(mFileSize);
        dest.writeParcelable(mPermission, flags);
    }

    public boolean isForwardLock() {
        boolean forwardLock = false;
        if (mUid != null && mUid.contains(FWD_LCK_ID)) {
            forwardLock = true;
        }
        return forwardLock;
    }

    String getActualMimeType(String derivedMimeIype) {
        String actualMimeType = derivedMimeIype;
        if (derivedMimeIype.contains(MIME_TYPE_PREFIX)) {
            actualMimeType = derivedMimeIype.replace(MIME_TYPE_PREFIX, "")
                    .trim();
        }
        return actualMimeType;
    }
}
