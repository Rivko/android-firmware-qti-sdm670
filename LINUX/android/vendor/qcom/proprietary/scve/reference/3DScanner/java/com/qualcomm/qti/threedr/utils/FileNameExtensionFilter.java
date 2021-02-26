/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.utils;

import java.io.File;
import java.io.FilenameFilter;

public class FileNameExtensionFilter implements FilenameFilter {

    String extension;

    public FileNameExtensionFilter(String extn) {
        this.extension = extn;
    }

    @Override
    public boolean accept(File dir, String filename) {
        if ((new File(dir, filename).isDirectory()) && (filename.endsWith(extension))) {
            return true;
        }
        return false;
    }
}
