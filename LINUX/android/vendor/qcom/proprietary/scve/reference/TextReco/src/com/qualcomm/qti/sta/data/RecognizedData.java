/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.data;

import java.util.ArrayList;

/**
 * class to store the recognized data
 */
public class RecognizedData {
    private ArrayList<RecognizedRect> rectArray = new ArrayList<RecognizedRect>();
    private String text = null;
    public final int nativeType;
    public final int id;

    /**
     * Constructor for the class
     *
     * @param dataId
     *            id of the data
     * @param type
     *            recognized data type
     * @param text
     *            recognized text
     */
    public RecognizedData(RecognizedRect rect, int type, int dataId, String text) {
        rectArray.add(rect);
        this.nativeType = type;
        id = dataId;
        this.text = text;
    }

    /**
     * gets the text associated
     *
     * @return text
     */
    public String getText() {
        return text;
    }

    /**
     * gets the enclosed rect
     *
     * @return enclosed rect
     */
    public RecognizedRect getRect() {
        return rectArray.get(0);
    }

    /**
     * gets the enclosed rect
     *
     * @return enclosed rect
     */
    public ArrayList<RecognizedRect> getRectList() {
        return rectArray;
    }

    /**
     * Updated the rect and text for tha data
     *
     * @param rect
     *            associated rect
     */
    public void setRect(RecognizedRect rect) {
        rectArray.clear();
        rectArray.add(rect);
    }

    /**
     * Updated the rect for the data
     *
     * @param rect
     *            associated rect
     */
    public void addRect(RecognizedRect rect) {
        rectArray.add(rect);
    }

    /**
     * Updated the rect and text for tha data
     *
     * @param rect
     *            associated rect
     */
    public void addRect(ArrayList<RecognizedRect> rect) {
        rectArray.addAll(rect);
    }

    /**
     * Updated the rect and text for tha data
     *
     * @param text
     *            associated text
     */
    public void setText(String text) {
        this.text = text;
    }

    /**
     * checks if the text contained by the two reconizedData are same.
     *
     * @param data
     *            data to be inspected
     * @return true if the texts are same, false otherwise
     */
    public boolean isSame(RecognizedData data) {
        return text.equals(data.text);
    }
}
