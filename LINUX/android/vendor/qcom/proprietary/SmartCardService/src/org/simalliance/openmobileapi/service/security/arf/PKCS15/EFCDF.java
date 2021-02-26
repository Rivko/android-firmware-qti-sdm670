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
    Copyright (c) 2014, The Linux Foundation. All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:
        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above
          copyright notice, this list of conditions and the following
          disclaimer in the documentation and/or other materials provided
          with the distribution.
        * Neither the name of The Linux Foundation nor the names of its
          contributors may be used to endorse or promote products derived
          from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

package org.simalliance.openmobileapi.service.security.arf.PKCS15;

import org.simalliance.openmobileapi.service.security.arf.ASN1;
import org.simalliance.openmobileapi.service.security.arf.DERParser;
import org.simalliance.openmobileapi.service.security.arf.SecureElement;
import org.simalliance.openmobileapi.service.security.arf.SecureElementException;
import org.simalliance.openmobileapi.internal.Util;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.PKCS15Exception;

import android.util.Log;
import java.util.ArrayList;
import java.util.Arrays;

/**
 * EF_CDF related features
 ***************************************************/
public class EFCDF extends EF {

    public static final String TAG = "SmartcardService EFCDF";
    // Standardized ID for EF_CDF file
    public static final byte[] EFCDF_PATH = { 0x50,0x03 };
    public ArrayList<byte[]> x509Bytes = null;
    private short DerIndex, DerSize  = 0;

    /**
     * Selects and Analyses EF_CDF file
     * stores the Path to "EF_TCF" from "TCF Tag" entry (T=04 L=02)
     * EF_CDF ::= SEQUENCE OF CertificateType
     *
     * After lots of decomposition, this can be reduced to the following
     * ASN.1 structure (simplified - many CommonObjectAttributes and
     * CommonCertificateAttributes elements have been elided - all optional),
     * with only the id in CommonCertificateAttributes being mandatory.
     *
     * The [TAG] value is used to distinguish the TypeAttributes element in a
     * PKCS15Object - which is what this is. The value 0xA1 is used in GCF test
     * but other values would seem to be possible.
     *
     * EF_CDF ::=
     *   SEQUENCE {                              -- SEQUENCE OF CertificateType
     *       SEQUENCE {                          -- CommonObjectAttributes
     *           label       LABEL OPTIONAL,
     *           flags       CommonObjectFlags OPTIONAL,
     *           ...
     *       }
     *       SEQUENCE {                          -- CommonCertificateAttributes
     *           id          OCTET STRING,
     *           ...
     *       }
     *       [TAG]                               -- TypeAttributes Tag
     *       SEQUENCE {                          -- X509CertificateAttributes
     *           SEQUENCE {                      -- Path
     *               path    OCTET STRING,
     *               index   INTEGER OPTIONAL,
     *               length  INTEGER OPTIONAL
     *           }
     *       }
     *   }
     *
     * Approach to parsing is to skip the CommonObjectAttributes and
     * CommonCertificateAttributes SEQUENCES, ignore the TAG value as
     * it is not clear what limits really exist on this and then
     * read path, index (if present) and length (if present).
     *
     * If the parse fails for any reason, a PKCS15 exception is thrown.
     * In most cases this throw occurs within the DERParser instance.
     *
     * Output is an array with: start, length, path (in that order).
     * start  is coded on two bytes (first is zero if required)
     * length is coded on two bytes
     * path   is coded on as many bytes as needed
     */

    public  ArrayList<byte[]> isx509(byte[] buffer) throws PKCS15Exception {
        x509Bytes = new ArrayList<byte[]>();

        /* Parse SEQUENCE OF */
        DERParser ef_cdf = new DERParser(buffer);
        while (!ef_cdf.isEndofBuffer()) {
            ef_cdf.parseTLV(ASN1.TAG_Sequence);

            /* TLV data contains a single CertificateType */
            DERParser certType = new DERParser(ef_cdf.getTLVData());
            certType.parseTLV(ASN1.TAG_Sequence);
            certType.skipTLVData();         /* Skip CommonObjectAttributes */
            certType.parseTLV(ASN1.TAG_Sequence);
            certType.skipTLVData();         /* Skip CommonCertificateAttributes */

            /* Now parse the TypeAttributes Tag - we log this as it is the one
            * potentially changing aspect of the parse and useful in debug.
            */
            byte typeAttrTag = certType.parseTLV();
            Log.i(TAG, String.format("Type Attribute Tag: %02X\n", typeAttrTag));

            /* Parse certificate attributes */
            DERParser certAttr = new DERParser(certType.getTLVData());
            certAttr.parseTLV(ASN1.TAG_Sequence);
            DERParser pathInfo = new DERParser(certAttr.getTLVData());
            pathInfo.parseTLV(ASN1.TAG_Sequence);
            DERParser path = new DERParser(pathInfo.getTLVData());

            /* Parse path, optionally start, optionally size */
            path.parseTLV(ASN1.TAG_OctetString);
            byte[] efPath = path.getTLVData();
            byte[] nstart = new byte[2];
            byte[] nlength = new byte[2];
            byte[] none = {0, 0};
            byte[] tmpBuf = new byte[efPath.length + 4];

            /* Try to obtain start */
            if (!path.isEndofBuffer()) {
                short l = path.parseTLV(ASN1.TAG_Integer);

                /* Zero pad a short start value */
                if (l == 1) {
                    nstart[0] = 0;
                    nstart[1] = path.getTLVData()[0];
                } else {
                    nstart = path.getTLVData();
                }

                /* Try to obtain length */
                if (!path.isEndofBuffer()) {
                    path.parseTLV(ASN1.TAG_ContextSpecPrim0); /* INTEGER */
                    nlength = path.getTLVData();
                } else {
                    PKCS15Exception ex =
                        new PKCS15Exception("[EF-CDF Parser] Path with only one of index and length not allowed\n");
                    throw ex;
                }
            } else {
            /* Neither start nor length available. Note that length without
             * start is illegal under the PKCS##15 spec
             */
                nstart = none;
                nlength = none;
            }

            /* Put result into a single array... */
            System.arraycopy(nstart, 0, tmpBuf, 0, 2);
            System.arraycopy(nlength, 0, tmpBuf, 2, 2);
            System.arraycopy(efPath, 0, tmpBuf, 4, efPath.length);
            x509Bytes.add(tmpBuf);
        }
        return x509Bytes;
    }

    public ArrayList<byte[]> returnCouples (){
        return x509Bytes;
    }

    /**
     * Constructor
     * @param secureElement SE on which ISO7816 commands are applied
     */
    public EFCDF(SecureElement handle) {
        super(handle);
    }

    public ArrayList<byte[]> checkCDF(byte[] cdfPath)  throws PKCS15Exception,SecureElementException {
        Log.i(TAG,"Analysing EF_CDF...");
        byte[] path = null;
        if (cdfPath != null) {
            path = cdfPath;
        } else {
             path = EFCDF_PATH;
        }

        if ( selectFile(path)!= APDU_SUCCESS) {
            Log.i(TAG,"EF_CDF not found!!");
            return null;
        } else {
            return isx509(readBinary(0,Util.END));
        }
    }

}
