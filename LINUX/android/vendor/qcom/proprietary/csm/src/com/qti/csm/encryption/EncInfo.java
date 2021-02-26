/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.encryption;

import java.io.Serializable;

public class EncInfo implements Serializable {

    private static final long serialVersionUID = 1L;
    final String version = "1.0";
    final String magic = "EncryptOption";
    int size;
    String fileName;

    @Override
    public String toString() {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append("version=" + version + "\n");
        stringBuilder.append("fileName=" + fileName + "\n");
        stringBuilder.append("size=" + size);
        return stringBuilder.toString();
    }
}