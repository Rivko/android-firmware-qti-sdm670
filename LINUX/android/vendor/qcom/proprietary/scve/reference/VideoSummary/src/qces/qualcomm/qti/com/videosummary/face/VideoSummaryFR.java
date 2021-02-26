/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.face;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Point;
import android.net.Uri;
import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.util.ArrayList;

import qces.qualcomm.qti.com.videosummary.FaceRecognition.DetectedFaces.DetectedFaceModel;
import qces.qualcomm.qti.com.videosummary.Util.ImageUtils;


/**
 * Created by matthewf on 12/8/15.
 */
public class VideoSummaryFR {

    public static final int SCVE_MODE_DEFAULT = 0;
    public static final int SCVE_MODE_CPU_OFFLOAD = 1;
    public static final int SCVE_MODE_PERFORMANCE = 3;

    private static final String LOG_TAG = "VideoSummaryFR";
    private static final String FACE_ENGINE_VERSION = "1.0.1";

    private static final int FR_SCORE_MIN = 175;
    private static final int FR_SCORE_MAX = 400;
    private static final int FR_MATCH_PERCENT = 50;

    private static final int FR_MAX_USERS_PER_IMAGE = 30;
    private static final int FR_DATABASE_MAX_USERS = 100;
    private static final int MIN_FACE_SIZE = 64;
    private static final int MAX_FACE_SIZE = 8192;
    private static final int FR_MAX_NUM_RESULTS = 10; // Max recognized faces for a single request

    private static final String SCVE_DATA_FOLDER = "/scveData";
    private static final String FR_DATABASE_NAME = "frdb.dat";

    private final FaceInfo[] faceInfos = new FaceInfo[FR_MAX_USERS_PER_IMAGE];
    private final FaceRecognitionResult[] faceRecognitionResults = new FaceRecognitionResult[FR_MAX_NUM_RESULTS];

    private Context context;
    private String imgUriPath;
    private FaceLibIntf fli = null;
    private File frDatabaseFile;
    private int numFacesDetected;
    private Bitmap rgbaBitmap;
    private ArrayList<DetectedFaceModel> DFMList;

    public VideoSummaryFR(Context context, Uri imgUri) {
        this.context = context;
        if (imgUri != null) {
            this.imgUriPath = ImageUtils.getImagePathFromUri(context, imgUri);
        } else {
            this.imgUriPath = null;
        }
        DFMList = new ArrayList<>();
        initFR();
    }

    private void initFR() {
        if (fli == null)
            fli = new FaceLibIntf();

        frDatabaseFile = new File(Environment.getExternalStorageDirectory() + SCVE_DATA_FOLDER, FR_DATABASE_NAME);

        //instantiate face data arrays
        int result = fli.nCreateScveContext(SCVE_MODE_DEFAULT);
        for (int i = 0; i < faceInfos.length; i++) {
            faceInfos[i] = new FaceInfo();
        }

        for (int i = 0; i < faceRecognitionResults.length; i++) {
            faceRecognitionResults[i] = new FaceRecognitionResult();
        }
        if (result != 0) {
            Log.e(LOG_TAG, "PARSE: SCVE context failed ");
            return;
        }
        Log.d(LOG_TAG, "PARSE: init " + frDatabaseFile.getPath());
        result = fli.nInit(0, FR_MAX_USERS_PER_IMAGE, FR_DATABASE_MAX_USERS, frDatabaseFile.getPath());
        if (result != 0) {
            Log.e(LOG_TAG, "PARSE: init failed: " + result);
            return;
        }

    }

    public void deinitFR() {
        fli.nFreeFaceRecognition();
        fli.nFreeScveContext();
    }

