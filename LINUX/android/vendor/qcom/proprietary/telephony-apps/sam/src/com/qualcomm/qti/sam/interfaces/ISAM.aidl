/******************************************************************************
 * @file    ISAM.aidl
 * @brief   This interface describes the APIs for the SAM service which
            allows clients to retrieve SAM card information and send
            SAM card related requests to modem.
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
import com.qualcomm.qti.sam.interfaces.CardATR;
import com.qualcomm.qti.sam.interfaces.SlotMap;
import com.qualcomm.qti.sam.interfaces.ISAMUnsolicitedCallback;
import com.qualcomm.qti.sam.interfaces.ISAMResponseCallback;

/**
 * Interface used to interact with the SAM service
 * {@hide}
 */
interface ISAM {

    /**
     * registerUnsolicitedResponseCallback will be used by
     * SAM client to register unsolicited response callback
     * to be notified asynchronously
     *
     * @param callback
     *    Defines the callback interface
     *
     * @return
     *    SUCCESS 0
     *       Successfully registerd callback with SAM service.
     *    INTERNAL_FAILURE 8
     *       Failed to register callback due to internal error.
     */
    int registerUnsolicitedResponseCallback(in ISAMUnsolicitedCallback callback);

    /**
     * deregisterUnsolicitedResponseCallback will be used by SAM client
     * to deregister unsolicited callback
     *
     * @param callback
     *    Defines the callback interface
     *
     * @return
     *    SUCCESS 0
     *       Successfully deregisterd callback with SAM service.

     *    INTERNAL_FAILURE 8
     *       Failed to deregister callback due to internal error.
     */
    int deregisterUnsolicitedResponseCallback(in ISAMUnsolicitedCallback callback);

    /**
    * Get Card State.
    * @param - slotId - SAM card slot id
    *        - state - (output variabe) SAM card state
    * @return status of query on SAM card state
    */
    int getCardState(int slotId, out CardState cardState);

    /**
    * Get Card ATR.
    * @param - slotId - SAM card slot id
    *        - cardATR - (output variabe) SAM card ATR
    * @return status of query on SAM card ATR
    */
    int getCardATR(int slotId, out CardATR cardATR);

    /**
    * Send APDU command.
    * @param - slotId - SAM card slot id
    *       Following fields are used to derive the APDU ("command" and "length"
    *          values in TS 27.007 +CSIM and +CGLA commands).
    *        - class_type - class type
    *        - instruction - instruction
    *        - param1 - parameter 1
    *        - param2 - parameter 2
    *        - param3 - parameter 3
    *        - dataCmd - command
    *        - callback - callback to be called asynchronously.
    * @return intermediate status
    */
    int streamApdu(int slotId, int class_type, int instruction, int param1, int param2, int param3, String dataCmd, ISAMResponseCallback callback);

    /**
    * Switch Slots.
    * @param - slotMap - slot mapping information
    *
    *        - callback - callback to be called asynchronously.
    * @return intermediate status
    */
    int switchSlot(in SlotMap slotMap, ISAMResponseCallback callback);

    /**
    * Get slot mapping.
    * @param - slotMap - slot mapping information
    * @return status of the request
    */
    int getSlotMapping(out SlotMap slotMap);

    /**
    * Card power up request.
    * @param - slotId - SAM card slot id
    *        - callback - callback to be called asynchronously.
    *
    * @return intermediate status
    */
    int cardPowerUp(int slotId, ISAMResponseCallback callback);

    /**
    * Card power down request.
    * @param - slotId - SAM card slot id
    *        - callback - callback to be called asynchronously.
    *
    * @return intermediate status
    */
    int cardPowerDown(int slotId, ISAMResponseCallback callback);
}
