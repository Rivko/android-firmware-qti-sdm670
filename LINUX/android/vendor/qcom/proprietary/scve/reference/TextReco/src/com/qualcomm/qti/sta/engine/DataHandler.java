/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.engine;

import java.io.ByteArrayOutputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Iterator;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.YuvImage;
import android.util.Log;

import com.qualcomm.qti.sta.data.ApplicationMessage;
import com.qualcomm.qti.sta.data.DateTextHandler;
import com.qualcomm.qti.sta.data.DateTimeData;
import com.qualcomm.qti.sta.data.DateTimeRange;
import com.qualcomm.qti.sta.data.RecognizedData;
import com.qualcomm.qti.sta.data.RecognizedRect;
import com.qualcomm.qti.sta.data.RecognizedRect.LinesComparisonData;
import com.qualcomm.qti.sta.data.Utils;
import com.qualcomm.qti.sta.engine.JniManager.STAListener;

/**
 * class for managing parsed data
 */
public class DataHandler {
    private static final String LOG_TAG = "DataHandler";

    private static final float SAME_LINE_X_NOISE_PERCENT = 1.3f;
    private static final float SAME_LINE_Y_NOISE_PERCENT = 0.5f;
    private static final float GROUP_LINE_Y_NOISE_PERCENT = 0.85f;
    private static final float GROUP_LINE_HEIGHT_NOISE_PERCENT = 1.4f;
    private static final int MULTILINE_MAX_WORDS = 4;

    private int width = 0;
    private int height = 0;
    private int angle = 0;
    private RectF roi = null;

    private Bitmap sourceBitmap = null;
    private Bitmap previewFrameBitmap = null;
    private DateTextHandler dateTextHandler = null;

    private final ArrayList<RecognizedData> rawDataList = new ArrayList<RecognizedData>();

    /**
     * Sets the source bitmap for latest results
     *
     * @param bitmap
     *            source bitmap
     */
    public void setSourceBitmap(Bitmap bitmap) {
        this.sourceBitmap = bitmap;
    }

    /**
     * resets the parser data
     *
     * @param resetDataId
     *            reset data id
     */
    public void reset(int resetDataId) {
        sourceBitmap = null;
        if (resetDataId > 0) {
            Log.d(LOG_TAG, "Removing data with id: " + resetDataId);
            Iterator<RecognizedData> iterator = rawDataList.iterator();
            while (iterator.hasNext()) {
                RecognizedData data = iterator.next();
                if (data.id == resetDataId) {
                    iterator.remove();
                }
            }
        } else {
            rawDataList.clear();
        }
    }

    /**
     * sets the image information for the preview capture mode
     *
     * @param imageBytes
     *            nv21 image bytes
     * @param width
     *            image width
     * @param height
     *            image height
     * @param angle
     *            image angle
     */
    public void setPreviewFrameData(byte[] imageBytes, int width, int height, int angle, RectF roi) {
        this.width = width;
        this.height = height;
        this.angle = angle;
        this.roi = roi;

        prepareBitmapForFrame(imageBytes);
    }

    /**
     * Adds new data to the existing one
     *
     * @param dataList
     *            new data list
     * @param staListener
     *            sta listener
     * @return true if new data added, false otherwise
     */
    public boolean addNewData(ArrayList<RecognizedData> dataList, STAListener staListener) {
        createRepeatInfoForDates(dataList);
        if (dataList.size() > 1) {
            sortTextsBasedOnLocation(dataList);

            concatenateSameLineTexts(dataList);
            concatenateMultilineTexts(dataList);
            concatenateSameTexts(dataList);

            concatenateSameLineDates(dataList);
            concatenateMultipleLineDates(dataList);
        }
        verifyDates(dataList);
        removeUndesired(dataList);

        // remove duplicates
        if (rawDataList.size() > 0 && dataList.size() > 0) {
            removeDuplicates(rawDataList, dataList);
            if (dataList.size() == 0 && staListener != null) {
                staListener.onSTAError(ApplicationMessage.STA_API_NO_NEW_RECORDS);
                Log.d(LOG_TAG, "all data duplicate");
                return false;
            }
        }
        Log.d(LOG_TAG, "data items added: " + dataList.size());
        rawDataList.addAll(dataList);
        return true;
    }

