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

/*
 * Contributed by: Giesecke & Devrient GmbH.
 */

package org.simalliance.openmobileapi.service.security.arf.PKCS15;

import android.util.Log;

import java.util.HashMap;
import java.util.Map;

import org.simalliance.openmobileapi.internal.Util;
import org.simalliance.openmobileapi.service.security.arf.ASN1;
import org.simalliance.openmobileapi.service.security.arf.DERParser;
import org.simalliance.openmobileapi.service.security.arf.SecureElement;
import org.simalliance.openmobileapi.service.security.arf.SecureElementException;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.EFACConditions;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.PKCS15Exception;
import org.simalliance.openmobileapi.service.security.gpac.dataobjects.AID_REF_DO;

/**
 * EF_ACRules related features
 ***************************************************/
public class EFACRules extends EF {

    public static final String TAG = "ACE ARF EF_ACRules";
    // AID used to store rules for default application
    public static final byte[] DEFAULT_APP = new byte[0];

    protected Map<String, byte[]> mAcConditionDataCache = new HashMap<String, byte[]>();


    /**
     * Decodes EF_ACRules file
     * @param buffer ASN.1 data
     */
    private void decodeDER(byte[] buffer)
        throws PKCS15Exception
    {
        byte[] AID=null;
        DERParser DER=new DERParser(buffer);

        // mapping to GPAC data objects
        int tag = 0;

        while(!DER.isEndofBuffer()) {
            DER.parseTLV(ASN1.TAG_Sequence);
            switch(DER.parseTLV()) {
            case (byte)0xA0: // Restricted AID
                DER.parseTLV(ASN1.TAG_OctetString);
                AID=DER.getTLVData();
                tag = AID_REF_DO._TAG;
                break;
            case (byte)0x81: // Rules for default Application
                AID=null;
                tag = AID_REF_DO._TAG_DEFAULT_APPLICATION;
                break;
            case (byte)0x82: // Rules for default case
                AID=DEFAULT_APP;
                tag = AID_REF_DO._TAG;
                break;
            default:
                throw new PKCS15Exception("[Parser] Unexpected ACRules entry");
            }
            byte[] path = DER.parsePathAttributes();

            // 2012-09-04
            // optimization of reading EF ACCondition
            if( path != null  ){
                String pathString = Util.bytesToString(path);
                EFACConditions temp = new EFACConditions(mSEHandle,new AID_REF_DO(tag, AID ));
                // check if EF was already read before
                if( this.mAcConditionDataCache.containsKey(pathString )){
                    // yes, then reuse data
                    temp.addRestrictedHashesFromData(this.mAcConditionDataCache.get(pathString));
                } else {
                    // no, read EF and add to rules cache
                    temp.addRestrictedHashes(path);
                    if( temp.getData() != null ){
                        // if data are read the put it into cache.
                        this.mAcConditionDataCache.put(pathString, temp.getData());
                    }
                }
            }
        }
    }

    /**
     * Constructor
     * @param secureElement SE on which ISO7816 commands are applied
     */
    public EFACRules(SecureElement handle) {
        super( handle );
    }

    /**
     * Selects and Analyses EF_ACRules file
     * @param path Path of the "EF_ACRules" file
     */
    public void analyseFile(byte[] path)
        throws PKCS15Exception,SecureElementException {

        Log.i(TAG,"Analysing EF_ACRules...");

        // clear EF AC Condition data cache.
        mAcConditionDataCache.clear();

        if ( selectFile(path)!= APDU_SUCCESS)
            throw new PKCS15Exception("EF_ACRules not found!!");

        try {
            decodeDER( readBinary(0,Util.END));
        } catch(PKCS15Exception e) {
            throw e;
    }}
}
