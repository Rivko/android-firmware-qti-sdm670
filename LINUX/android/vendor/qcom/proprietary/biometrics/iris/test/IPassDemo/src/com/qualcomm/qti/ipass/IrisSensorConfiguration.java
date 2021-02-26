/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */

/*
 * Copyright (C) 2015 The Android Open Source Project
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
 * limitations under the License
 */

package com.qualcomm.qti.ipass;


import android.os.AsyncTask;
import android.util.AtomicFile;
import android.util.Slog;
import android.util.Xml;

import libcore.io.IoUtils;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlSerializer;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import android.graphics.Rect;
import android.util.Size;


/**
 * Class managing the set of iris  per user across device reboots.
 */
class IrisSensorConfiguration {
    private static final String TAG = "IrisSensorConfiguration";

    private static final String IRIS_SENSOR_FILE = "/data/iris_sensor.xml";

    private static final String IRIS_SENSOR_TAG = "Sensor";
    private static final String ATTR_SENSOR_WIDTH = "width";
    private static final String ATTR_SENSOR_HEIGHT = "height";

    private static final String IRIS_PREVIEW_TAG = "Preview";
    private static final String ATTR_PREVIEW_POS_X = "x";
    private static final String ATTR_PREVIEW_POS_Y = "y";
    private static final String ATTR_PREVIEW_WIDTH = "width";
    private static final String ATTR_PREVIEW_HEIGHT = "height";

    private final File mFile;
    private Size mSensorSize;
    private Rect mPreviewRect;

    public IrisSensorConfiguration(Size defaultSensorSize, Rect defaultPreviewRect) {
        mFile = getFile();
        mSensorSize = defaultSensorSize;
        mPreviewRect = defaultPreviewRect;
        synchronized (this) {
            readStateSyncLocked();
        }
    }

    public Size getSensorSize() {
        return mSensorSize;
    }

    public Rect getPreviewRect() {
        return mPreviewRect;
    }

    public void save() {
        synchronized (this) {
            scheduleWriteStateLocked();
        }
    }

    private static File getFile() {
        return new File(IRIS_SENSOR_FILE);
    }

    private final Runnable mWriteStateRunnable = new Runnable() {
        @Override
        public void run() {
            doWriteState();
        }
    };

    private void scheduleWriteStateLocked() {
        AsyncTask.execute(mWriteStateRunnable);
    }

    private void doWriteState() {
        AtomicFile destination = new AtomicFile(mFile);

        FileOutputStream out = null;
        try {
            out = destination.startWrite();

            XmlSerializer serializer = Xml.newSerializer();
            serializer.setOutput(out, "utf-8");
            serializer.setFeature("http://xmlpull.org/v1/doc/features.html#indent-output", true);
            serializer.startDocument(null, true);

            serializer.startTag(null, IRIS_SENSOR_TAG);
            serializer.attribute(null, ATTR_SENSOR_WIDTH, Integer.toString(mSensorSize.getWidth()));
            serializer.attribute(null, ATTR_SENSOR_HEIGHT, Integer.toString(mSensorSize.getHeight()));
            serializer.endTag(null, IRIS_SENSOR_TAG);

            serializer.startTag(null, IRIS_PREVIEW_TAG);
            serializer.attribute(null, ATTR_PREVIEW_POS_X, Integer.toString(mPreviewRect.left));
            serializer.attribute(null, ATTR_PREVIEW_POS_Y, Integer.toString(mPreviewRect.top));
            serializer.attribute(null, ATTR_PREVIEW_WIDTH, Integer.toString(mPreviewRect.width()));
            serializer.attribute(null, ATTR_PREVIEW_HEIGHT, Integer.toString(mPreviewRect.height()));
            serializer.endTag(null, IRIS_PREVIEW_TAG);

            serializer.endDocument();
            destination.finishWrite(out);

            // Any error while writing is fatal.
        } catch (Throwable t) {
            Slog.wtf(TAG, "Failed to write iris sensor configuration, restoring backup", t);
            destination.failWrite(out);
            throw new IllegalStateException("Failed to write iris sensor configuration", t);
        } finally {
            IoUtils.closeQuietly(out);
        }
    }

    private void readStateSyncLocked() {
        FileInputStream in;
        if (!mFile.exists()) {
            return;
        }
        try {
            in = new FileInputStream(mFile);
        } catch (FileNotFoundException fnfe) {
            Slog.i(TAG, "No iris sensor configuration");
            return;
        }
        try {
            XmlPullParser parser = Xml.newPullParser();
            parser.setInput(in, null);
            parseStateLocked(parser);

        } catch (XmlPullParserException | IOException e) {
            throw new IllegalStateException("Failed parsing iris sensor configuration file: "
                    + mFile , e);
        } finally {
            IoUtils.closeQuietly(in);
        }
    }

    private void parseStateLocked(XmlPullParser parser)
            throws IOException, XmlPullParserException {
        final int outerDepth = parser.getDepth();
        int type;
        while ((type = parser.next()) != XmlPullParser.END_DOCUMENT
                && (type != XmlPullParser.END_TAG || parser.getDepth() > outerDepth)) {
            if (type == XmlPullParser.END_TAG || type == XmlPullParser.TEXT) {
                continue;
            }

            String tagName = parser.getName();
            if (tagName.equals(IRIS_SENSOR_TAG)) {
                parseIrisSensorLocked(parser);
            } else if (tagName.equals(IRIS_PREVIEW_TAG)) {
                parseIrisPreviewLocked(parser);
            }
        }
    }

    private void parseIrisSensorLocked(XmlPullParser parser)
            throws IOException, XmlPullParserException {

        String sensorWidth= parser.getAttributeValue(null, ATTR_SENSOR_WIDTH);
        String sensorHeight = parser.getAttributeValue(null, ATTR_SENSOR_HEIGHT);

        mSensorSize = new Size(Integer.parseInt(sensorWidth), Integer.parseInt(sensorHeight));
    }

    private void parseIrisPreviewLocked(XmlPullParser parser)
        throws IOException, XmlPullParserException {

        String pos_x = parser.getAttributeValue(null, ATTR_PREVIEW_POS_X);
        String pos_y = parser.getAttributeValue(null, ATTR_PREVIEW_POS_Y);
        String previewWidth= parser.getAttributeValue(null, ATTR_PREVIEW_WIDTH);
        String previewHeight = parser.getAttributeValue(null, ATTR_PREVIEW_HEIGHT);

        mPreviewRect = new Rect(
                            Integer.parseInt(pos_x),
                            Integer.parseInt(pos_y),
                            Integer.parseInt(pos_x) + Integer.parseInt(previewWidth),
                            Integer.parseInt(pos_y) + Integer.parseInt(previewHeight));
    }

}
