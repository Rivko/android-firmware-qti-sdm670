/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.telephonyservice;

import java.util.Arrays;

import android.os.Parcel;
import android.os.Parcelable;

public class KsNafResponse implements Parcelable {

    private int mType;
    private byte[] mResponse;
    private String mBootstrapTransactionId;
    private String mLifetime;

    public int getType() {
        return mType;
    }

    public byte[] getResponse() {
        return mResponse;
    }

    public String getBootstrapTransactionId() {
        return mBootstrapTransactionId;
    }

    public String getLifetime() {
        return mLifetime;
    }

    protected KsNafResponse(int ksNafType, byte[] ksNafResponse,
            String bootstrapTransactionId, String ksLifetime)
    {
        mType = ksNafType;
        mResponse = ksNafResponse;
        mBootstrapTransactionId = bootstrapTransactionId;
        mLifetime = ksLifetime;
    }

    private KsNafResponse(Parcel in) {
        mType = in.readInt();
        mResponse = new byte[in.readInt()];
        in.readByteArray(mResponse);
        mBootstrapTransactionId = in.readString();
        mLifetime = in.readString();
    }

    public int describeContents() {
        return 0;
    }

    public void writeToParcel(Parcel out, int flags) {
        out.writeInt(mType);
        out.writeInt(mResponse.length);
        out.writeByteArray(mResponse);
        out.writeString(mBootstrapTransactionId);
        out.writeString(mLifetime);
    }

    public String toString() {
        return "KsNafResponse mType: " + mType
                + " mResponse: " + Arrays.toString(mResponse)
                + " mBootstrapTransactionId: " + mBootstrapTransactionId
                + " mLifetime: " + mLifetime;
    }

    public static final Parcelable.Creator<KsNafResponse> CREATOR =
                                                  new Parcelable.Creator<KsNafResponse>() {
        public KsNafResponse createFromParcel(Parcel in) {
            return new KsNafResponse(in);
        }

        public KsNafResponse[] newArray(int size) {
            return new KsNafResponse[size];
        }
    };

}