    private void removeUndesired(ArrayList<RecognizedData> dataList) {
        Iterator<RecognizedData> iterator = dataList.iterator();
        while (iterator.hasNext()) {
            RecognizedData data = iterator.next();
            if(data.getText().length() == 1) {
                Log.d(LOG_TAG, "Removed single character data: " + data.getText());
                iterator.remove();
            }
        }
    }

    private void concatenateSameTexts(ArrayList<RecognizedData> dataList) {
        HashMap<String, Integer> generalTexts = new HashMap<String, Integer>();
        Iterator<RecognizedData> iterator = dataList.iterator();
        while (iterator.hasNext()) {
            RecognizedData data = iterator.next();
            if (FieldType.isGeneralText(data.nativeType)) {
                Integer keyIndex = generalTexts.get(data.getText());
                if (keyIndex != null) {
                    RecognizedData olderData = dataList.get(keyIndex);
                    olderData.addRect(data.getRect());
                    iterator.remove();

                } else {
                    generalTexts.put(data.getText(), dataList.indexOf(data));
                }
            }
        }
    }

    private void concatenateMultilineTexts(ArrayList<RecognizedData> dataList) {
        ArrayList<RecognizedData> generalTexts = new ArrayList<RecognizedData>();
        Iterator<RecognizedData> iterator = dataList.iterator();
        while (iterator.hasNext()) {
            RecognizedData data = iterator.next();
            if (FieldType.isGeneralText(data.nativeType)) {
                generalTexts.add(new RecognizedData(data.getRect(), data.nativeType, data.id, data
                        .getText()));
                iterator.remove();
            }
        }

        if (generalTexts.size() > 1) {
            ArrayList<Integer> secondLineArray = new ArrayList<Integer>();
            int listSize = generalTexts.size();
            for (int i = 0; i < listSize; i++) {
                int secondLineIndex = -1;
                RecognizedData firstLine = generalTexts.get(i);
                for (int j = 0; j < listSize; j++) {
                    if (i != j) {
                        RecognizedData secondLine = generalTexts.get(j);
                        if (areVerticalLinesInReach(firstLine, secondLine)) {
                            secondLineIndex = j;
                            break;
                        }
                    }
                }
                secondLineArray.add(secondLineIndex);
            }
            boolean areAllGrouped = false;
            while (!areAllGrouped) {
                areAllGrouped = true;
                for (int i = 0; i < listSize; i++) {
                    int secondLineIndex = secondLineArray.get(i);
                    if (secondLineIndex != -1) {
                        RecognizedData firstLine = generalTexts.get(i);
                        RecognizedData secondLine = generalTexts.get(secondLineIndex);

                        secondLine.setText(firstLine.getText() + "\n" + secondLine.getText());
                        secondLine.addRect(firstLine.getRectList());

                        firstLine.setText("");
                        secondLineArray.set(i, -1);
                        areAllGrouped = false;
                        break;
                    }
                }
            }
            iterator = generalTexts.iterator();
            while (iterator.hasNext()) {
                RecognizedData data = iterator.next();
                if (data.getText().isEmpty()) {
                    iterator.remove();
                }
            }
        }
        dataList.addAll(generalTexts);
    }

    private static void removeDuplicates(ArrayList<RecognizedData> existingDataList,
            ArrayList<RecognizedData> dataList) {
        Iterator<RecognizedData> iterator = dataList.iterator();
        while (iterator.hasNext()) {
            RecognizedData data = iterator.next();
            for (RecognizedData earlierData : existingDataList) {
                if (earlierData.isSame(data)) {
                    iterator.remove();
                    break;
                }
            }
        }
    }

    private void createRepeatInfoForDates(ArrayList<RecognizedData> dataList) {
        for (RecognizedData data : dataList) {
            if (FieldType.isDateText(data.nativeType)) {
                ((DateTimeData)data).createRepeatInfo(dateTextHandler);
            }
        }
    }

    private void verifyDates(ArrayList<RecognizedData> dataList) {
        for (RecognizedData data : dataList) {
            if (FieldType.isDateText(data.nativeType)) {
                ((DateTimeData)data).verify(dateTextHandler);
            }
        }
    }

