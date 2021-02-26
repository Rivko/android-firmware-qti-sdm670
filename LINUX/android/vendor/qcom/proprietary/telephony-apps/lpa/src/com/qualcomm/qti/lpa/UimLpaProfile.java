/******************************************************************************
 * @file    UimLpaProfile.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/


package com.qualcomm.qti.lpa;

import android.os.Parcelable;
import android.os.Parcel;
import java.util.Arrays;


public class UimLpaProfile implements Parcelable {

    public final int UIM_LPA_ICON_TYPE_JPEG = 0;
    public final int UIM_LPA_ICON_TYPE_PNG = 1;

    public boolean  profileState;
    public boolean  nicknamePresent;
    public String   nickname;
    public boolean  profileNamePresent;
    public String   profileName;
    public byte[]   iccid;
    public boolean  spnNamePresent;
    public String   spnName;
    public int      iconType;
    public byte[]   icon;
    public int      profileClass;
    public int      profilePolicyMask;

    public UimLpaProfile() {
    }

    //decode from parcel
    public UimLpaProfile(Parcel in) {

        int iconLen = 0;
        int iccidLen = 0;

        //profileName
        this.profileNamePresent = (in.readInt() == 0) ? false : true;
        if (this.profileNamePresent) {
            this.profileName = in.readString();
        }

        //nickname
        this.nicknamePresent = (in.readInt() == 0) ? false : true;
        if (this.nicknamePresent) {
            this.nickname = in.readString();
        }

        //profileState
        this.profileState = (in.readInt() == 0) ? false : true;

        //iccid
        iccidLen = in.readInt();
        if (iccidLen > 0) {
            this.iccid = new byte[iccidLen];
            in.readByteArray(this.iccid);
        }

        //spnName
        this.spnNamePresent = (in.readInt() == 0) ? false : true;
        if (this.spnNamePresent) {
            this.spnName = in.readString();
        }

        //iconType
        this.iconType = in.readInt();

        //icon
        iconLen = in.readInt();
        if (iconLen > 0) {
            this.icon = new byte[iconLen];
            in.readByteArray(this.icon);
        }

        this.profileClass = in.readInt();

        this.profilePolicyMask = in.readInt();

  }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        //profileName
        if (this.profileName != null) {
            dest.writeInt(1);
            dest.writeString(this.profileName);
        } else {
            dest.writeInt(0);
        }

        //nickname
        if (this.nickname != null) {
            dest.writeInt(1);
            dest.writeString(this.nickname);
        } else {
            dest.writeInt(0);
        }

        //profileState
        dest.writeInt((this.profileState ? 1 : 0));

        //iccid
        if (this.iccid != null) {
            dest.writeInt(this.iccid.length);
            dest.writeByteArray(this.iccid);
        } else {
            dest.writeInt(0);
        }

        //spnName
        if (this.spnName != null) {
            dest.writeInt(1);
            dest.writeString(this.spnName);
        } else {
            dest.writeInt(0);
        }

        //iconType
        dest.writeInt(this.iconType);

        //icon
        if (this.icon != null) {
            dest.writeInt(this.icon.length);
            dest.writeByteArray(this.icon);
        } else {
            dest.writeInt(0);
        }
        dest.writeInt(this.profileClass);

        dest.writeInt(this.profilePolicyMask);

    }

    public static final Creator<UimLpaProfile> CREATOR =
                                      new Creator<UimLpaProfile>() {

        public UimLpaProfile createFromParcel(Parcel in) {
            return new UimLpaProfile(in);
        }

        public UimLpaProfile[] newArray(int size) {
            return new UimLpaProfile[size];
        }
    };

    public void setProfileState(boolean profileState) {
        this.profileState = profileState;
    }
     public boolean getProfileState() {
        return profileState;
    }

    public void setNickname(String nickname) {
        this.nickname = nickname;
    }

    public String getNickname() {
        return nickname;
    }

    public void setProfilename(String profileName) {
        this.profileName = profileName;
    }

    public String getProfileName() {
        return profileName;
    }

    public void setIccid(byte[] iccid) {
        if (iccid != null) {
            this.iccid = Arrays.copyOf(iccid, iccid.length);
        }
    }

    public byte[] geticcid() {
        return iccid;
    }

    public void setSpnName(String spnName) {
        this.spnName = spnName;
    }

    public String getSpnName() {
        return spnName;
    }

    public void setIconType(int iconType) {
        this.iconType = iconType;
    }

    public int getIconType() {
        return iconType;
    }

    public void setIcon(byte[] icon) {
        if (icon != null) {
            this.icon = Arrays.copyOf(icon, icon.length);
        }
    }

    public byte[] getIcon() {
        return icon;
    }

    public void setProfileClass(int profileClass) {
        this.profileClass = profileClass;
    }

    public int getProfileClass() {
        return this.profileClass;
    }

    public void setProfilePolicy(int profilePolicyMask) {
        this.profilePolicyMask = profilePolicyMask;
    }

    public int getProfilePolicy() {
        return this.profilePolicyMask;
    }

    @Override
    public String toString() {
        if (this.profileName != null) {
            return this.profileName.toString();
        } else {
            return new String("No name");
        }
    }

}

