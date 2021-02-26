/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.engine;

import java.util.ArrayList;

import android.graphics.Bitmap;
import android.graphics.RectF;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;

import com.qualcomm.qti.sta.data.ApplicationMessage;
import com.qualcomm.qti.sta.data.DateTimeData;
import com.qualcomm.qti.sta.data.DateTimeRange;
import com.qualcomm.qti.sta.data.RecognizedData;
import com.qualcomm.qti.sta.data.RecognizedRect;

/**
 * Class to interact with STA library through JNI layer
 */
public class LibraryHandler {
    private static final String LOG_TAG = "LibraryHandler";

    private JniActionListener jniActionListener = null;
    private HandlerThread libraryThread = null;
    private Handler libraryHandler = null;

    static {
        System.loadLibrary("scveTextRecoPostProcessing");
        System.loadLibrary("fastcvopt");
        System.loadLibrary("scveImageCorrection");
        System.loadLibrary("scveTextReco");
        System.loadLibrary("scveTextReco_stub");
        System.loadLibrary("scveCommon");
        System.loadLibrary("scveCommon_stub");
        System.loadLibrary("STAWrapper");

        nClassInit();
    }

    /**
     * Interface for Jni Action listener
     */
    public interface JniActionListener {
        /**
         * Notification for the parsed out recognized data
         *
         * @param dataList
         *            new data list
         * @param timestamp
         *            timestamp of the request
         */
        public void onDataListReady(ArrayList<RecognizedData> dataList, long timestamp);

        /**
         * Notification in case of any JNI error
         *
         * @param message
         *            error message
         */
        public void onJniActionError(ApplicationMessage message);

        /**
         * Notification in case of JNI progress message
         *
         */
        public void onJniActionInProgress(int progressCode);

        /**
         * Notification on parser reconfigured
         */
        public void onParserReady();

        /**
         * Notification on parser released
         */
        public void onParserReleased();

        /**
         * Notification on parser released
         */
        public void onParsingInitailized();

        /**
         * get the rectified image bytes
         *
         * @param nv21Bytes
         *            image bytes
         * @param width
         *            width of the image
         * @param height
         *            height of the image
         */
        public void onImageRectified(byte[] nv21Bytes, int width, int height);
    }

    /**
     * Sets the jni action Listener
     *
     * @param listener
     */
    /* package */void setJniActionListener(JniActionListener listener) {
        jniActionListener = listener;
    }

    /**
     * initializes the STA library
     *
     * @param textRecordMode
     *            text record mode
     * @param locale
     *            locale for the JniHandler parser
     * @param dataPatterns
     *            dataPatterns for parser. these dataPatterns determine which
     *            data type needs to look for
     */
    /* package */void init(int scveMode, int textRecordMode, String locale, int dataPatterns) {
        if (libraryThread != null) {
            throw new RuntimeException("JniHandler is already initialized");
        }
        libraryThread = new HandlerThread("JniHandler");
        libraryThread.start();
        libraryHandler = new Handler(libraryThread.getLooper());
        libraryHandler.post(new DoInitRunnable(scveMode, textRecordMode, locale, dataPatterns));
    }

    /**
     * reinitializes the STA library
     *
     * @param textRecordMode
     *            text record mode
     * @param locale
     *            locale for the JniHandler parser
     * @param dataPatterns
     *            dataPatterns for parser. these dataPatterns determine which
     *            data type needs to look for
     */
    public void restartEngine(int scveMode, int textRecordMode, String locale, int dataPatterns) {
        libraryHandler.post(new DoInitRunnable(scveMode, textRecordMode, locale, dataPatterns, true));
    }

    /**
     * reconfigure up the sta library
     *
     * @param locale
     *            locale for the parser
     * @param dataPatterns
     *            dataPatterns for parser. these dataPatterns determine which
     *            data type needs to look for
     */
    public void reconfigureHandler(String locale, int dataPatterns) {
        libraryHandler.post(new DoReconfigureHandlerRunnable(locale, dataPatterns));
    }

