/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.fr;

import android.graphics.Point;
import android.graphics.Rect;
import android.os.Parcel;
import android.os.Parcelable;

import com.qualcomm.qti.facerecognition.FaceInfo;
import com.qualcomm.qti.facerecognition.FaceLandmark;
import com.qualcomm.qti.facerecognition.FaceRecognitionResult;

import java.io.Serializable;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.Objects;

/**
 * This class represents a face, as detected (and optionally recognized) by the SCVE facial
 * recognition engine
 */
public class Face implements Parcelable {

    /**
     * This class represents the landmarks of an eye
     */
    public static final class Eye implements Parcelable {
        public Point center = new Point();
        public Point innerCorner = new Point();
        public Point outerCorner = new Point();

        public Eye() {}

        @Override
        public int describeContents() {
            return 0;
        }

        @Override
        public void writeToParcel(Parcel dest, int flags) {
            dest.writeParcelable(center, flags);
            dest.writeParcelable(innerCorner, flags);
            dest.writeParcelable(outerCorner, flags);
        }

        public static final Parcelable.Creator CREATOR = new Parcelable.Creator() {
            public Eye createFromParcel(Parcel in) {
                return new Eye(in);
            }

            public Eye[] newArray(int size) {
                return new Eye[size];
            }
        };

        public Eye(Parcel in) {
            center = in.readParcelable(Point.class.getClassLoader());
            innerCorner = in.readParcelable(Point.class.getClassLoader());
            outerCorner = in.readParcelable(Point.class.getClassLoader());
        }
    }

    /**
     * This class represents the landmarks of the mouth
     */
    public static final class Mouth implements Parcelable {
        public Point left = new Point();
        public Point right = new Point();
        public Point up = new Point();

        public Mouth() {}

        @Override
        public int describeContents() {
            return 0;
        }

        @Override
        public void writeToParcel(Parcel dest, int flags) {
            dest.writeParcelable(left, flags);
            dest.writeParcelable(right, flags);
            dest.writeParcelable(up, flags);
        }

        public static final Parcelable.Creator CREATOR = new Parcelable.Creator() {
            public Mouth createFromParcel(Parcel in) {
                return new Mouth(in);
            }

            public Mouth[] newArray(int size) {
                return new Mouth[size];
            }
        };

        public Mouth(Parcel in) {
            left = in.readParcelable(Point.class.getClassLoader());
            right = in.readParcelable(Point.class.getClassLoader());
            up = in.readParcelable(Point.class.getClassLoader());
        }
    }

    /**
     * This class represents the landmarks of the nose
     */
    public static final class Nose implements Parcelable {
        public Point left = new Point();
        public Point right = new Point();

        public Nose() {}

        @Override
        public int describeContents() {
            return 0;
        }

        @Override
        public void writeToParcel(Parcel dest, int flags) {
            dest.writeParcelable(left, flags);
            dest.writeParcelable(right, flags);
        }

        public static final Parcelable.Creator CREATOR = new Parcelable.Creator() {
            public Nose createFromParcel(Parcel in) {
                return new Nose(in);
            }

            public Nose[] newArray(int size) {
                return new Nose[size];
            }
        };

        public Nose(Parcel in) {
            left = in.readParcelable(Point.class.getClassLoader());
            right = in.readParcelable(Point.class.getClassLoader());
        }
    }

    /**
     * This class represents the results of facial recognition, as compared to the database of
     * enrolled faces
     */
    public static final class RecognitionResult implements Parcelable {
        /**
         * {@link FaceRecognitionResult}s with scores above this value are considered high-precision
         * recognitions
         */
        public static final float SCORE_THRESHOLD_HIGH = 175f;

        /**
         * {@link FaceRecognitionResult}s with scores above this value are considered medium-precision
         * recognitions
         */
        public static final float SCORE_THRESHOLD_MEDIUM = 150f;

        /**
         * {@link FaceRecognitionResult}s with scores above this value are considered low-precision
         * recognitions. Any scores below this threshold are considered unrecognized.
         */
        public static final float SCORE_THRESHOLD_LOW = 150f;

        public final float score;
        public final String persistentId;

        public RecognitionResult(float score, String persistentId) {
            this.score = score;
            this.persistentId = persistentId;
        }

        @Override
        public int describeContents() {
            return 0;
        }

        @Override
        public void writeToParcel(Parcel dest, int flags) {
            dest.writeFloat(score);
            dest.writeString(persistentId);
        }

        public static final Parcelable.Creator CREATOR = new Parcelable.Creator() {
            public RecognitionResult createFromParcel(Parcel in) {
                return new RecognitionResult(in);
            }

            public RecognitionResult[] newArray(int size) {
                return new RecognitionResult[size];
            }
        };

        public RecognitionResult(Parcel in) {
            score = in.readFloat();
            persistentId = in.readString();
        }
    }

    public int id;
    public Rect bounds = new Rect();
    public Eye leftEye = new Eye();
    public Eye rightEye = new Eye();
    public Nose nose = new Nose();
    public Mouth mouth = new Mouth();

    public RecognitionResult[] recognitionResults;

