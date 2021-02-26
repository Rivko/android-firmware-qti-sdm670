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


/**
 * Response-RefreshTag DO
 * The GET DATA (RefreshTag) command has to return a refresh tag indicating changes
 * in the access control data in a RefreshTag DO.
 * This refresh tag is an attribute (8-byte random number) of the ARA-M which is
 * newly generated if the ARA-M detects an update of access control data
 * in the Secure Element.
 *
 *
 *
 */
public class Response_RefreshTag_DO extends BerTlv {

    public final static int _TAG = 0xDF20;

    private long mRefreshTag;
    private byte[] mRefreshTagArray = null;

    public Response_RefreshTag_DO(byte[] rawData, int valueIndex,
            int valueLength) {
        super(rawData, _TAG, valueIndex, valueLength);

    }

    public long getRefreshTag(){
        return mRefreshTag;
    }

    public byte[] getRefreshTagArray(){
        return mRefreshTagArray;
    }

    @Override
    /**
     * Tag: DF 20
     * Length: 8 bytes
     * Value:
     * The RefreshTag is an 8 bytes random number.
     * A new RefreshTag value indicates changes in the access control data
     * stored in the SE.
     */
    public void interpret()
        throws ParserException {

        mRefreshTag = 0;

        if( super.getValueLength() != 8 ){
            throw new ParserException( "Invalid length of RefreshTag DO!" );
        }

        byte[] data = super.getRawData();
        int index = super.getValueIndex();

        if( index + super.getValueLength() > data.length ){
            throw new ParserException( "Not enough data for RefreshTag DO!" );
        }
        mRefreshTagArray = new byte[super.getValueLength()];
        System.arraycopy(data, index, mRefreshTagArray, 0, mRefreshTagArray.length);

        long temp;
        temp = data[index++];
        mRefreshTag =(temp << 56L);
        temp = data[index++];
        mRefreshTag +=(temp << 48L);
        temp = data[index++];
        mRefreshTag +=(temp << 40L);
        temp = data[index++];
        mRefreshTag +=(temp << 32L);
        temp = data[index++];
        mRefreshTag +=(temp << 24L);
        temp = data[index++];
        mRefreshTag +=(temp << 16L);
        temp = data[index++];
        mRefreshTag +=(temp <<  8L);
        temp = data[index++];
        mRefreshTag +=(temp);
    }
}
