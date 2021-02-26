/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.MediaInfoGenerator;

import android.net.Uri;
import android.os.Parcel;
import android.os.Parcelable;

import java.util.ArrayList;

/**
 * Created by mithun on 9/8/15.
 */
public class MediaInfo implements Parcelable {

    /* {@link #contentUri} is the content:// style uri which is to be used to uniquely identify this video */
    public Uri contentUri;
    public long createdDate;
    public double latitude;
    public double longitude;
    public long duration;

    public MediaInfo() {

    }

    public MediaInfo(Parcel in) {
        String data[] = new String[5];

        in.readStringArray(data);
        this.contentUri = Uri.parse(data[0]);
        this.createdDate = Long.parseLong(data[1]);
        this.latitude = Double.parseDouble(data[2]);
        this.longitude = Double.parseDouble(data[3]);
        this.duration = Long.parseLong(data[4]);

    }

    public static final Creator<MediaInfo> CREATOR = new Creator<MediaInfo>() {
        @Override
        public MediaInfo createFromParcel(Parcel in) {
            return new MediaInfo(in);
        }

        @Override
        public MediaInfo[] newArray(int size) {
            return new MediaInfo[size];
        }
    };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeStringArray(new String[]{this.contentUri.toString(),
                Long.toString(this.createdDate),
                Double.toString(this.latitude),
                Double.toString(this.longitude),
                Long.toString(this.duration)});
    }

    public static ArrayList<Uri> getArrayListOfUris(ArrayList<MediaInfo> mediaInfoArrayList) {
        ArrayList<Uri> uris = new ArrayList<Uri>(mediaInfoArrayList.size());
        for (int i = 0; i < mediaInfoArrayList.size(); i++) {
            uris.add(mediaInfoArrayList.get(i).contentUri);
        }
        return uris;
    }
}
