/******************************************************************************
 * @file    SAMErrors.java
 * @brief   This interface describes the SAM Error codes.
 *
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/

package com.qualcomm.qti.sam.interfaces;

public interface SAMErrors {
    int SUCCESS = 0;
    int RADIO_NOT_AVAILABLE = 1;
    int REQUEST_NOT_SUPPORTED = 2;
    int BUSY = 3;
    int SIM_ABSENT = 4;
    int INVALID_PARAMETER = 5;
    int INIT_FAILURE = 6;
    int SERVICE_NOT_CONNECTED = 7;
    int INTERNAL_FAILURE = 8;
}