    /**
     * restarts the text handler with new values
     *
     * @param textRecordMode
     *            text record mode
     * @param locale
     *            locale for the parser
     * @param dataPatterns
     *            dataPatterns for parser. these dataPatterns determine which
     *            data type needs to look for
     */
    public void restartHandler(int textRecordMode, String locale, int dataPatterns) {
        libraryHandler.post(new DoRestartHandlerRunnable(textRecordMode, locale, dataPatterns));

    }

    /**
     * restarts the library engine
     *
     * @param textRecordMode
     *            text record handler mode
     */
    public void restart(int textRecordMode) {
        libraryHandler.post(new DoRestartRunnable(textRecordMode));
    }

    /**
     * Clean up the library handler
     */
    /* package */void wrapUp() {
        if (libraryHandler == null) {
            throw new IllegalArgumentException("JniHandler is not initialized");
        }

        libraryHandler.post(doRelease);
        Log.d(LOG_TAG, "Wait for library thread");
        try {
            libraryThread.join();
            Log.d(LOG_TAG, "library thread joined");

        } catch (InterruptedException e) {
            Log.w(LOG_TAG, "Interrupted while waiting for Library thread to finish");
            Thread.currentThread().interrupt();
        }
        libraryHandler = null;
        libraryThread = null;
    }

    /**
     * Sends the image to the sta library for parsing.
     *
     * @param rgbaBitmap
     *            rgba bitmap image
     * @param timestamp
     *            request timestamp
     * @param staDataId
     *            sta data id
     */
    /* package */void parseImage(Bitmap rgbaBitmap, long timestamp, int staDataId) {
        if (libraryHandler == null) {
            throw new IllegalArgumentException("JniHandler is not initialized");
        }
        libraryHandler.post(new DoParseImageRunnable(rgbaBitmap, timestamp, staDataId));
    }

    /**
     * Apply Image correction algorithm on captured snapshot
     *
     * @param snapshot
     *            Snapshot bitmap
     */
    /* package */void rectifyImage(Bitmap snapshot, int regionExtractionMode, int imageEnhancedMode) {
        if (libraryHandler == null) {
            throw new IllegalArgumentException("JniHandler is not initialized");
        }
        libraryHandler.post(new DoCorrectImageRunnable(snapshot, regionExtractionMode,
                imageEnhancedMode));
    }

    /**
     * Parses the preview frame for algorithm
     *
     * @param imageBytes
     *            nv21 image bytes
     * @param width
     *            image width
     * @param height
     *            image height
     * @param angle
     *            image angle
     * @param timestamp
     *            request timestamp
     * @param staDataId
     *            sta data id
     */
    /* package */void parseFrame(byte[] imageBytes, int width, int height, int angle,
            RectF imageRect, long timestamp, int staDataId) {
        if (libraryHandler == null) {
            throw new IllegalArgumentException("JniHandler is not initialized");
        }
        libraryHandler.post(new DoParseFrameRunnable(imageBytes, width, height, angle, imageRect,
                timestamp, staDataId));
    }

    // N.B. Runs on the library thread
    private class DoInitRunnable implements Runnable {
        private final int scveMode;
        private final int textRecordMode;
        private final String locale;
        private final int dataPatterns;
        private final boolean reInit;

        private DoInitRunnable(int scveMode, int textRecordMode, String locale, int dataPatterns) {
            this(scveMode, textRecordMode, locale, dataPatterns, false);
        }
        private DoInitRunnable(int scveMode, int textRecordMode, String locale, int dataPatterns, boolean reInit) {
            this.scveMode = scveMode;
            this.textRecordMode = textRecordMode;
            this.locale = locale;
            this.dataPatterns = dataPatterns;
            this.reInit = reInit;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void run() {
            if(reInit) {
                Log.i(LOG_TAG, "Re-Init Message");
                nSTARelease();

            } else {
            Log.i(LOG_TAG, "Init Message");
            }
            int returnStatus = nSTAInit(scveMode, textRecordMode, locale, dataPatterns);
            if (returnStatus != JniConstants.STA_RESULT_SUCCESS) {
                Log.d(LOG_TAG, "Initializing sta failed: " + returnStatus);
                if (jniActionListener != null) {
                    ApplicationMessage errorMessage = ApplicationMessage.STA_API_INIT_FAILED;
                    errorMessage.setErrorValue(returnStatus);
                    jniActionListener.onJniActionError(errorMessage);
                }
            } else {
                if (jniActionListener != null) {
                    jniActionListener.onParserReady();
                }
                Log.i(LOG_TAG, "Initialized sta library with scveMode: " + scveMode  + " mode:" + textRecordMode + ", locale: "
                        + locale + ", dataPatterns: " + dataPatterns);
            }
        }
    }