    private void concatenateMultipleLineDates(ArrayList<RecognizedData> dataList) {
        DateTimeData parentData = null;
        Iterator<RecognizedData> iterator = dataList.iterator();
        while (iterator.hasNext()) {
            RecognizedData recognizedData = iterator.next();
            if (FieldType.isDateText(recognizedData.nativeType)
                    && ((DateTimeData) recognizedData).isMergable()) {
                DateTimeData data = (DateTimeData) recognizedData;
                if (parentData == null) {
                    parentData = data;
                } else {
                    boolean isMerged = false;
                    if (isVerticallyConnected(parentData, data)) {
                        ArrayList<String> mergedDates = mergeDates(parentData, data);
                        if(mergedDates != null) {
                            parentData.addRect(data.getRectList());
                            parentData.setDates(mergedDates);
                            iterator.remove();
                            isMerged = true;
                        }
                    }
                    if (!isMerged) {
                        parentData = data;
                    }
                }
            }
        }
    }

    private ArrayList<String> mergeDates(DateTimeData data1, DateTimeData data2) {
        if(data1.getDateCount() > 1 && data2.getDateCount() > 1){
            return null;
        }

        if(data1.getDateCount() > 1) {
            DateTimeData temp = data1;
            data1 = data2;
            data2 = temp;
        }

        ArrayList<String> mergedDates = new ArrayList<String>();
        DateTimeRange data1Range = dateTextHandler.parseRange(data1.getDateTime());
        for(int i = 0; i < data2.getDateCount(); i++) {
            DateTimeRange data2Range = dateTextHandler.parseRange(data2.getDateAt(i));
            if(isVerticalDateMergeable(data1Range, data2Range)) {
                if (data2Range.merge(data1Range)) {
                    String mergedDate = data2Range.toSTADateTimeText();
                    mergedDates.add(mergedDate);
                } else {
                    return null;
                }
            }
        }
        return (mergedDates.size() == 0) ? null : mergedDates;
    }

    private boolean isVerticalDateMergeable(DateTimeRange range1, DateTimeRange range2) {
        if (!range1.isHourMinutesNull() && !range2.isHourMinutesNull()) {
            return false;
        }
        if (!range1.isYearMonthDayNull() && !range2.isYearMonthDayNull()) {
            return false;
        }
        return true;
    }

    private void concatenateSameLineDates(ArrayList<RecognizedData> dataList) {
        DateTimeData parentData = null;
        Iterator<RecognizedData> iterator = dataList.iterator();
        while (iterator.hasNext()) {
            RecognizedData recognizedData = iterator.next();
            if (FieldType.isDateText(recognizedData.nativeType)
                    && ((DateTimeData) recognizedData).isMergable()) {
                DateTimeData data = (DateTimeData) recognizedData;
                if (parentData == null) {
                    parentData = data;
                } else {
                    boolean isMerged = false;
                    if (isHorizontallyConnected(parentData, data)) {
                        DateTimeRange parentDTRange = dateTextHandler.parseRange(parentData
                                .getDateTime());
                        DateTimeRange dataDTRange = dateTextHandler.parseRange(data.getDateTime());
                        if (parentDTRange.merge(dataDTRange)) {
                            String mergedDate = parentDTRange.toSTADateTimeText();
                            parentData.addRect(data.getRect());
                            parentData.setDateTime(mergedDate);
                            iterator.remove();
                            isMerged = true;
                        }
                    }
                    if (!isMerged) {
                        parentData = data;
                    }
                }
            }
        }
    }

    private boolean isHorizontallyConnected(RecognizedData parent, RecognizedData child) {
        RecognizedRect parentRect = parent.getRect();
        RecognizedRect childRect = child.getRect();
        float xyDiffAllowed = parentRect.height();
        float maxLimitY = xyDiffAllowed * SAME_LINE_Y_NOISE_PERCENT;

        LinesComparisonData comparison = RecognizedRect.getSameLineComparison(parentRect, childRect);
        return (comparison.dy < maxLimitY) ? true : false;
    }

    private boolean isVerticallyConnected(RecognizedData parent, RecognizedData child) {
        RecognizedRect parentRect = parent.getRect();
        RecognizedRect childRect = child.getRect();

        float maxLimitY = parent.getRect().height() * 2;

        float distanceY = (float) RecognizedRect.getMultiLineDy(parentRect, childRect);
        return (distanceY < maxLimitY) ? true : false;
    }

