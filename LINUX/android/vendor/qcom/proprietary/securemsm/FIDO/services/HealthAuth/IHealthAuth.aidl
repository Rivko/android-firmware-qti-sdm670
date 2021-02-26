/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.healthauthservice;

import com.qualcomm.qti.auth.healthauthservice.IHealthAuthListener;

/**
 * Interface to encapsulate Device health services that are exposed to clients.
 */
interface IHealthAuth {
    /**
     * Runs Runtime Integrity check on the device and reports the result via IHealthAuthListener callback.
     * nonce - Cryptographic nonce.
     * secAppName - TA App that will sign the final report.
     * iHealthAuthListener - Signed report is delivered to this callback listener. Clients implement the callback.
     */
    int getDeviceHealthReport(in byte[] nonce, in String secAppName, in IHealthAuthListener iHealthAuthListener);
}