    // N.B. Runs on the library thread
    private class DoRestartHandlerRunnable implements Runnable {
        private final int textRecordMode;
        private final String locale;
        private final int dataPatterns;

        private DoRestartHandlerRunnable(int textRecordMode, String locale, int dataPatterns) {
            this.textRecordMode = textRecordMode;
            this.locale = locale;
            this.dataPatterns = dataPatterns;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void run() {
            Log.i(LOG_TAG, "Restart handler Message");
            int returnStatus = nSTARestartTextHandler(textRecordMode, locale, dataPatterns);
            if (returnStatus != JniConstants.STA_RESULT_SUCCESS) {
                Log.d(LOG_TAG, "Restart sta failed: " + returnStatus);
                if (jniActionListener != null) {
                    ApplicationMessage errorMessage = ApplicationMessage.STA_API_INIT_FAILED;
                    errorMessage.setErrorValue(returnStatus);
                    jniActionListener.onJniActionError(errorMessage);
                    return;
                }
            } else {
                if (jniActionListener != null) {
                    jniActionListener.onParserReady();
                }
                Log.i(LOG_TAG, "Restarted with mode: " + textRecordMode + ", data patterns: "
                        + dataPatterns + ", locale: " + locale);
            }
        }
    }

    // N.B. Runs on the library thread
    private class DoReconfigureHandlerRunnable implements Runnable {
        private final String locale;
        private final int dataPatterns;

        private DoReconfigureHandlerRunnable(String locale, int dataPatterns) {
            this.locale = locale;
            this.dataPatterns = dataPatterns;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void run() {
            Log.i(LOG_TAG, "Reconfigure handler Message");
            int returnStatus = nSTAReconfigureTextHandler(locale, dataPatterns);
            if (returnStatus != JniConstants.STA_RESULT_SUCCESS) {
                Log.d(LOG_TAG, "ReConfigure sta failed: " + returnStatus);
                if (jniActionListener != null) {
                    ApplicationMessage errorMessage = ApplicationMessage.STA_API_INIT_FAILED;
                    errorMessage.setErrorValue(returnStatus);
                    jniActionListener.onJniActionError(errorMessage);
                    return;
                }
            } else {
                if (jniActionListener != null) {
                    jniActionListener.onParserReady();
                }
                Log.i(LOG_TAG, "Reconfigure with data patterns:" + dataPatterns + ", locale: "
                        + locale);
            }
        }
    }

    Runnable doRelease = new Runnable() {
        /**
         * {@inheritDoc}
         */
        @Override
        public void run() {
            Log.i(LOG_TAG, "Exit Message");
            libraryHandler.getLooper().quit();
            nSTARelease();
            jniActionListener.onParserReleased();
        }
    };

    private class DoCorrectImageRunnable implements Runnable {
        private final Bitmap rgbaBitmap;
        private final int regionExtractionMode;
        private final int imageEnhancedMode;

        private DoCorrectImageRunnable(Bitmap rgbaBitmap, int regionExtractionMode,
                int imageEnhancedMode) {
            this.rgbaBitmap = rgbaBitmap;
            this.regionExtractionMode = regionExtractionMode;
            this.imageEnhancedMode = imageEnhancedMode;
        }

        private byte[] getNV21(Bitmap argbBitmap) {
            int width = argbBitmap.getWidth();
            int height = argbBitmap.getHeight();
            int[] argb = new int[width * height];

            argbBitmap.getPixels(argb, 0, width, 0, 0, width, height);

            byte[] nv21Bytes = new byte[width * height * 3 / 2];
            convertToNv21(nv21Bytes, argb, width, height);
            return nv21Bytes;
        }

