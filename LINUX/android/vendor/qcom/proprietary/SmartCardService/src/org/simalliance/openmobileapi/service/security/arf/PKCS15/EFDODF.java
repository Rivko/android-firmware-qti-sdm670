/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc. 
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

package org.simalliance.openmobileapi.service.security.arf.PKCS15;

import org.simalliance.openmobileapi.service.security.arf.ASN1;
import org.simalliance.openmobileapi.service.security.arf.DERParser;
import org.simalliance.openmobileapi.service.security.arf.SecureElement;
import org.simalliance.openmobileapi.service.security.arf.SecureElementException;
import org.simalliance.openmobileapi.internal.Util;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.PKCS15Exception;

import android.util.Log;

/**
 * EF_DODF related features
 ***************************************************/
public class EFDODF extends EF{

    public static final String TAG = "ACE ARF EF_DODF";
    // OID defined by Global Platform for the "Access Control"
    public static final String AC_OID = "1.2.840.114283.200.1.1";

    /**
     * Decodes EF_DODF file
     * @param buffer ASN.1 data
     * @return Path to "Access Control Main" from "Access Control" OID;
     *             <code>null</code> otherwise
     */
    private byte[] decodeDER(byte[] buffer)
    throws PKCS15Exception {
        byte objectType;
        short[] context=null;
        DERParser DER=new DERParser(buffer);

        while(!DER.isEndofBuffer()) {
            if (DER.parseTLV()==(byte)0xA1) { // OidDO Data Object
                // Common Object Attributes
                DER.parseTLV(ASN1.TAG_Sequence);
                DER.skipTLVData();
                // Common Data Object Attributes
                DER.parseTLV(ASN1.TAG_Sequence);
                DER.skipTLVData();

                objectType=DER.parseTLV();
                if (objectType==(byte)0xA0) { // SubClassAttributes [Optional]
                    DER.skipTLVData();
                    objectType=DER.parseTLV();
                }
                if (objectType==(byte)0xA1) { // OidDO
                    DER.parseTLV(ASN1.TAG_Sequence);
                    context=DER.saveContext();
                    if (DER.parseOID().compareTo(AC_OID)!=0) {
                        DER.restoreContext(context);
                        DER.skipTLVData();
                    } else return DER.parsePathAttributes();
                } else throw new PKCS15Exception("[Parser] OID Tag expected");
            } else DER.skipTLVData();
        }
        return null; // No "Access Control" OID found
    }


    /**
     * Constructor
     * @param secureElement SE on which ISO7816 commands are applied
     */
    public EFDODF(SecureElement handle) {
        super(handle);
    }

    /**
     * Selects and Analyses EF_DODF file
     * @param path Path of the "EF_DODF" file
     * @return Path to "EF_ACMain" from "Access Control" OID;
     *             <code>null</code> otherwise
      */
    public byte[] analyseFile(byte[] path)
        throws PKCS15Exception,SecureElementException
    {
        Log.i(TAG,"Analysing EF_DODF...");

        if (selectFile(path)!=APDU_SUCCESS)
            throw new PKCS15Exception("EF_DODF not found!");

        return decodeDER(readBinary(0,Util.END));
    }

}
