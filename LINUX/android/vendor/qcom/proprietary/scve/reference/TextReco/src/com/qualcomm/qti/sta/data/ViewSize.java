/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.data;

/**
 * Class for View size data
 */
public class ViewSize {
    public int width;
    public int height;

    /**
     * Default constructor for the class
     */
    public ViewSize() {
        set(0, 0);
    }

    /**
     * Constructor for the class
     *
     * @param wd
     *            width of the size
     * @param ht
     *            height of the size
     */
    public ViewSize(int wd, int ht) {
        set(wd, ht);
    }

    /**
     * sets the width and height
     *
     * @param wd
     *            width of the size
     * @param ht
     *            height of the size
     */
    public void set(int wd, int ht) {
        width = wd;
        height = ht;
    }
}
