/******************************************************************************
 * @file    CardState.java
 * @brief   card state parcel definition
 *
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

public class CardState implements Parcelable {
    private int mSlotId;
    private int mCardState;

    public static final int CARD_STATE_ABSENT  = 0;
    public static final int CARD_STATE_PRESENT = 1;
    public static final int CARD_STATE_ERROR   = 2;

    public CardState() {
    }

    public void setSlotId(int slotId) {
        mSlotId = slotId;
    }

    public int getSlotId() {
        return mSlotId;
    }

    public void setCardState(int cardState) {
        mCardState = cardState;
    }

    public int getCardState() {
        return mCardState;
    }

    public CardState (Parcel in){
        mSlotId = in.readInt();
        mCardState = in.readInt();
    }

    @Override
    public int describeContents() {
        return 0;
    }

    public void readFromParcel(Parcel in){
        mSlotId = in.readInt();
        mCardState = in.readInt();
    }

    @Override
    public void writeToParcel(Parcel dest, int flags){
        dest.writeInt(mSlotId);
        dest.writeInt(mCardState);
    }

    public static final Parcelable.Creator<CardState> CREATOR =
                                        new Parcelable.Creator<CardState>() {

        public CardState createFromParcel(Parcel in) {
            return new CardState(in);
        }

        public CardState[] newArray(int size) {
            return new CardState[size];
        }
    };
}
