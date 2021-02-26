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
import org.simalliance.openmobileapi.internal.ByteArrayConverter;
import org.simalliance.openmobileapi.internal.Util;
import org.simalliance.openmobileapi.service.security.arf.DERParser;
import org.simalliance.openmobileapi.service.security.arf.SecureElement;
import org.simalliance.openmobileapi.service.security.arf.SecureElementException;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.PKCS15Exception;

import android.util.Log;

/**
 * EF_ODF related features
 ***************************************************/
public class EFODF extends EF {

    public static final String TAG = "SmartcardService ACE ARF";
    // Standardized ID for EF_ODF file
    public static final byte[] EFODF_PATH = { 0x50,0x31 };
    byte[] ret = null;
    byte[] mCDFPath = null;
    /**
     * Decodes EF_ODF file
     * @param buffer ASN.1 data
     * @return Path to "EF_DODF" from "DODF Tag" entry;
     *             <code>null</code> otherwise
     */
    private byte[] decodeDER(byte[] buffer)
    throws PKCS15Exception {
        DERParser DER=new DERParser(buffer);
        while(!DER.isEndofBuffer()) {
            if (DER.parseTLV()==(byte)0xA5) { //CDF
                mCDFPath = DER.parsePathAttributes();
                Log.i(TAG, "ODF content found mCDFPath :" + ByteArrayConverter.byteArrayToHexString(mCDFPath));
            } else DER.skipTLVData();
        }

        DERParser DER2=new DERParser(buffer);
        while(!DER2.isEndofBuffer()) {
            if (DER2.parseTLV()==(byte)0xA7)  { // DODF
                return DER2.parsePathAttributes();
            } else DER2.skipTLVData();
        } return null;
    }

    public byte[] getCDFPath() {
        return mCDFPath;
    }

    /**
     * Constructor
     * @param secureElement SE on which ISO7816 commands are applied
     */
    public EFODF(SecureElement handle) {
        super(handle);
    }

    /**
     * Selects and Analyses EF_ODF file
     * @return Path to "EF_DODF" from "DODF Tag" entry;
     *             <code>null</code> otherwise
     */
    public byte[] analyseFile( byte[] pkcs15Path )  throws PKCS15Exception,SecureElementException {
        Log.i(TAG,"Analysing EF_ODF...");


        // 2012-04-12
        // extend path if ODF path was determined from EF DIR.
        byte[] path = null;
        if( pkcs15Path != null ){
            path = new byte[pkcs15Path.length + EFODF_PATH.length];
            System.arraycopy(pkcs15Path, 0, path, 0, pkcs15Path.length);
            System.arraycopy(EFODF_PATH, 0, path, pkcs15Path.length, EFODF_PATH.length );
        } else {
            path = EFODF_PATH;
        }
        //---

        if (selectFile(path)!= APDU_SUCCESS) {
            //let's give it another try..
            try {
                synchronized(this) {
                    wait(1000);
                }
            } catch (InterruptedException e) {
                Log.e(TAG, "Interupted while waiting for EF_ODF : " + e);
            }
            if (selectFile(path)!= APDU_SUCCESS)
                throw new PKCS15Exception("EF_ODF not found!!");
        }
        return decodeDER(readBinary(0,Util.END));
    }

}
