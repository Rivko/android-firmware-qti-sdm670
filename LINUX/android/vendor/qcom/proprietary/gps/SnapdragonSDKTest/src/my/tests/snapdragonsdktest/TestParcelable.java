/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package my.tests.snapdragonsdktest;

import android.os.Parcel;
import android.os.Parcelable;
import android.os.BadParcelableException;
import android.util.Log;


public class TestParcelable implements Parcelable {
    private static String TAG = "TestParcelable";
    public int mitest;
    public String mstest;

    public TestParcelable(int i, String s) {
        mitest = i;
        mstest = s;
    }

    public TestParcelable(Parcel source) {
        mitest = source.readInt();
        mstest = source.readString();
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(mitest);
        dest.writeString(mstest);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    public static final Parcelable.Creator<TestParcelable> CREATOR =
            new Parcelable.Creator<TestParcelable>() {
        @Override
        public TestParcelable createFromParcel(Parcel source) {
             return new TestParcelable(source);
        }

        @Override
        public TestParcelable[] newArray(int size) {
            return new TestParcelable[size];
        }
    };
}

