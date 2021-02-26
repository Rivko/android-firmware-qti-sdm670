/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidosuiservice;

interface IFidoSuiController {
  // return false if Sui was started and has not been aborted because of an error;
  // return true if Sui was started and has been aborted successfully;
  //        or if Sui was not started;
  boolean abortSui();
}