        private void convertToNv21(byte[] nv21Bytes, int[] argb, int width, int height) {
            final int frameSize = width * height;

            int yIndex = 0;
            int uvIndex = frameSize;

            int rValue, gValue, bValue;
            int yValue, uValue, vValue;
            int index = 0;
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {

                    rValue = (argb[index] & 0xff0000) >> 16;
                    gValue = (argb[index] & 0xff00) >> 8;
                    bValue = (argb[index] & 0xff) >> 0;

                    yValue = ((66 * rValue + 129 * gValue + 25 * bValue + 128) >> 8) + 16;
                    uValue = ((-38 * rValue - 74 * gValue + 112 * bValue + 128) >> 8) + 128;
                    vValue = ((112 * rValue - 94 * gValue - 18 * bValue + 128) >> 8) + 128;

                    nv21Bytes[yIndex++] = (byte) ((yValue < 0) ? 0
                            : ((yValue > 255) ? 255 : yValue));
                    if (j % 2 == 0 && index % 2 == 0) {
                        nv21Bytes[uvIndex++] = (byte) ((vValue < 0) ? 0 : ((vValue > 255) ? 255
                                : vValue));
                        nv21Bytes[uvIndex++] = (byte) ((uValue < 0) ? 0 : ((uValue > 255) ? 255
                                : uValue));
                    }

                    index++;
                }
            }
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void run() {
            Log.i(LOG_TAG, "Correct Message");
            byte[] nv21Bytes = getNV21(rgbaBitmap);

            int result = nIcInit();
            if (result != JniConstants.STA_RESULT_SUCCESS) {
                Log.d(LOG_TAG, "IC intialize failed. Error code: 0x" + Integer.toHexString(result));
                ApplicationMessage error = ApplicationMessage.IC_API_INIT_FAILED;
                error.setErrorValue(result);
                jniActionListener.onJniActionError(error);
                return;
            }
            Log.d(LOG_TAG, "IC init success");

            int[] outValues = new int[1];
            result = nIcRectifyImage(nv21Bytes, rgbaBitmap.getWidth(), rgbaBitmap.getHeight(),
                    regionExtractionMode, imageEnhancedMode, JniConstants.IC_SCALE_HIGH_RESOLUTION,
                    outValues);

            if (result != JniConstants.STA_RESULT_SUCCESS) {
                Log.d(LOG_TAG, "Failed to make rectified image: 0x" + Integer.toHexString(result));
                ApplicationMessage error = ApplicationMessage.IC_API_RECTIFY_FAILED;
                error.setErrorValue(result);
                jniActionListener.onJniActionError(error);
                return;

            } else if (outValues[0] <= 0) {
                ApplicationMessage error = ApplicationMessage.IC_API_NO_CANDIDATE;
                jniActionListener.onJniActionError(error);
                return;
            }
            int[] imageSize = new int[2];
            if (nIcGetRectifiedImageInfo(0, imageSize) != JniConstants.STA_RESULT_SUCCESS) {
                Log.d(LOG_TAG, "Failed to get a rectified image info");
                ApplicationMessage error = ApplicationMessage.IC_API_GET_RECTIFY_FAILED;
                error.setErrorValue(result);
                jniActionListener.onJniActionError(error);
                return;
            } else if (imageSize[0] <= 0 || imageSize[1] <= 0) {
                Log.d(LOG_TAG, "Wrong image size: " + imageSize[0] + " X " + imageSize[1]);
                ApplicationMessage error = ApplicationMessage.IC_API_WRONG_IMAGE;
                jniActionListener.onJniActionError(error);
                return;
            }
            byte[] rectifiedBytes = new byte[imageSize[0] * imageSize[1] * 3 / 2];
            result = nIcGetRectifiedImage(0, rectifiedBytes);
            if (result != JniConstants.STA_RESULT_SUCCESS) {
                Log.d(LOG_TAG, "ImageCorrection: failed to get a rectified image");
                ApplicationMessage error = ApplicationMessage.IC_API_GET_RECTIFY_FAILED;
                error.setErrorValue(result);
                jniActionListener.onJniActionError(error);
                return;
            }
            Log.d(LOG_TAG, "Successfully got a rectified image.");
            jniActionListener.onImageRectified(rectifiedBytes, imageSize[0], imageSize[1]);

            nIcWrap();
        }
    }

