/******************************************************************************
  @file    SAMUnsolicitedCallback.java
  @brief   callback interface for unsolicted callback. Client code can
           override the same and pass it to SAM Manager.

  ---------------------------------------------------------------------------
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.qti.sam.manager;

import com.qualcomm.qti.sam.interfaces.CardState;
import com.qualcomm.qti.sam.interfaces.ISAMUnsolicitedCallback;

public class SAMUnsolicitedCallback extends ISAMUnsolicitedCallback.Stub {
    /**
    * card state update callback.
    * @param - cardState - card state
    * @return none
    */
    public void cardStateUpdateIndication(CardState cardState) {
    }
}