    private boolean isSameLine(RecognizedData parent, RecognizedData child) {
        float xyDiffAllowed = parent.getRect().height();
        float maxLimitY = xyDiffAllowed * SAME_LINE_Y_NOISE_PERCENT;
        float maxLimitX = xyDiffAllowed * SAME_LINE_X_NOISE_PERCENT;

        LinesComparisonData comparison = RecognizedRect.getSameLineComparison(parent.getRect(), child.getRect());
        if (comparison.dy < maxLimitY && comparison.dx < maxLimitX) {
            Log.d(LOG_TAG, "Hr: group [" + parent.getText() + "<+>" + child.getText() + "]");
            //Log.d(LOG_TAG, "Hr: val [" + parent.getText() + "<+>" + child.getText() + "] distance "
            //        + comparison.dx + "," + comparison.dy + " allowed " + maxLimitX + "," + maxLimitY);
            return true;
        }
        Log.d(LOG_TAG, "Hr: diff [" + parent.getText() + "<+>" + child.getText() + "] distance "
                + comparison.dx + "," + comparison.dy + " allowed " + maxLimitX + "," + maxLimitY);
        return false;
    }

    private void sortTextsBasedOnLocation(ArrayList<RecognizedData> dataList) {
        // sort the list based on y and x

        class DataListComparator implements Comparator<RecognizedData> {
            /**
             * {@inheritDoc}
             */
            @Override
            public int compare(RecognizedData lhs, RecognizedData rhs) {
                LinesComparisonData comparison = RecognizedRect.getSameLineComparison(lhs.getRect(), rhs.getRect());
                //Log.d(LOG_TAG, comparison.toString());
                float maxAllowedY = lhs.getRect().height() * SAME_LINE_Y_NOISE_PERCENT;
                //Log.d(LOG_TAG, "height: " + lhs.getRect().height());
                if (comparison.dy < maxAllowedY) { // same line text
                    if (comparison.horizontallyInOrder) {
                        //Log.d(LOG_TAG, lhs.getText() + " -> " + rhs.getText() + " - horizontally correct");
                        return -1;
                    } else {
                        //Log.d(LOG_TAG, rhs.getText() + " -> " + lhs.getText() + " - changed horizontally incorrect");
                        return 1;
                    }
                } else if (comparison.verticallyInOrder) {
                    //Log.d(LOG_TAG, lhs.getText() + " -> " + rhs.getText() + " - vertically correct");
                    return -1;
                }
                //Log.d(LOG_TAG, rhs.getText() + " -> " + lhs.getText() + " - changed vertically incorrect");
                return 1;
            }
        }
        Collections.sort(dataList, new DataListComparator());
    }

    private boolean areVerticalLinesInReach(RecognizedData firstLine, RecognizedData secondLine) {
        RecognizedRect firstLineRect = firstLine.getRect();
        RecognizedRect secondLineRect = secondLine.getRect();

        LinesComparisonData comparison = RecognizedRect.getMultilineComparison(firstLineRect, secondLineRect);
        if (!comparison.verticallyInOrder) {// not in order
            Log.d(LOG_TAG, "Vt: diff [" + firstLine.getText() + "<+>" + secondLine.getText()
                    + "] wrong order");
            return false;
        }
        float xyDiffAllowed = firstLineRect.height() * GROUP_LINE_Y_NOISE_PERCENT;
        if (comparison.dy >= xyDiffAllowed) {
            Log.d(LOG_TAG, "Vt: diff [" + firstLine.getText() + "<+>" + secondLine.getText()
                    + "] far enough " + comparison.dy + " > " + xyDiffAllowed);
            return false;
        }

        RecognizedRect smallerRect;
        RecognizedRect tallerRect;
        if (firstLineRect.height() > secondLineRect.height()) {
            smallerRect = secondLineRect;
            tallerRect = firstLineRect;
        } else {
            smallerRect = firstLineRect;
            tallerRect = secondLineRect;
        }
        float allowedHeight = smallerRect.height() * GROUP_LINE_HEIGHT_NOISE_PERCENT;
        if (tallerRect.height() > allowedHeight) {
            Log.d(LOG_TAG, "Vt: diff [" + firstLine.getText() + "<+>" + secondLine.getText()
                    + "] heights differ: " + tallerRect.height() + " > " + allowedHeight);
            return false;
        }

        int firstLineSpaces = firstLine.getText().split("\\s+").length;
        int secondLineSpaces = secondLine.getText().split("\\s+").length;
        if (firstLineSpaces < MULTILINE_MAX_WORDS && secondLineSpaces < MULTILINE_MAX_WORDS) {
            Log.d(LOG_TAG, "Vt: diff [" + firstLine.getText() + "<+>" + secondLine.getText()
                    + "] not enough words");
            return false;
        }

        Log.d(LOG_TAG, "Vt: merge [" + firstLine.getText() + "<+>" + secondLine.getText() + "]");
        return true;
    }

