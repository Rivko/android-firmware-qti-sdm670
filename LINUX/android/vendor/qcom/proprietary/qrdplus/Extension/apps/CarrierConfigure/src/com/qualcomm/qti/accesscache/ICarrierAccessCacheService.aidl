/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.accesscache;

interface ICarrierAccessCacheService{
    boolean writeActionFile(String content);
    boolean writeSubMaskFile(String content);
    boolean writeCommandFile(String filePath, String locale, boolean wipedata);
    String readSubMaskFile();
    String copyFile(String srcPath);
    Map getCacheCarrierList();
    String copyToData(String srcFilePath);
    int getMBNFileDataFromOta(String srcFilePath, String fileName, inout byte[] fileData,
            int fileMaxSize);
}