    private class DoParseImageRunnable implements Runnable {
        private final long timestamp;
        private final int staDataId;
        private final Bitmap rgbaBitmap;

        private DoParseImageRunnable(Bitmap rgbaBitmap, long timestamp, int staDataId) {
            this.rgbaBitmap = rgbaBitmap;
            this.timestamp = timestamp;
            this.staDataId = staDataId;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void run() {
            Log.i(LOG_TAG, "Parse Message");
            if (rgbaBitmap == null) {
                Log.d(LOG_TAG, "Image data null");
                return;
            }
            int[] outValues = new int[2];
            int resultStatus = nSTAParseImage(rgbaBitmap, outValues);
            if (resultStatus != JniConstants.STA_RESULT_SUCCESS) {
                Log.d(LOG_TAG, "STA: parse image failed: " + resultStatus);
                if (jniActionListener != null) {
                    ApplicationMessage errorMessage = ApplicationMessage.STA_API_PARSING_FAILED;
                    errorMessage.setErrorValue(resultStatus);
                    jniActionListener.onJniActionError(errorMessage);
                }
                return;
            }
            populateResults(outValues[0], timestamp, staDataId);
        }
    }

    private class DoParseFrameRunnable implements Runnable {
        private final long timestamp;
        private final int staDataId;

        private final byte[] imageBytes;
        private final int width;
        private final int height;
        private final int angle;

        private final RectF imageRect;

        private DoParseFrameRunnable(byte[] imageBytes, int width, int height, int angle,
                RectF imageRect, long timestamp, int staDataId) {
            this.imageBytes = imageBytes;
            this.width = width;
            this.height = height;
            this.angle = angle;
            this.imageRect = imageRect;

            this.timestamp = timestamp;
            this.staDataId = staDataId;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void run() {
            Log.i(LOG_TAG, "Parse Message");
            if (imageBytes == null) {
                Log.d(LOG_TAG, "Image bytes null");
                return;
            }
            int[] outValues = new int[2];
            int resultStatus = nSTAParseFrame(imageBytes, width, height, angle,
                    (int) imageRect.left, (int) imageRect.top, (int) imageRect.width(),
                    (int) imageRect.height(), outValues);
            if (resultStatus == JniConstants.STA_RESULT_IN_PROGRESS
                    || resultStatus == JniConstants.STA_RESULT_EMPTY) {
                Log.d(LOG_TAG, "STA: parsing in progress, status: " + resultStatus);
                if (jniActionListener != null) {
                    jniActionListener.onJniActionInProgress(resultStatus);
                }

            } else if (resultStatus != JniConstants.STA_RESULT_SUCCESS) {
                Log.d(LOG_TAG, "STA: parsing error " + resultStatus);
                if (jniActionListener != null) {
                    ApplicationMessage errorMessage;
                    if (resultStatus == JniConstants.STA_RESULT_TIMEOUT) {
                        errorMessage = ApplicationMessage.STA_API_PARSING_TIMEOUT;
                    } else {
                        errorMessage = ApplicationMessage.STA_API_PARSING_FAILED;
                        errorMessage.setErrorValue(resultStatus);
                    }
                    jniActionListener.onJniActionError(errorMessage);

                }
            } else {
                populateResults(outValues[0], timestamp, staDataId);
            }
        }
    }

    // N.B. Runs on the library thread
    private class DoRestartRunnable implements Runnable {
        private final int textRecordMode;

        private DoRestartRunnable(int textRecordMode) {
            this.textRecordMode = textRecordMode;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void run() {
            Log.i(LOG_TAG, "Restart Message");
            int resultStatus = nSTARestartRecording(textRecordMode);

            if (jniActionListener != null) {
                if (resultStatus == JniConstants.STA_RESULT_SUCCESS) {
                    jniActionListener.onParsingInitailized();
                } else {
                    Log.d(LOG_TAG, "Restart status: " + resultStatus);
                    ApplicationMessage message = ApplicationMessage.STA_API_PARSING_RESTART_FAILED;
                    message.setErrorValue(resultStatus);
                    jniActionListener.onJniActionError(message);
                }

            }
        }
    }

