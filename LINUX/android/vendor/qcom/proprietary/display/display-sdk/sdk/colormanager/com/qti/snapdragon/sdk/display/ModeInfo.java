/* ====================================================================
 * Copyright (c) 2014 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =====================================================================
 * @file ModeInfo.java
 *
 */

package com.qti.snapdragon.sdk.display;

import com.qti.snapdragon.sdk.display.ColorManager.DCM_DISPLAY_TYPE;
import com.qti.snapdragon.sdk.display.ColorManager.MODE_TYPE;

import android.os.Parcel;
import android.os.Parcelable;

/**
 * This class will be used to provide mode information like its id, name, type.
 */
public class ModeInfo implements Parcelable{
    private int id = -1;
    private String name = null;
    private MODE_TYPE modeType = MODE_TYPE.MODE_SYSTEM;

    /**
     * This constructor will create the instance with the required parameters
     * @param pId id of the mode. To be used for all mode operations.
     * @param pName name of the mode
     * @param pType indicates whether this mode is MODE_SYSTEM or MODE_USER
     */
    public ModeInfo(int pId, String pName, int pType){
        id = pId;
        name = pName;
        modeType = MODE_TYPE.values()[pType];
    }

    /**
     * This API returns the id of the mode
     * @return mode id
     */
    public int getId(){
        return id;
    }

    /**
     * This api returns the name of the mode
     * @return mode name
     */
    public String getName(){
        return name;
    }

    /**
     * This api returns the type of the mode
     * @return MODE_SYSTEM or MODE_USER
     */
    public MODE_TYPE getModeType(){
        return modeType;
    }

    /**
     * @y.exclude
     */
    @Override
    public int describeContents(){
        return 0;
    }

    /**
     * This method is not intended for public use
     */
    @Override
    public void writeToParcel(Parcel destParcel, int flags){
        destParcel.writeInt(id);
        destParcel.writeString(name);
        destParcel.writeInt(modeType.getValue());
    }

    /**
     *
     */
    public static final Parcelable.Creator<ModeInfo> CREATOR = new Parcelable.Creator<ModeInfo>() {
        @Override
        public ModeInfo createFromParcel(Parcel inParcel){
            int id = inParcel.readInt();
            String name = inParcel.readString();
            int modeType = inParcel.readInt();
            return new ModeInfo(id, name, modeType);
        }

        @Override
        public ModeInfo[] newArray(int size){
            return new ModeInfo[size];
        }
    };
}
