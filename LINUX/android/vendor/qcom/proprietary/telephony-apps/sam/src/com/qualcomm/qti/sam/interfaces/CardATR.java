/******************************************************************************
 * @file    CardATR.java
 *
 * @brief   Card ATR parcel definition
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/

package com.qualcomm.qti.sam.interfaces;

import android.util.Log;
import android.os.Parcelable;
import android.os.Parcel;

public class CardATR implements Parcelable {
    private int mSlotId;
    private String mCardATR;

    public CardATR() {
    }

    public void setSlotId(int slotId) {
        mSlotId = slotId;
    }

    public int getSlotId() {
        return mSlotId;
    }

    public void setCardATR(String cardATR) {
        mCardATR = cardATR;
    }

    public String getCardATR() {
        return mCardATR;
    }

    public CardATR (Parcel in){
        mSlotId = in.readInt();
        mCardATR = in.readString();
    }

    @Override
    public int describeContents() {
        return 0;
    }

    public void readFromParcel(Parcel in){
        mSlotId = in.readInt();
        mCardATR = in.readString();
    }

    @Override
    public void writeToParcel(Parcel dest, int flags){
        dest.writeInt(mSlotId);
        dest.writeString(mCardATR);
    }

    public static final Parcelable.Creator<CardATR> CREATOR =
                                        new Parcelable.Creator<CardATR>() {

        public CardATR createFromParcel(Parcel in) {
            return new CardATR(in);
        }

        public CardATR[] newArray(int size) {
            return new CardATR[size];
        }
    };
}