    /**
     * Construct a new {@link Face}
     * @param faceInfo a {@link FaceInfo} from which to construct this {@link Face}
     * @param recognitionResults an optional array of facial recognition results for this
     *                           {@link Face}
     * @param numRecognitionResults the number of {@link FaceRecognitionResult}s from
     *                              recognitionResults to process
     * @throws NullPointerException if faceInfo is null
     * @throws IllegalArgumentException if recognitionResults is non-null and numRecognitionResults
     *                                  is > recognitionResults.length
     */
    /*package*/ Face(FaceInfo faceInfo, FaceRecognitionResult[] recognitionResults,
                     int numRecognitionResults) {
        Objects.requireNonNull(faceInfo, "faceInfo may not be null");

        if (recognitionResults != null && numRecognitionResults > recognitionResults.length) {
            throw new IllegalArgumentException("numRecognitionResults must be <= " +
                    "recognitionResults.length");
        }

        id = faceInfo.id;

        bounds.set(faceInfo.leftTopX, faceInfo.leftTopY, faceInfo.rightBottomX,
                faceInfo.rightBottomY);

        leftEye.center.set(faceInfo.faceLandmark.leftEyeX, faceInfo.faceLandmark.leftEyeY);
        leftEye.innerCorner.set(faceInfo.faceLandmark.leftEyeInnerCornerX,
                faceInfo.faceLandmark.leftEyeInnerCornerY);
        leftEye.outerCorner.set(faceInfo.faceLandmark.leftEyeOuterCornerX,
                faceInfo.faceLandmark.leftEyeOuterCornerY);

        rightEye.center.set(faceInfo.faceLandmark.rightEyeX, faceInfo.faceLandmark.rightEyeY);
        rightEye.innerCorner.set(faceInfo.faceLandmark.rightEyeInnerCornerX,
                faceInfo.faceLandmark.rightEyeInnerCornerY);
        rightEye.outerCorner.set(faceInfo.faceLandmark.rightEyeOuterCornerX,
                faceInfo.faceLandmark.rightEyeOuterCornerY);

        nose.left.set(faceInfo.faceLandmark.noseLeftX, faceInfo.faceLandmark.noseLeftY);
        nose.right.set(faceInfo.faceLandmark.noseRightX, faceInfo.faceLandmark.noseRightY);

        mouth.left.set(faceInfo.faceLandmark.mouthLeftX, faceInfo.faceLandmark.mouthLeftY);
        mouth.right.set(faceInfo.faceLandmark.mouthRightX, faceInfo.faceLandmark.mouthRightY);
        mouth.up.set(faceInfo.faceLandmark.mouthUpX, faceInfo.faceLandmark.mouthUpY);

        if (recognitionResults != null && numRecognitionResults > 0) {
            this.recognitionResults = new RecognitionResult[recognitionResults.length];
            for (int i = 0; i < recognitionResults.length; i++) {
                this.recognitionResults[i] = new RecognitionResult(recognitionResults[i].matchScore,
                        new String(recognitionResults[i].pid, StandardCharsets.UTF_8));
            }
        } else {
            this.recognitionResults = new RecognitionResult[0];
        }
    }

    /**
     * Create a {@link FaceInfo} from this {@link Face}
     * @return a new {@link FaceInfo} representing the landmarks of this {@link Face}
     */
    /*package*/ FaceInfo createFaceInfo() {
        FaceLandmark faceLandmark = new FaceLandmark();
        faceLandmark.setLandmarkInfos(leftEye.center.x, leftEye.center.y,
                rightEye.center.x, rightEye.center.y,
                leftEye.innerCorner.x, leftEye.innerCorner.y,
                leftEye.outerCorner.x, leftEye.outerCorner.y,
                rightEye.innerCorner.x, rightEye.innerCorner.y,
                rightEye.outerCorner.x, rightEye.outerCorner.y,
                mouth.left.x, mouth.left.y,
                mouth.right.x, mouth.right.y,
                nose.left.x, nose.left.y,
                nose.right.x, nose.right.y,
                mouth.up.x, mouth.up.y);

        FaceInfo faceInfo = new FaceInfo();
        faceInfo.setFaceInfos(id,
                bounds.left, bounds.top,
                bounds.right, bounds.top,
                bounds.left, bounds.bottom,
                bounds.right, bounds.bottom,
                faceLandmark);

        return faceInfo;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(id);
        dest.writeParcelable(bounds, flags);
        dest.writeParcelable(leftEye, flags);
        dest.writeParcelable(rightEye, flags);
        dest.writeParcelable(nose, flags);
        dest.writeParcelable(mouth, flags);
        dest.writeParcelableArray(recognitionResults, flags);
    }

    public static final Parcelable.Creator CREATOR = new Parcelable.Creator() {
        public Face createFromParcel(Parcel in) {
            return new Face(in);
        }

        public Face[] newArray(int size) {
            return new Face[size];
        }
    };

    // For parceling
    public Face(Parcel in) {
        id = in.readInt();
        bounds = in.readParcelable(Rect.class.getClassLoader());
        leftEye = in.readParcelable(Eye.class.getClassLoader());
        rightEye = in.readParcelable(Eye.class.getClassLoader());
        nose = in.readParcelable(Nose.class.getClassLoader());
        mouth = in.readParcelable(Mouth.class.getClassLoader());

        Parcelable[] parcelableArray = in.readParcelableArray
                (RecognitionResult.class.getClassLoader());
        if (parcelableArray != null) {
            recognitionResults = Arrays.copyOf(parcelableArray, parcelableArray.length,
                    RecognitionResult[].class);
        }
    }
}
