/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.FaceRecognition.DetectedFaces;

import android.graphics.Bitmap;
import android.os.Parcel;
import android.os.Parcelable;

import qces.qualcomm.qti.com.videosummary.providers.FRQueryHelper;

/**
 * Created by mithun on 12/9/15.
 */
public class DetectedFaceModel implements Parcelable {

    public byte[] frDatasetId;//this will be null if unknown face
    public String name;
    public Bitmap photo;
    public FaceType faceType;

    private static final String TAG = DetectedFaceModel.class.getSimpleName();


    public enum FaceType {DEMO, UNKNOWN_TITLE, UNKNOWN, KNOWN_TITLE, KNOWN}

    public DetectedFaceModel(byte[] frDatasetId, String name, Bitmap photo, FaceType faceType) {
        this.frDatasetId = frDatasetId;
        this.name = name;
        this.photo = photo;
        this.faceType = faceType;
    }

    public DetectedFaceModel(Parcel in) {
        faceType = (FaceType) in.readValue(FaceType.class.getClassLoader());
        int frDatasetIdLength = in.readInt();
        if (frDatasetIdLength > 0) {
            frDatasetId = new byte[frDatasetIdLength];
            in.readByteArray(frDatasetId);
        } else {
            frDatasetId = null;
        }
        name = in.readString();
        int length = in.readInt();
        byte[] byteArray = new byte[length];
        in.readByteArray(byteArray);
        photo = FRQueryHelper.BitmapUtilities.getImage(byteArray);

    }

    public static final Creator<DetectedFaceModel> CREATOR = new Creator<DetectedFaceModel>() {
        @Override
        public DetectedFaceModel createFromParcel(Parcel source) {
            return new DetectedFaceModel(source);
        }

        @Override
        public DetectedFaceModel[] newArray(int size) {
            return new DetectedFaceModel[size];
        }
    };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeValue(faceType);
        if (frDatasetId == null) {
            dest.writeInt(0);//length of frDatasetId is 0
        } else {
            dest.writeInt(frDatasetId.length);
            dest.writeByteArray(frDatasetId);
        }
        dest.writeString(name);
        byte[] byteArray = FRQueryHelper.BitmapUtilities.getBytes(photo);
        dest.writeInt(byteArray.length);
        dest.writeByteArray(byteArray);

    }

}
