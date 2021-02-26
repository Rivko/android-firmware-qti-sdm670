/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.highlights;

import android.content.Context;
import android.graphics.Bitmap;
import android.os.Parcel;
import android.os.Parcelable;

import java.util.ArrayList;
import java.util.List;

import qces.qualcomm.qti.com.videosummary.FaceRecognition.DetectedFaces.DetectedFaceModel;
import qces.qualcomm.qti.com.videosummary.MediaInfoGenerator.MediaInfo;
import qces.qualcomm.qti.com.videosummary.MediaInfoGenerator.TitleGenerator;
import qces.qualcomm.qti.com.videosummary.providers.QueryHelper;

/**
 * Created by mithun on 8/14/15.
 */
public class HighlightsModel implements Parcelable {

    public String videoFilePath;
    public String thumbImgFilePath;
    public Bitmap thumbImgBmap;
    public String title;
    public String subTitle;
    public long timeStampMillis;
    public CardType cardType;
    public SummaryType summaryType;

    //Will store multi speed in case of timelapse or length+theme for highlights
    //but since audio= THEME is common in both, we will use  Byte4(MSB) to store theme ID
    //TIMELAPSE => Speed storage spec:
    // LSB(Byte1) will have speed,
    //      Byte2 will have eventSpeed,
    // In Byte3,
    //      1st bit is Event Detection switch 0 - off, 1 - On
    //      if 1st bit ON then 2nd and 3rd bit are valid, where
    //          2nd bit : Motion 0-disabled, 1-enabled
    //          3rd bit : Faces  0-disabled, 1-enabled
    // MSB(Byte4) will have theme ID (valuse will be 0, 1,2,3,4..,

    //HIGHLIGHTS => Length & theme storage spec:
    // LSB(Byte1) will have length,
    // MSB(Byte4) will have theme ID (valuse will be 0, 1,2,3,4..,
    public int speedOrLength;
    public CreatingStatus status;

    public ArrayList<MediaInfo> compositonMediaList;

    public List<byte[]> faceIdList;
    public List<DetectedFaceModel> starringList;

    public enum CardType {DEMO, CREATING, SAVED_TO_GALLERY, SUMMARY, ERROR}

    public enum SummaryType {HIGHLIGHTS, TIMELAPSE}

    private static final String TAG = HighlightsModel.class.getSimpleName();

    /**
     * To keep track of which item in the recycler view is being swiped, if so dont do progress update on that
     */
    public static int swipingPos = -1;

    public HighlightsModel(CardType cardType, SummaryType summaryType,
                           String videoFilePath, String thumbImgFilePath,
                           long timeStampMillis,
                           String title, String subTitle,
                           ArrayList<MediaInfo> compositonMediaList, int speedOrLength,
                           List<byte[]> faceIdList) {
        this.cardType = cardType;
        this.summaryType = summaryType;
        this.videoFilePath = videoFilePath;
        this.thumbImgFilePath = thumbImgFilePath;
        this.thumbImgBmap = null;//do not populate this bitmap here, as this object wil be passed around diff activities and this will consume too much memory
        this.timeStampMillis = timeStampMillis;
        this.title = title;
        this.subTitle = subTitle;
        this.compositonMediaList = compositonMediaList;
        this.speedOrLength = speedOrLength;
        if (cardType == CardType.CREATING || cardType == CardType.ERROR) {
            this.status = new CreatingStatus();
        }
        this.faceIdList = faceIdList;
        this.starringList = null;//do not populate this list here, as this object wil be passed around diff activities and this will consume too much memory


    }


    public static final Creator<HighlightsModel> CREATOR = new Creator<HighlightsModel>() {
        @Override
        public HighlightsModel createFromParcel(Parcel source) {
            return new HighlightsModel(source);
        }

        @Override
        public HighlightsModel[] newArray(int size) {
            return new HighlightsModel[size];
        }
    };

    @Override
    public int describeContents() {
        return 0;
    }

    public HighlightsModel(Parcel in) {
        videoFilePath = in.readString();
        thumbImgFilePath = in.readString();
        title = in.readString();
        subTitle = in.readString();
        timeStampMillis = in.readLong();
        cardType = (CardType) in.readValue(CardType.class.getClassLoader());
        summaryType = (SummaryType) in.readValue(SummaryType.class.getClassLoader());
        speedOrLength = in.readInt();
        faceIdList = QueryHelper.getFaceIdsAsArrayListOfByteArrayFromString(in.readString());
        if (in.readByte() == 0x01) {
            compositonMediaList = new ArrayList<MediaInfo>();
            in.readList(compositonMediaList, MediaInfo.class.getClassLoader());
        } else {
            compositonMediaList = null;
        }
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(videoFilePath);
        dest.writeString(thumbImgFilePath);
        dest.writeString(title);
        dest.writeString(subTitle);
        dest.writeLong(timeStampMillis);
        dest.writeValue(cardType);
        dest.writeValue(summaryType);
        dest.writeInt(speedOrLength);
        dest.writeString(QueryHelper.getConcatenatedFaceIdsAsString(faceIdList));
        if (compositonMediaList == null) {
            dest.writeByte((byte) (0x00));
        } else {
            dest.writeByte((byte) (0x01));
            dest.writeList(compositonMediaList);
        }
    }


