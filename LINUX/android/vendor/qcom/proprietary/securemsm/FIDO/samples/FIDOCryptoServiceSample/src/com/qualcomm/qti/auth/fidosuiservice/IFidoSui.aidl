/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidosuiservice;

import com.qualcomm.qti.auth.fidosuiservice.IFidoSuiListener;
import com.qualcomm.qti.auth.fidosuiservice.IFidoSuiController;

interface IFidoSui {
  IFidoSuiController showMessageSui(
    in String secAppName,
    in String appId,
    in byte[] nonce,
    in int layoutId,
    in byte[] content,
    in IFidoSuiListener listener);
}
