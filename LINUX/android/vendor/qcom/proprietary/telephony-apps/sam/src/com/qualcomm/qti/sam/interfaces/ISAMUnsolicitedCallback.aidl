/******************************************************************************
 * @file    ISAMUnsolicitedCallback.aidl
 * @brief   This interface describes the SAM unsolicited Response.
 *
 * @version 00.00.01
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/

package com.qualcomm.qti.sam.interfaces;

import com.qualcomm.qti.sam.interfaces.CardState;

interface ISAMUnsolicitedCallback {
    /**
    * card state update callback.
    * @param - cardState - card state
    * @return none
    */
    oneway void cardStateUpdateIndication(in CardState cardState);
}
