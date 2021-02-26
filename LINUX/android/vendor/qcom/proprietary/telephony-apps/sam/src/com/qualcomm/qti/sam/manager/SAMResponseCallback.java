/******************************************************************************
  @file    SAMResponseCallback.java
  @brief   SAM asyncronous response callback definition. Client code can
           override the same and pass it to SAM Manager.

  ---------------------------------------------------------------------------
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.qti.sam.manager;

import com.qualcomm.qti.sam.interfaces.ISAMResponseCallback;

public class SAMResponseCallback extends ISAMResponseCallback.Stub {

    /**
    * APDU response callback.
    * @param - slotId - SAM card slot id
    *        - actionStatus - status of the request
    *        - statusWord1- status word
    *        - statusWord1- status word
    *        - dataRsp - data response
    * @return none
    */
    public void streamApduResponse(int slotId, int actionStatus, int statusWord1, int statusWord2, String dataRsp) {
    }

    /**
    * switch slot response callback.
    * @param - slotId - SAM card slot id
    *        - actionStatus - status of the request
    * @return none
    */
    public void switchSlotResponse(int actionStatus) {
    }

    /**
    * card power up response callback.
    * @param - slotId - SAM card slot id
    *        - actionStatus - status of the request
    * @return none
    */
    public void cardPowerUpResponse(int slotId, int actionStatus) {
    }

    /**
    * card power down callback.
    * @param - slotId - SAM card slot id
    *        - actionStatus - status of the request
    * @return none
    */
    public void cardPowerDownResponse(int slotId, int actionStatus) {
    }
}