    private void concatenateSameLineTexts(ArrayList<RecognizedData> dataList) {
        RecognizedData parentData = null;
        Iterator<RecognizedData> iterator = dataList.iterator();
        while (iterator.hasNext()) {
            RecognizedData data = iterator.next();
            if (FieldType.isGeneralText(data.nativeType)) {
                if (parentData == null) {
                    parentData = data;
                } else {
                    if (isSameLine(parentData, data)) {
                        RecognizedRect newRect = RecognizedRect.getMergedHorizontally(
                                parentData.getRect(), data.getRect());
                        parentData.setRect(newRect);
                        parentData.setText(parentData.getText() + " " + data.getText());
                        iterator.remove();

                    } else {
                        parentData = data;
                    }
                }
            }
        }
    }

    /**
     * gets the parsed out data list
     *
     * @return data list
     */
    public ArrayList<RecognizedData> getDataList() {
        return rawDataList;
    }

    private void prepareBitmapForFrame(byte[] sourceImageBytes) {
        Log.d(LOG_TAG, "Preparing Image start");
        YuvImage yuvimage = new YuvImage(sourceImageBytes, ImageFormat.NV21, width, height, null);
        ByteArrayOutputStream bytesOutStream = new ByteArrayOutputStream();
        yuvimage.compressToJpeg(new Rect(0, 0, width, height), 100, bytesOutStream);
        byte[] dataBytes = bytesOutStream.toByteArray();
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inPreferredConfig = Bitmap.Config.RGB_565;
        Bitmap rawBitmap = BitmapFactory.decodeByteArray(dataBytes, 0, dataBytes.length, options);

        // rotate
        previewFrameBitmap = rawBitmap;
        if (angle > 0) {
            Log.d(LOG_TAG, "create rotated bitmap by; " + angle);
            Matrix matrix = new Matrix();
            matrix.preRotate(angle);
            previewFrameBitmap = Bitmap.createBitmap(rawBitmap, 0, 0, rawBitmap.getWidth(),
                    rawBitmap.getHeight(), matrix, true);

            if(previewFrameBitmap != rawBitmap) {// create bitmap can return same source
                rawBitmap.recycle();
            }
        }

        // crop
        sourceBitmap = Bitmap.createBitmap(previewFrameBitmap, (int) roi.left, (int) roi.top,
                (int) roi.width(), (int) roi.height());
        Log.d(LOG_TAG, "Preparing Image done");

        if (previewFrameBitmap != null && sourceBitmap != previewFrameBitmap) {
            previewFrameBitmap.recycle();
            previewFrameBitmap = null;
        }
    }

    /**
     * Saves the final image to the local storage
     *
     * @param finalImage
     *            final image object
     */
    public void saveFinalImage(String finalImage) {
        if (sourceBitmap == null || finalImage == null) {
            return;
        }
        Utils.saveBitmap(finalImage, sourceBitmap);
    }

    /**
     * get the current stored preview frame
     *
     * @return preview frame
     */
    public Bitmap getPreviewFrameBitmap() {
        return previewFrameBitmap;
    }

    /**
     * Sets the date text handler
     *
     * @param dateTextHandler
     *            date text handler
     */
    public void setDateTextHandler(DateTextHandler dateTextHandler) {
        this.dateTextHandler = dateTextHandler;
    }

    /**
     * updated the text for data specified by index
     *
     * @param index
     *            index in the array list
     * @param newText
     *            new text
     */
    public void updateText(int index, String newText) {
        if (index >= 0 || index < rawDataList.size()) {
            RecognizedData data = rawDataList.get(index);
            data.setText(newText);
        }
    }
}
