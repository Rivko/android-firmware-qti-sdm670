/*
 * Copyright (C) 2015, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * Contributed by: Giesecke & Devrient GmbH.
 */

package org.simalliance.openmobileapi.service;

import android.os.Parcel;
import android.os.Parcelable;

/**
 * Created by sevilser on 23/12/14.
 */
public class OpenLogicalChannelResponse implements Parcelable {

    private int mChannelNumber;
    private byte[] mSelectResponse;

    public static final Parcelable.Creator<OpenLogicalChannelResponse> CREATOR = new Parcelable.Creator<OpenLogicalChannelResponse>() {
        public OpenLogicalChannelResponse createFromParcel(Parcel in) {
            return new OpenLogicalChannelResponse(in);
        }

        public OpenLogicalChannelResponse[] newArray(int size) {
            return new OpenLogicalChannelResponse[size];
        }
    };

    public OpenLogicalChannelResponse(int channelNumber, byte[] selectResponse) {
        mChannelNumber = channelNumber;
        mSelectResponse = selectResponse;
    }

    private OpenLogicalChannelResponse(Parcel in) {
        mChannelNumber = in.readInt();
        mSelectResponse = in.createByteArray();
    }

    public int getChannel() {
        return mChannelNumber;
    }

    public byte[] getSelectResponse() {
        return mSelectResponse;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        out.writeInt(mChannelNumber);
        out.writeByteArray(mSelectResponse);
    }
}
