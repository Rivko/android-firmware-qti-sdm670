/******************************************************************************
 * @file    KsNafResponse.aidl
 * @brief   Response object for gbaInit API defined in the QtiTelephonyService
 *
 * @version 1.0
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/

package com.qualcomm.qti.telephonyservice;

    /**
     * Response object for gbaInit API defined in the QtiTelephonyService.
     *
     * @return
     *     Generic Bootstrapping Architecture NAF type.
     * public int getType()
     *
     * @return
     *     KS NAF in Base64 format. See 3GPP TS 33.220 4.5.2.
     * public byte[] getResponse()
     *
     * @return
     *     Bootstrapping transaction identifier.
     * public String getBootstrapTransactionId()
     *
     * @return
     *     KS lifetime in UTC format. Tailing "Z" indicates GMT.
     *     See ISO 8601.
     * public String getLifetime()
     *
     */
parcelable KsNafResponse;