    private void detectFaces() {

        Point imageSize = ImageUtils.getImageSize(imgUriPath);
        int sampleSize = ImageUtils.calculateInSampleSize(imageSize.x, imageSize.y);
        rgbaBitmap = ImageUtils.loadParsableBitmap(imgUriPath, sampleSize, true);

        if (rgbaBitmap == null) {
            Log.e(LOG_TAG, "PARSE: Unable to load image");
            return;
        }
        if (rgbaBitmap.getWidth() < MIN_FACE_SIZE || rgbaBitmap.getHeight() < MIN_FACE_SIZE) {
            Log.e(LOG_TAG, "PARSE: skipping small image");
            return;
        }
        if (rgbaBitmap.getWidth() >= MAX_FACE_SIZE || rgbaBitmap.getHeight() >= MAX_FACE_SIZE) {
            Log.e(LOG_TAG, "PARSE: skipping big image");
            return;
        }

        Log.e(LOG_TAG, "PARSE: going: " + rgbaBitmap.getWidth() + "," + rgbaBitmap.getHeight());

        numFacesDetected = fli.nDetectFaces(rgbaBitmap, faceInfos);

        for (int i = 0; i < numFacesDetected; i++) {

            Bitmap detectedFaceBitmap = thumbnailBitmapFromFaceInfo(faceInfos[i], rgbaBitmap);
            DetectedFaceModel dfm = new DetectedFaceModel(null, null, detectedFaceBitmap, DetectedFaceModel.FaceType.UNKNOWN);
            DFMList.add(dfm);
        }
    }

    private Bitmap thumbnailBitmapFromFaceInfo(FaceInfo fi, Bitmap rgbaBitmap) {
        int leftEdge = Math.min(fi.leftTopX, fi.rightBottomX);
        int rightEdge = Math.max(fi.rightTopX, fi.rightBottomX);
        int topEdge = Math.min(fi.leftTopY, fi.rightTopY);
        int bottomEdge = Math.max(fi.leftBottomY, fi.rightBottomY);

        int x = leftEdge;
        int y = topEdge;
        int width = rightEdge - leftEdge;
        int height = bottomEdge - topEdge;


        //expand the envelope a little to make better thumbnails
        //int newWidth = (int) (width * 1.05);
        int newHeight = (int) (height * 1.5);
        int newWidth = newHeight;
        x = x - ((newWidth - width) / 2);
        y = y - ((newHeight - height) / 2);

        x = Math.max(x, 0);
        y = Math.max(y, 0);
        if ((x + newWidth) > rgbaBitmap.getWidth())
            x = rgbaBitmap.getWidth() - newWidth;
        if ((y + newHeight) > rgbaBitmap.getHeight())
            y = rgbaBitmap.getHeight() - newHeight;


        return Bitmap.createBitmap(rgbaBitmap, x, y, newWidth, newHeight);
    }

    public ArrayList<DetectedFaceModel> doFR() {

        detectFaces();
        int numRecognitions = 0;

        if (numFacesDetected > 0) {
            for (int i = 0; i < numFacesDetected; i++) {

                numRecognitions = fli.nRecognizeFace(rgbaBitmap, faceInfos[i], faceRecognitionResults.length,
                        faceRecognitionResults);

                if (numRecognitions > 0) {
                    DetectedFaceModel dfm = DFMList.get(i);
                    dfm.faceType = DetectedFaceModel.FaceType.KNOWN;
                    //dfm.frDatasetId = faceRecognitionResults[0].pid.toString();
                    dfm.frDatasetId = faceRecognitionResults[0].pid;
                }
            }
        }

        return DFMList;
    }

    public int enrollFace(int id, byte[] pid) {
        FaceInfo fi = faceInfos[id];
        //int result = fli.nEnrollFace(rgbaBitmap, fi, pid.getBytes(StandardCharsets.UTF_8));
        int result = fli.nEnrollFace(rgbaBitmap, fi, pid);
        result &= fli.nSaveDatabase(frDatabaseFile.getPath()); //should persist enrollment data to the file
        return result; //0 if success
    }

    public int removeFaceFromEnrollment(byte[] pid) {
        int result = fli.nRemoveFromDatabase(pid);
        result &= fli.nSaveDatabase(frDatabaseFile.getPath()); //should persist enrollment data to the file
        return result;
    }

}
