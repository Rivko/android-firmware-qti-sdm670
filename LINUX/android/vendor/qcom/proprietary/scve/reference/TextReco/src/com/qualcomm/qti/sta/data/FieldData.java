/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.data;

import com.qualcomm.qti.sta.engine.FieldType;

/**
 * class for storing contact or event Field information
 */
public class FieldData {
    private FieldType type = FieldType.UNKNOWN;
    private String value = null;

    /**
     * Constructor for the class
     *
     * @param type
     *            field type
     * @param value
     *            field value
     */
    public FieldData(FieldType type, String value) {
        this.type = type;
        this.value = value;
    }

    /**
     * gets the fields value
     *
     * @return field value
     */
    public String getText() {
        return value;
    }

    /**
     * gets the field type
     *
     * @return field type
     */
    public FieldType getType() {
        return type;
    }

    /**
     * Sets the Field value
     *
     * @param value
     *            field value
     */
    public void setValue(String value) {
        this.value = value;
    }

    /**
     * Sets the field type
     *
     * @param fieldType
     *            field type
     */
    public void setType(FieldType fieldType) {
        type = fieldType;
    }
}
