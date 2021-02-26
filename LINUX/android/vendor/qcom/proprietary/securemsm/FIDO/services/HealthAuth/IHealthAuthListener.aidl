/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.healthauthservice;

/**
 * Interface for Listener that clients need implement
 */
oneway interface IHealthAuthListener {
    /**
     * Clients get the signed report/result via this callback
     * nonce - Cryptographic nonce, should be same as what as passed in getDeviceHealthReport().
     * result - Overall result of the operation. 0 if success, non-zero in case of failure.
     * authenticatorName - Name of the authenticator service.
     * encapsulatedReport - Signed device health report (only valid if result is 0), null otherwise.
     */
    void onDeviceHealthReport(in byte[] nonce, int result, String authenticatorName, in byte[] encapsulatedReport);
}
