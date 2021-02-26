/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (C) 2011 Deutsche Telekom, A.G.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.simalliance.openmobileapi.service.security.arf;

/**
 * Defines all tags for parsing PKCS#15 files
 ***************************************************/
public abstract class ASN1 {

    // ASN.1 tags
    public static final byte TAG_Sequence           = 0x30;
    public static final byte TAG_Integer            = 0x02;
    public static final byte TAG_OctetString        = 0x04;
    public static final byte TAG_OID                = 0x06;
    public static final byte TAG_ContextSpecPrim0   = (byte) 0x80;
    public static final byte TAG_ContextSpecPrim1   = (byte) 0x81;
    public static final byte TAG_ContextSpecPrim2   = (byte) 0x82;
    public static final byte TAG_ContextSpecPrim3   = (byte) 0x83;
    public static final byte TAG_ContextSpecPrim4   = (byte) 0x84;
    public static final byte TAG_ContextSpecPrim5   = (byte) 0x85;
    public static final byte TAG_ContextSpecPrim6   = (byte) 0x86;
    public static final byte TAG_ContextSpecPrim7   = (byte) 0x87;
    public static final byte TAG_ContextSpecPrim8   = (byte) 0x88;
    public static final byte TAG_PrivateKey         = (byte) 0xA0;
    public static final byte TAG_PublicKey          = (byte) 0xA1;
    public static final byte TAG_PublicKeyTrusted   = (byte) 0xA2;
    public static final byte TAG_SecretKey          = (byte) 0xA3;
    public static final byte TAG_Certificate        = (byte) 0xA4;
    public static final byte TAG_CertificateTrusted = (byte) 0xA5;
    public static final byte TAG_CertificateUseful  = (byte) 0xA6;
    public static final byte TAG_DataObject         = (byte) 0xA7;
    public static final byte TAG_AuthenticateObject = (byte) 0xA8;

    // EF_DIR tags
    public static final byte TAG_ApplTemplate       = 0x61;
    public static final byte TAG_ApplIdentifier     = 0x4F;
    public static final byte TAG_ApplLabel          = 0x50;
    public static final byte TAG_ApplPath           = 0x51;
    public static final byte TAG_FCP                = 0x62;

    // Others tags
    public static final byte TAG_Padding            = (byte)0xFF;
}
