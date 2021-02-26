/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.engine;

import java.lang.ref.WeakReference;
import java.util.ArrayList;

import android.graphics.Bitmap;
import android.graphics.RectF;
import android.util.Log;

import com.qualcomm.qti.sta.data.ApplicationMessage;
import com.qualcomm.qti.sta.data.CategoryData;
import com.qualcomm.qti.sta.data.DateTextHandler;
import com.qualcomm.qti.sta.data.RecognizedData;
import com.qualcomm.qti.sta.data.Utils;
import com.qualcomm.qti.sta.engine.LibraryHandler.JniActionListener;

/**
 * Manager class for interfacing with library
 */
public class JniManager implements JniActionListener {
    private static final String LOG_TAG = "JniManager";

    /**
     * class to maintain the current preview session data
     */
    public class FrameSession {
        private boolean active = false;
        private byte[] imageBuffer = null;
        private int width = 0;
        private int height = 0;
        private int angle = 0;
        private RectF roi = null;

        /**
         * Checks if the session is active
         *
         * @return true if active, false otherwise
         */
        public boolean isActive() {
            return active;
        }

        /**
         * gets the current image bytes reference
         *
         * @return current image bytes
         */
        public byte[] getImageBuffer() {
            return imageBuffer;
        }

        private void setImageBuffer(byte[] imageBuffer, int width, int height, int angle, RectF roi) {
            this.imageBuffer = imageBuffer;
            this.width = width;
            this.height = height;
            this.angle = angle;
            this.roi = roi;
        }

        private void resetImage() {
            setImageBuffer(null, 0, 0, 0, null);
        }

        private void reset() {
            active = false;
            imageBuffer = null;
        }

        private void start() {
            active = true;
        }
    }

    private long requestTimestamp = 0;
    private int dataId = 0;
    private CategoryData categoryData = null;
    private Object dataLock = new Object();

    private Parser staParser = new Parser();
    private DataHandler dataHandler = new DataHandler();
    private FrameSession frameSession = new FrameSession();

    /**
     * Listener for STA result notifications
     */
    public interface STAListener {
        /**
         * Notification on getting any error form the sta library
         *
         * @param errorMessage
         *            Application error message
         */
        public void onSTAError(ApplicationMessage errorMessage);

        /**
         * Notification on getting STA results
         */
        public void onSTAResult();

        /**
         * Notification on STA Ready to parse image
         */
        public void onSTAReady();

        /**
         * Notification on Restarting parsing
         */
        public void onSTAParsingInitialized();

        /**
         * get the rectified image bytes
         *
         * @param Bitmap
         *            rectified image
         */
        public void onImageRectified(Bitmap bitmap);
    }

    private final LibraryHandler libHandler = new LibraryHandler();

    private static WeakReference<JniManager> instance = null;
    private int referenceCount = 0;
    private STAListener staListener = null;
    private String finalImage = null;

    /**
     * a method to get the singleton object of the class
     *
     * @return object of the singleton class
     */
    public static synchronized JniManager getInstance() {
        JniManager jniManager;
        if (instance == null || instance.get() == null) {
            Log.d(LOG_TAG, "JniManager created");
            jniManager = new JniManager();
            instance = new WeakReference<JniManager>(jniManager);
        } else {
            jniManager = instance.get();
        }
        jniManager.referenceCount++;
        Log.d(LOG_TAG, "Reference count: " + jniManager.referenceCount);
        return jniManager;
    }

    private JniManager() {
        libHandler.setJniActionListener(this);
    }

    /**
     * Sets the STA parser listener
     *
     * @param listener
     *            STA parser listener
     */
    public void setParserListener(STAListener listener) {
        synchronized (dataLock) {
            staListener = listener;
        }
    }

