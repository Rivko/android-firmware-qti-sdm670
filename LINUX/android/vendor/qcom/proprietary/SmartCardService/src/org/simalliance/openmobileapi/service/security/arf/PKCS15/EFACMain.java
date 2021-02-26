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
import java.util.Arrays;

import org.simalliance.openmobileapi.service.security.arf.ASN1;
import org.simalliance.openmobileapi.service.security.arf.DERParser;
import org.simalliance.openmobileapi.service.security.arf.SecureElement;
import org.simalliance.openmobileapi.service.security.arf.SecureElementException;
import org.simalliance.openmobileapi.internal.Util;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.PKCS15Exception;

/**
 * EF_ACMain related features
 ***************************************************/
public class EFACMain extends EF {

    public static final String TAG = "ACE ARF EF_ACMain";
    // Length of the "RefreshTag"
    public static final short REFRESH_TAG_LEN=8;

    // "EF Access Control Main" path
    private byte[] mACMainPath=null;

    /**
     * Decodes EF_ACMain file
     * @param buffer ASN.1 data
     * @return Path to "Access Control Rules"
     */
    private byte[] decodeDER(byte[] buffer)
        throws PKCS15Exception
    {
        DERParser DER=new DERParser(buffer);
        DER.parseTLV(ASN1.TAG_Sequence);
        if (DER.parseTLV(ASN1.TAG_OctetString)!=REFRESH_TAG_LEN)
            throw new PKCS15Exception("[Parser] RefreshTag length not valid");

        byte[] refreshTag=DER.getTLVData();
        if (!Arrays.equals(refreshTag,this.mSEHandle.getRefreshTag())) {
            mSEHandle.setRefreshTag(refreshTag);
            return DER.parsePathAttributes();
        }
        return null; // RefreshTag not updated
    }


    /**
     * Constructor
     * @param secureElement SE on which ISO7816 commands are applied
     */
    public EFACMain(SecureElement handle,byte[] path) {
        super(handle);
        mACMainPath=path;
    }

    /**
     * Selects and Analyses EF_ACMain file
     * @return Path to "EF_ACRules" if "RefreshTag" has been updated;
     *             <code>null</code> otherwise
     */
    public byte[] analyseFile()
        throws PKCS15Exception,SecureElementException
    {
        Log.i(TAG,"Analysing EF_ACMain...");
        byte[] path = mACMainPath;

        /*
        // 2012-04-12
        // extend path if ODF path was determined from EF DIR.
        if( mSEHandle.getPKCS15Path() != null ) {
            path = new byte[mSEHandle.getPKCS15Path().length + mACMainPath.length];
            System.arraycopy(mSEHandle.getPKCS15Path(), 0, path, 0, mSEHandle.getPKCS15Path().length);
            System.arraycopy(mACMainPath, 0, path, mSEHandle.getPKCS15Path().length, mACMainPath.length );
        }
        //---
         *
         */

        if ( selectFile(path) != APDU_SUCCESS) {
            throw new PKCS15Exception("EF_ACMain not found!");
        }
        return decodeDER(readBinary(0,Util.END));
    }
}
