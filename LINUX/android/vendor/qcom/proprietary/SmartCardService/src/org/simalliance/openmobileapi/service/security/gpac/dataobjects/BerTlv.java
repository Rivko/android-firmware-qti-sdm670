/*
 * Copyright 2012 Giesecke & Devrient GmbH.
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
package org.simalliance.openmobileapi.service.security.gpac.dataobjects;

import java.io.ByteArrayOutputStream;
import java.util.Arrays;


public class BerTlv {

    private byte[] mRawData = null;

    private int mTag = 0;

    private int mValueIndex = 0;
    private int mValueLength = 0;

    public BerTlv( byte[] rawData, int tag, int valueIndex, int valueLength ) {
        mRawData = rawData;
        mTag = tag;
        mValueIndex = valueIndex;
        mValueLength = valueLength;
    }

    public static String toHex(byte[] digest) {
        String digits = "0123456789abcdef";
        StringBuilder sb = new StringBuilder(digest.length * 2);
        for (byte b : digest) {
            int bi = b & 0xff;
            sb.append(digits.charAt(bi >> 4));
            sb.append(digits.charAt(bi & 0xf));
        }
        return sb.toString();
    }

    public static BerTlv decode( byte[] data, int startIndex )
        throws ParserException {
            return BerTlv.decode(data, startIndex, true);
    }

    public static BerTlv decode( byte[] data, int startIndex, boolean containsAllData )
        throws ParserException {

        if( data == null || data.length == 0 ){
            throw new ParserException("No data given!");
        }

        int curIndex = startIndex;
        int tag = 0;

        /* tag */
        if( curIndex < data.length ) {
            int temp = data[curIndex++] & 0xff;
            switch (temp) {
            case 0xff: // tag is in two byte format
            case 0xdf:
                if( curIndex < data.length ) {
                    tag = ((temp & 0xff) << 8) | (data[curIndex++] & 0xff);
                } else {
                    throw new ParserException("Index " + curIndex + " out of range! [0..[" + data.length);
                }
                break;

            default: // tag is in single-byte format
                tag = temp;
                break;
            }
        } else {
            throw new ParserException("Index " + curIndex + " out of range! [0..[" + data.length);
        }

        /* length */
        int length;
        if( curIndex < data.length ) {
            int temp = data[curIndex++] & 0xff;
            if (temp < 0x80) {
                length = temp;
            } else if (temp == 0x81) {
                if( curIndex < data.length ) {
                    length = data[curIndex++] & 0xff;
                    if (length < 0x80) {
                        throw new ParserException("Invalid TLV length encoding!");
                    }
                    if(containsAllData &&
                       data.length < length + curIndex) {
                        throw new ParserException("Not enough data provided!");
                    }
                } else {
                    throw new ParserException("Index " + curIndex + " out of range! [0..[" + data.length);
                }
            } else if (temp == 0x82) {
                if( (curIndex + 1)< data.length ) {
                    length = ((data[curIndex] & 0xff) << 8) | (data[curIndex + 1] & 0xff);
                } else {
                    throw new ParserException("Index out of range! [0..[" + data.length);
                }
                curIndex += 2;
                if (length < 0x100) {
                    throw new ParserException("Invalid TLV length encoding!");
                }
                if (containsAllData &&
                    data.length < length + curIndex) {
                    throw new ParserException("Not enough data provided!");
                }
            } else if (temp == 0x83) {
                if( (curIndex + 2)< data.length ) {
                    length = ((data[curIndex] & 0xff) << 16)
                            | ((data[curIndex + 1] & 0xff) << 8)
                            | (data[curIndex + 2] & 0xff);
                } else {
                    throw new ParserException("Index out of range! [0..[" + data.length);
                }
                curIndex += 3;
                if (length < 0x10000) {
                    throw new ParserException("Invalid TLV length encoding!");
                }
                if (containsAllData &&
                    data.length < length + curIndex) {
                    throw new ParserException("Not enough data provided!");
                }
            } else {
                throw new ParserException("Unsupported TLV length encoding!");
            }
        } else {
            throw new ParserException("Index " + curIndex + " out of range! [0..[" + data.length);
        }
        // create object
        return new BerTlv( data, tag, curIndex, length);
    }

    public void interpret()
        throws ParserException {
        // has to be overwritten in derived classes.
    }

    /**
     * Builds up the TLV into a byte stream.
     *
     * Tags can be encoded as one or two bytes
     *
     * @param stream
     * @throws DO_Exception
     */
    public void build( ByteArrayOutputStream stream )
        throws DO_Exception {

        // put tag into stream
        if( mTag > 0xFF ){
            stream.write(((mTag & 0x0000FF00)>>8));
            stream.write((mTag & 0x000000FF));
        } else {
            stream.write((mTag & 0x000000FF));
        }

        // write length
        encodeLength( mValueLength, stream );

        // write value
        if( mValueLength > 0 ){
            stream.write(mRawData, mValueIndex, mValueLength);
        }
    }

    public int getTag(){
        return mTag;
    }


    public int getValueIndex(){
        return mValueIndex;
    }


    public byte[] getValue(){
        // sanity checks
        if( mRawData == null ||
            mValueLength == 0 ||
            mValueIndex < 0 || mValueIndex > mRawData.length ||
            mValueIndex + mValueLength > mRawData.length )
            return null;

        byte[] data = new byte[mValueLength];

        System.arraycopy(mRawData, mValueIndex, data, 0, mValueLength);

        return data;
    }

    protected byte[] getRawData(){
        return mRawData;
    }

    public int getValueLength() {
        return mValueLength;
    }

    /**
     * Encodes length according to ASN1.
     * Supported are length values up to 3 bytes -> 83 xx yy zz.
     *
     * @param length
     * @param stream
     */
    public static void encodeLength( int length, ByteArrayOutputStream stream){

        if (length > 0x0000FFFF ) {
            stream.write(0x83);
            stream.write(((length & 0x00FF0000)>>16));
            stream.write(((length & 0x0000FF00)>>8));
            stream.write((length & 0x000000FF));
        } else if( length > 0x000000FF){
            stream.write(0x82);
            stream.write(((length & 0x0000FF00)>>8));
            stream.write((length & 0x000000FF));
        } else if( length > 0x0000007F){
            stream.write(0x81);
            stream.write((length & 0x000000FF));
        } else {
            stream.write((length & 0x000000FF));
        }
    }

    @Override
    public boolean equals(Object obj){
        boolean equals = false;

        if( obj instanceof BerTlv ){
            BerTlv berTlv = (BerTlv)obj;

            equals = this.mTag == berTlv.mTag;

            if(equals ){
                byte[] test1 = this.getValue();
                byte[] test2 = berTlv.getValue();

                if( test1 != null ){
                    //equals &= test1.equals(test2);
                    equals &= Arrays.equals(test1, test2);
                } else if( test1 == null && test2 == null ){
                    equals &= true;
                }
            }
        }
        return equals;
    }
}