    /**
     * Initializes the STA parser
     *
     * @param scveMode
     *            SCVE mode
     * @param textRecordMode
     *            text record mode for STA
     * @param locale
     *            locale for STA
     * @param dataPatterns
     *            STA data patterns to be look for
     * @param dateTextHandler
     *            date text handler
     * @param finalImage
     *            image where snapshot is saved
     */
    public void initializeParser(int scveMode, int textRecordMode, String locale, int dataPatterns,
            DateTextHandler dateTextHandler, String finalImage) {
        synchronized (dataLock) {
            this.finalImage = finalImage;
            dataHandler.setDateTextHandler(dateTextHandler);
            staParser.initialize(libHandler, staListener, scveMode, textRecordMode, locale, dataPatterns);
        }
    }

    /**
     * Releases the STA Library
     */
    public void release() {
        referenceCount--;
        Log.d(LOG_TAG, "Reference count: " + referenceCount);
        if (referenceCount == 0) {
            if (!staParser.isState(Parser.State.NOT_INITIALIZED)) {
                staParser.setState(Parser.State.STOPPING);
                Log.d(LOG_TAG, "Parser: Stopping");
                libHandler.wrapUp();
            }
        }
    }

    /**
     * Parses the image for text patterns
     *
     * @param bitmap
     *            image to be parsed
     */
    public void parseImage(final Bitmap bitmap) {
        synchronized (dataLock) {
            dataHandler.setSourceBitmap(bitmap);
            requestTimestamp = System.currentTimeMillis();
            dataId++;
            Log.d(LOG_TAG, "capture with dataID: " + dataId);
            libHandler.parseImage(bitmap, requestTimestamp, dataId);
        }
    }

    /**
     * Sends an image for Rectification
     *
     * @param snapshot
     *            snapshot bitmap
     */
    public void rectifyImage(Bitmap snapshot, int regionExtractionMode, int imageEnhancedMode) {
        synchronized (dataLock) {
            Log.d(LOG_TAG, "IC: Correct Image");
            libHandler.rectifyImage(snapshot, regionExtractionMode, imageEnhancedMode);
        }
    }

