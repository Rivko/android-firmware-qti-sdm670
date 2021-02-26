/**
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.loadcarrier;

interface ILoadCarrierService{
    Map getSdcardCarrierList();
    String copyToData(String srcFilePath);
    String downloadToData(String url);
}
