/* Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.storagecleaner;

import com.qti.storagecleaner.ICleanerCallBack;
// Declare any non-default types here with import statements

interface IStorageCleanerService {

    void startCleaner(String path, String pattern);
    void registCleanerCallBack(ICleanerCallBack callback);

}
