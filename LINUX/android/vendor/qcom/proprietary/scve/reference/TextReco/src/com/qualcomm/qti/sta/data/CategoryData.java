/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.data;

import java.util.ArrayList;

/**
 * Category data
 */
public class CategoryData {
    public final ArrayList<FieldData> data;
    public final int dataId;

    /**
     * Constructor for the class
     *
     * @param data
     *            fields data list
     * @param dataId
     *            data id
     */
    public CategoryData(ArrayList<FieldData> data, int dataId) {
        this.data = data;
        this.dataId = dataId;
    }
}
