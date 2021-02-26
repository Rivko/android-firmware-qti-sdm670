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

import android.util.Log;
import java.util.Arrays;

import org.simalliance.openmobileapi.service.security.arf.ASN1;
import org.simalliance.openmobileapi.service.security.arf.DERParser;
import org.simalliance.openmobileapi.service.security.arf.SecureElement;
import org.simalliance.openmobileapi.service.security.arf.SecureElementException;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.PKCS15Exception;

/**
 * EF_DIR related features
 ***************************************************/
public class EFDIR extends EF{

    public static final String TAG = "ACE ARF EF_Dir";
    // Standardized ID for EF_DIR file
    public static final byte[] EFDIR_PATH = { 0x3F,0x00,0x2F,0x00 };


    /**
     * Decodes EF_DIR file
     * @param buffer ASN.1 data
     * @param AID Record key to search for
     * @return Path to "EF_ODF" when an expected record is found;
     *             <code>null</code> otherwise
     */
    private byte[] decodeDER(byte[] buffer,byte[] AID)
    throws PKCS15Exception {
        DERParser DER=new DERParser(buffer);
        DER.parseTLV(ASN1.TAG_ApplTemplate);
        // Application Identifier
        DER.parseTLV(ASN1.TAG_ApplIdentifier);
        if (!Arrays.equals(DER.getTLVData(),AID))
            return null; // Record for another AID

        // Application Label or Application Path
        byte objectType=DER.parseTLV();
        if (objectType==ASN1.TAG_ApplLabel) {
            // Application Label [Optional]
            DER.getTLVData();
            DER.parseTLV(ASN1.TAG_ApplPath);
        } else if (objectType!=ASN1.TAG_ApplPath)
                     throw new PKCS15Exception("[Parser] Application Tag expected");
        // Application Path
        return DER.getTLVData();
    }


    /**
     * Constructor
     * @param secureElement SE on which ISO7816 commands are applied
     */
    public EFDIR(SecureElement handle) {
        super(handle);
    }

    /**
     * Analyses DIR file and lookups for AID record
     * @param AID Record key to search for
     * @return Path to "EF_ODF" when an expected record is found;
     *             <code>null</code> otherwise
     */
    public byte[] lookupAID(byte[] AID) throws PKCS15Exception,SecureElementException {
        Log.i(TAG,"Analysing EF_DIR...");

        if (selectFile(EFDIR_PATH)!= APDU_SUCCESS)
            throw new PKCS15Exception("EF_DIR not found!!");

        byte[] data,ODFPath=null;
        short index=1;
        while(index<=getFileNbRecords()) {
            data=readRecord(index++);
            if ((ODFPath=decodeDER(data,AID))!=null)
                break;
        }
        return ODFPath;
    }

}