    public void updateTitle(Context context) {
        TitleGenerator titleGenerator = new TitleGenerator(context, compositonMediaList, summaryType);
        title = titleGenerator.getTitle();
        subTitle = titleGenerator.getSubTitle();
    }

    /**
     * This method will update the HiglightModel for faceIdList field and also will update the db table
     *
     * @param context
     * @param faceIdList
     */
    public void updateFaceIdsForStarring(Context context, List<byte[]> faceIdList) {
        this.faceIdList = faceIdList;
        QueryHelper.updateHighlights(context, videoFilePath, faceIdList);
    }

    //TIMELAPSE => Speed storage spec:
    // LSB(Byte1) will have speed,
    //      Byte2 will have eventSpeed,
    // In Byte3,
    //      1st bit is Event Detection switch 0 - off, 1 - On
    //      if 1st bit ON then 2nd and 3rd bit are valid, where
    //          2nd bit : Motion 0-disabled, 1-enabled
    //          3rd bit : Faces  0-disabled, 1-enabled
    //Highlights/Timelapse, Theme is common and will be stored in Byte4, will have theme ID (valuse will be 0, 1,2,3,4..,)
    public static int updateSpeed(int speedOrLength, int speed) {
        if (speed > 16) {
            throw new IndexOutOfBoundsException("speed(" + speed + ") is > 16");
        }
        return (speedOrLength & 0xFFFFFF00) | (speed & 0xFF);
    }

    public int getSpeed() {
        return (speedOrLength & 0xff);
    }

    public static int updateEventSpeed(int speedOrLength, int eventSpeed) {
        if (eventSpeed > 16) {
            throw new IndexOutOfBoundsException("eventSpeed(" + eventSpeed + ") is > 16");
        }
        return (speedOrLength & 0xFFFF00FF) | ((eventSpeed & 0xFF) << 8);
    }

    public int getEventSpeed() {
        return ((speedOrLength >> 8) & 0xff);
    }

    public static int updateEventDetetion(int speedOrLength, boolean val) {
        int retVal = speedOrLength;
        if (val) {
            retVal = speedOrLength | (0x01 << 16);//set bit no 16 to One
        } else {
            retVal = speedOrLength & (~(0x01 << 16));//set bit no 16 to Zero
        }
        return retVal;
    }

    public boolean getEventDetectionEnabled() {
        return ((speedOrLength >> 16) & 0x01) > 0;
    }

    public static int updateMotionEventDetetion(int speedOrLength, boolean val) {
        int retVal = speedOrLength;
        if (val) {
            retVal = speedOrLength | (0x01 << 17);//set bit no 16 to One
        } else {
            retVal = speedOrLength & (~(0x01 << 17));//set bit no 16 to Zero
        }
        return retVal;
    }

    public boolean getMotionEventDetectionEnabled() {
        return ((speedOrLength >> 17) & 0x01) > 0;
    }

    public static int updateFaceEventDetetion(int speedOrLength, boolean val) {
        int retVal = speedOrLength;
        if (val) {
            retVal = speedOrLength | (0x01 << 18);//set bit no 16 to One
        } else {
            retVal = speedOrLength & (~(0x01 << 18));//set bit no 16 to Zero
        }
        return retVal;
    }

    public boolean getFaceEventDetectionEnabled() {
        return ((speedOrLength >> 18) & 0x01) > 0;
    }

    public static int getSpeedOrLength(int speed, boolean eventDetectEnabled, int eventSpeed, boolean motionEventDetectEnabled, boolean faceEventDetectEnabled) {
        int speedOrLength = 0;
        speedOrLength = updateSpeed(speedOrLength, speed);

        speedOrLength = updateEventDetetion(speedOrLength, eventDetectEnabled);
        speedOrLength = updateEventSpeed(speedOrLength, eventSpeed);
        speedOrLength = updateMotionEventDetetion(speedOrLength, motionEventDetectEnabled);
        speedOrLength = updateFaceEventDetetion(speedOrLength, faceEventDetectEnabled);
        return speedOrLength;
    }
    //END of TIMELAPSE => Speed storage spec

    //HIGHLIGHTS => Length & theme storage spec:
    // LSB(Byte1) will have length,
    public static int updateLength(int speedOrLength, int length) {
        if (length > 255) {
            throw new IndexOutOfBoundsException("length(" + length + ") is > 8 bits, storage limitation");
        }
        return (speedOrLength & 0xFFFFFF00) | (length & 0xFF);
    }

    public int getLength() {
        return (speedOrLength & 0xff);
    }

    public static int getSpeedOrLength(int length, int theme) {
        int speedOrLength = 0;
        speedOrLength = updateLength(speedOrLength, length);
        speedOrLength = updateTheme(speedOrLength, theme);
        return speedOrLength;
    }
    //END of HIGHLIGHTS => Length & theme storage spec

    //Highlights/Timelapse common Theme will be stored in MSB(Byte4) will have theme ID (valuse will be 0, 1,2,3,4..,)
    public static int updateTheme(int speedOrLength, int theme) {
        if (theme > 255) {
            throw new IndexOutOfBoundsException("theme (" + theme + ") is > 255, storage limitation");
        }
        return (speedOrLength & 0x00FFFFFF) | ((theme & 0xFF) << 24);
    }

    public int getTheme() {
        return ((speedOrLength >> 24) & 0xff);
    }

    public void setTheme(int theme) {
        speedOrLength = updateTheme(speedOrLength, theme);
    }

}
