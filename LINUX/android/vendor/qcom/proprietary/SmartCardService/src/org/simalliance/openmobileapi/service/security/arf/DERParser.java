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

package org.simalliance.openmobileapi.service.security.arf;

import android.util.Log;
import java.util.Arrays;

import org.simalliance.openmobileapi.service.security.arf.PKCS15.PKCS15Exception;

/**
 * Base class for parsing PKCS#15 files
 ***************************************************/
public class DERParser {

    public static final String TAG = "AccessControl";
    // DER parameters
    private byte[] mDERBuffer;
    private short mDERSize,mDERIndex,mTLVDataSize=0;

    /**
     * Returns "Base 128" encoded integer
     * @return Converted integer
     */
    private int readIntBase128() {
        int value=0;
        // If the MSb is set to 0, it is the last byte
        do {
            value=(value<<7) + (mDERBuffer[mDERIndex] & 0x7F);
        } while ((mDERBuffer[mDERIndex++]&0x80) != 0);
        return value;
    }

    /**
     * Returns size of the TLV encoded value
     * @return Size of the TLV
     */
    private short getTLVSize()
    throws PKCS15Exception {
        int size,TLVSize=0;

        if (isEndofBuffer())
            throw new PKCS15Exception("[Parser] Cannot retreive size");
        // Determine data size
        if ((TLVSize=(mDERBuffer[mDERIndex++] & 0xff))>=128) {
            size=TLVSize-128;
            for(TLVSize=0;size>0;size--) {
                if (!isEndofBuffer())
                    TLVSize=(TLVSize<<8)+(mDERBuffer[mDERIndex++] & 0xff);
                else throw new PKCS15Exception("[Parser] Cannot retreive size");
        }}

        // Check if the buffer contains enough data
        if ((mDERIndex+TLVSize)>mDERSize)
            throw new PKCS15Exception("[Parser] Not enough data");
        return (short)TLVSize;
    }

    /**
     * Returns type of the TLV encoded value
     * @return Type of the TLV
     */
    private byte getTLVType()
    throws PKCS15Exception {
        if (isEndofBuffer())
            throw new PKCS15Exception("[Parser] Cannot retreive type");
        return mDERBuffer[mDERIndex++];
    }


    /**
     * Constructor
     * @param buffer file data
     */
    public DERParser(byte[] buffer)
    throws PKCS15Exception {
        mDERBuffer=buffer;
        mDERIndex=0; mDERSize=0;
        if (mDERBuffer==null) return;
        mDERSize=(short)mDERBuffer.length;
        mTLVDataSize=mDERSize;

        // Remove padding
        if (mDERSize==0) return;
        if (mDERBuffer[mDERIndex]==ASN1.TAG_Padding) {
            mTLVDataSize=0;
            while(++mDERIndex<mDERSize) {
                if (mDERBuffer[mDERIndex]!=ASN1.TAG_Padding)
                    throw new PKCS15Exception("[Parser] Incorrect file format");
    }}}

    /**
     * Determines if we reached the end of the buffer
     * @return True if end of buffer is reached; False otherwise
     */
    public boolean isEndofBuffer()
    throws PKCS15Exception {
        if (mDERIndex==mDERSize) return true;
        if (mDERBuffer[mDERIndex]==ASN1.TAG_Padding) {
            // Remove padding
            while(++mDERIndex<mDERSize) {
            if (mDERBuffer[mDERIndex]!=ASN1.TAG_Padding)
                throw new PKCS15Exception("[Parser] Incorrect file format");
            } return true;
        } return false;
    }

    /**
     * Parses TLV from current index
     * @return Type of TLV structure
     */
    public byte parseTLV()
    throws PKCS15Exception {
        byte type=getTLVType();
        mTLVDataSize=getTLVSize();
        return type;
    }

    /**
     * Parses TLV from current index and check if type is correct
     * @param type Type required
     * @return Length of TLV data structure
     */
    public short parseTLV(byte type)
    throws PKCS15Exception {
        byte typeInBuffer = getTLVType();
        if (typeInBuffer == type) {
            mTLVDataSize = getTLVSize();
        } else {
            Log.e(TAG, "parseTLV expected: " + type + " got:" + typeInBuffer);
            Log.e(TAG, "parseTLV mDERIndex: " + mDERIndex + " mDERSize:" + mDERSize);
            throw new PKCS15Exception("[Parser] Unexpected type");
        }
        return mTLVDataSize;
    }

    /**
     * Skips data of the current TLV structure
     */
    public void skipTLVData() {
        mDERIndex+=mTLVDataSize;
    }

    /**
     * Returns data of the current TLV structure
     * @return Data of current TLV structure
     */
    public byte[] getTLVData() {
        byte[] data=Arrays.copyOfRange(mDERBuffer,mDERIndex,
                                                        mDERIndex+mTLVDataSize);
        mDERIndex+=mTLVDataSize;
        return data;
    }

    /**
     * Takes snaptshot of the current context
     * @return Saved context
     */
    public short[] saveContext() {
        short[] context=new short[2];
        context[0]=mDERIndex; context[1]=mTLVDataSize;
        return context;
    }

    /**
     * Restores a context from a snapshot previously saved
     * @param context Context snapshot
     */
    public void restoreContext(short[] context)
    throws PKCS15Exception {
        if ((context==null)||(context.length!=2))
            throw new PKCS15Exception("[Parser] Invalid context");
        if ((context[0]<0)||(context[0]>mDERSize))
            throw new PKCS15Exception("[Parser] Index out of bound");
        mDERIndex=context[0]; mTLVDataSize=context[1];
    }

    /**
     * Parses standardized OID
     * @return String containing OID
     */
    public String parseOID()
    throws PKCS15Exception {
        if (parseTLV(ASN1.TAG_OID)==0)
            throw new PKCS15Exception("[Parser] OID Length is null");

        int end=mDERIndex+mTLVDataSize;
        StringBuffer oid=new StringBuffer();

        // First subidentifier
        int subid=readIntBase128();
        // The first subidentifier contains the first two OID components
        // X.Y is encoded as (X*40)+Y (0<=X<=2 and 0<=Y<=39 for X=0 or X=1)
        if (subid<=79)
            oid.append(subid/40).append('.').append(subid%40);
        else oid.append("2.").append(subid-80);

        while (mDERIndex< end)
            oid.append('.').append(readIntBase128());
        Log.i(TAG,"Found OID: "+oid.toString());
        return oid.toString();
    }

    /**
     * Parses PKCS#15 path attribute
     * @return Path retreived from the attribute
     */
    public byte[] parsePathAttributes()
    throws PKCS15Exception {
        parseTLV(ASN1.TAG_Sequence);
        parseTLV(ASN1.TAG_OctetString);
        return getTLVData();
    }
}