    private ArrayList<String> parseDateTimeData(int resultIndex, int dateCount) {
        ArrayList<String> dates = new ArrayList<String>();
        StringBuffer dateTimeText = new StringBuffer();

        for (int d = 0; d < dateCount; d++) {
            dateTimeText.setLength(0);
            if (nSTAGetDatetime(resultIndex, d, dateTimeText)) {
                String dateTime = dateTimeText.toString();
                if (dateTime.length() == DateTimeRange.LIB_FORMAT_LENGTH) {
                    Log.d(LOG_TAG, "DateTime at " + d + ":" + dateTime);
                    dates.add(dateTime);
                } else {
                    Log.d(LOG_TAG, "STA: invalid datetime at " + d + ":" + dateTime);
                }
            } else {
                Log.d(LOG_TAG, "STA: record failed to fetch at: " + resultIndex);
            }
        }
        return dates;
    }

    private void populateResults(int records, long timestamp, int dataId) {
        Log.d(LOG_TAG, "TS: Got results at " + System.currentTimeMillis());
        if (records == 0) {
            Log.d(LOG_TAG, "STA: No records");
            if (jniActionListener != null) {
                jniActionListener.onJniActionError(ApplicationMessage.STA_API_NO_RECORDS);
            }
            return;
        }
        ArrayList<RecognizedData> dataList = new ArrayList<RecognizedData>();
        int[] metadata = new int[11];
        StringBuffer recognizedText = new StringBuffer();

        for (int i = 0; i < records; i++) {
            recognizedText.setLength(0);
            if (nSTAGetData(i, metadata, recognizedText)) {
                RecognizedRect rect = new RecognizedRect(metadata[2], metadata[3], metadata[4],
                        metadata[5], metadata[6], metadata[7], metadata[8], metadata[9]);
                if (metadata[1] == JniConstants.DATA_TYPE_DATETIME) {
                    Log.d(LOG_TAG, "Fetching dates for " + recognizedText.toString() + " at " + i);
                    ArrayList<String> dates = parseDateTimeData(i, metadata[10]);
                    if (dates != null && dates.size() > 0) {
                        Log.d(LOG_TAG, rect.toString());
                        dataList.add(new DateTimeData(rect, JniConstants.DATA_TYPE_DATETIME,
                                dataId, recognizedText.toString(), dates));
                }

                } else {
                    Log.d(LOG_TAG, recognizedText.toString() + " " + rect.toString());
                dataList.add(new RecognizedData(rect, metadata[1], dataId, recognizedText
                            .toString()));
                }
            } else {
                Log.d(LOG_TAG, "STA: record failed to fetch at: " + i);
            }
        }

        if (jniActionListener != null) {
            if (dataList.size() > 0) {
            jniActionListener.onDataListReady(dataList, timestamp);
            } else {
                jniActionListener.onJniActionError(ApplicationMessage.STA_API_NO_RECORDS);
            }
        }
    }

    private long nativeContext; // N.B. Do not modify from Java (only from native code)

    private static native void nClassInit();

    private native int nSTAInit(int ver, int textRecordMode, String locale, int dataPatterns);

    private native int nSTAReconfigureTextHandler(String locale, int dataPatterns);

    private native int nSTARestartTextHandler(int textRecordHandler, String locale, int dataPatterns);

    private native int nSTAParseImage(Bitmap bitmap, int[] values);

    private native int nSTARestartRecording(int textRecordMode);

    private native int nSTAParseFrame(byte[] imageBytes, int width, int height, int angle,
            int roiX, int roiY, int roiWidth, int roiHeight, int[] values);

    private native boolean nSTAGetData(int index, int[] metadata, StringBuffer text);

    private native boolean nSTAGetDatetime(int resultIndex, int datetimeIndex, StringBuffer text);

    private native void nSTARelease();

    private native int nIcInit();

    private native int nIcWrap();

    private native int nIcRectifyImage(byte[] frameBytes, int width, int height,
            int regionExtractionMode, int imageExtractionMode, int scaleMode, int[] numCand);

    private native int nIcGetRectifiedImageInfo(int index, int[] outValues);

    private native int nIcGetRectifiedImage(int index, byte[] frameOutBytes);
}