    /**
     * Sets the next frame for Parser. this should be called only in preview
     * capture mode
     *
     * @param imageBytes
     *            nv21 image bytes
     * @param width
     *            image width
     * @param height
     *            image height
     * @param angle
     *            image angle
     * @return Preview session data
     */
    public FrameSession setNextFrame(byte[] imageBytes, int width, int height, RectF roi, int angle) {
        synchronized (dataLock) {
            if (!frameSession.isActive()) {
                Log.d(LOG_TAG, "Frame: Rejected - no active process");
                frameSession.reset();
                return frameSession;
            }
            if (frameSession.getImageBuffer() == null) {
                Log.d(LOG_TAG, "Frame: Accepted - " + imageBytes);
                frameSession.setImageBuffer(imageBytes, width, height, angle, roi);
                requestTimestamp = System.currentTimeMillis();
                libHandler.parseFrame(imageBytes, width, height, angle, roi, requestTimestamp,
                        dataId);
            } else {
                //Log.d(LOG_TAG, "Frame: Rejected - " + imageBytes + ", Already processing - "
                //        + previewSession.getImageBuffer());
            }
            return frameSession;
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onDataListReady(ArrayList<RecognizedData> dataList, long timestamp) {
        Log.d(LOG_TAG, "Got data list");
        synchronized (dataLock) {

            if (requestTimestamp > 0 && requestTimestamp == timestamp) {// is cancelled
                if (staParser.getMode() == JniConstants.PARSER_MODE_PREVIEW) {
                    if (!frameSession.isActive()) {
                        Log.d(LOG_TAG, "Data rejected, session not valid");
                        return;
                    }
                    dataHandler.setPreviewFrameData(frameSession.imageBuffer, frameSession.width,
                            frameSession.height, frameSession.angle, frameSession.roi);
                    frameSession.reset();
                }

                dataHandler.saveFinalImage(finalImage);
                if (dataHandler.addNewData(dataList, staListener)) {
                    if (staListener != null) {
                        staListener.onSTAResult();
                    }
                    Log.d(LOG_TAG, "Data added");
                }

            } else {
                Log.d(LOG_TAG, "Data rejected");
            }
        }
    }

    /**
     * Cancels the current parsing request
     */
    public void resetParsing(int resetDataId) {
        synchronized (dataLock) {
            requestTimestamp = 0;

            frameSession.reset();
            dataHandler.reset(resetDataId);
            if (resetDataId > 0) {
                dataId = resetDataId - 1;
                Log.d(LOG_TAG, "new dataID: " + dataId);
            }

        }
    }

    /**
     * gets the parsed out STA data
     *
     * @return STA data
     */
    public ArrayList<RecognizedData> getDataList() {
        synchronized (dataLock) {
            return dataHandler.getDataList();
        }
    }

    /**
     * gets the data ID
     *
     * @return data id
     */
    public int getDataId() {
        synchronized (dataLock) {
            return dataId;
        }
    }

    /**
     * Clear the STA Data
     */
    public void clearData() {
        synchronized (dataLock) {
            frameSession.reset();
            dataHandler.reset(-1);
            dataId = 0;

            categoryData = null;
        }
    }

    /**
     * Get preview frame bitmap
     *
     * @return preview frame bitmap
     */
    public Bitmap getPreviewFrameBitmap() {
        synchronized (dataLock) {
            return dataHandler.getPreviewFrameBitmap();
        }
    }

    /**
     * gets the current category data
     *
     * @return category data
     */
    public CategoryData getCategoryData() {
        synchronized (dataLock) {
            return categoryData;
        }
    }

    /**
     * Sets the category data
     *
     * @param data
     *            category data
     */
    public void setCategoryData(CategoryData data) {
        synchronized (dataLock) {
            categoryData = data;
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onJniActionInProgress(int progressCode) {
        synchronized (dataLock) {
            frameSession.resetImage();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onJniActionError(ApplicationMessage message) {
        synchronized (dataLock) {
            if(staParser.isState(Parser.State.STARTING) ) {
                staParser.setState(Parser.State.NOT_INITIALIZED);
            }
            if (staListener != null) {
                staListener.onSTAError(message);
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onParserReady() {
        synchronized (dataLock) {
            staParser.setState(Parser.State.STARTED);
            Log.d(LOG_TAG, "Parser: Ready");
            if (staListener != null) {
                staListener.onSTAReady();
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onParserReleased() {
        synchronized (dataLock) {
            staParser.setState(Parser.State.NOT_INITIALIZED);
            Log.d(LOG_TAG, "Parser: Stopped");
        }
    }

    /**
     * check if the parser is initialized
     *
     * @return true if initialized, false otherwise
     */
    public boolean isParserInitialized() {
        synchronized (dataLock) {
            return staParser.isState(Parser.State.STARTED);
        }
    }

    /**
     * gets the parser mode
     *
     * @return parser mode
     */
    public int getParserMode() {
        synchronized (dataLock) {
            return staParser.getMode();
        }
    }

    /**
     * notifies the manager on starting preview session
     */
    public void startPreviewSession() {
        synchronized (dataLock) {
            frameSession.start();
            dataId++;
            Log.d(LOG_TAG, "capture with dataID: " + dataId);
            libHandler.restart(staParser.getMode());
        }
    }

    /**
     * updates the STA data with new updated text
     *
     * @param rawDataIndex
     *            data index in raw list
     * @param newText
     *            text to be updated
     */
    public void updateDataText(int rawDataIndex, String newText) {
        synchronized (dataLock) {
            dataHandler.updateText(rawDataIndex, newText);
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onParsingInitailized() {
        synchronized (dataLock) {
            Log.d(LOG_TAG, "Parser: Parsing initialized");
            if (staListener != null) {
                staListener.onSTAParsingInitialized();
            }
        }
    }
    /**
     * {@inheritDoc}
     */
    @Override
    public void onImageRectified(byte[] nv21Bytes, int width, int height) {
        synchronized (dataLock) {
            Log.d(LOG_TAG, "IC: Image rectified");
            Bitmap icImage = Utils.getBitmapFromYUV(nv21Bytes, width, height);
            Utils.saveBitmap(finalImage, icImage);
            if (staListener != null) {
                staListener.onImageRectified(icImage);
            }
        }
    }

}
