/******************************************************************************
 * @file    ISAMResponseCallback.aidl
 * @brief   This interface describes the SAM asyncronous Responses.
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

interface ISAMResponseCallback {

    /**
    * APDU response callback.
    * @param - slotId - SAM card slot id
    *        - actionStatus - status of the request
    *        - statusWord1- status word
    *        - statusWord1- status word
    *        - dataRsp - data response
    * @return none
    */
    oneway void streamApduResponse(int slotId, int actionStatus, int statusWord1, int statusWord2, String dataRsp);

    /**
    * switch slot response callback.
    * @param - slotId - SAM card slot id
    *        - actionStatus - status of the request
    * @return none
    */
    oneway void switchSlotResponse(int actionStatus);

    /**
    * card power up response callback.
    * @param - slotId - SAM card slot id
    *        - actionStatus - status of the request
    * @return none
    */
    oneway void cardPowerUpResponse(int slotId, int actionStatus);

    /**
    * card power down callback.
    * @param - slotId - SAM card slot id
    *        - actionStatus - status of the request
    * @return none
    */
    oneway void cardPowerDownResponse(int slotId, int actionStatus);
}
