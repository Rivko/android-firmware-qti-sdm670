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
import java.io.IOException;
import java.util.Arrays;

/**
 * Hash-REF-DO:
 * The Hash-REF-DO is used for retrieving and storing
 * the corresponding access rules for a device application
 * (which is identified by the hash value of its certificate)
 * from and to the ARA
 *
 *
 *
 */
public class Hash_REF_DO extends BerTlv {

    public final static int _TAG = 0xC1;
    public final static int _SHA1_LEN = 20;

    private byte[] mHash = null;

    public Hash_REF_DO(byte[] rawData, int valueIndex, int valueLength){
        super(rawData, _TAG, valueIndex, valueLength);
    }

    public Hash_REF_DO(byte[] hash){
        super(hash, _TAG, 0, (hash == null ? 0 : hash.length));
        mHash = hash;
    }

    public Hash_REF_DO(){
        super(null, _TAG, 0, 0);
        mHash = null;
    }

    public byte[] getHash(){
        return mHash;
    }

    @Override
    public String toString(){
        StringBuilder b = new StringBuilder();
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        b.append("Hash_REF_DO: ");
        try {
            this.build(out);
            b.append(BerTlv.toHex(out.toByteArray()));
        } catch (Exception e ){
            b.append(e.getLocalizedMessage());
        }
        return b.toString();
    }

    /**
     * Tags: C1
     * Length: 0 or _SHA1_LEN bytes
     *
     * Value:
     * Hash: identifies a specific device application
     * Empty: refers to all device applications
     *
     * Length:
     * _SHA1_LEN for 20 bytes SHA-1 hash value
     * 0 for empty value field
      */
    @Override
    public void interpret()
        throws ParserException {

        mHash = null;

        byte[] data = getRawData();
        int index = getValueIndex();

        // sanity checks
        if( getValueLength() != 0 && getValueLength() != _SHA1_LEN ) {
            throw new ParserException("Invalid value length for Hash-REF-DO!");
        }

        if( getValueLength() == _SHA1_LEN ) {
            if( index + getValueLength() > data.length){
                throw new ParserException( "Not enough data for Hash-REF-DO!");
            }

            mHash = new byte[getValueLength()];
            System.arraycopy(data, index, mHash, 0, getValueLength());
        }
    }

    /**
     * Tags: C1
     * Length: 0 or 20 bytes
     *
     * Value:
     * Hash: identifies a specific device application
     * Empty: refers to all device applications
     *
     * Length:
     * _SHA1_LEN for 20 bytes SHA-1 hash value
     * 0 for empty value field
      */
    @Override
    public void build( ByteArrayOutputStream stream)
        throws DO_Exception {

        // sanity checks
        if( mHash != null &&
            !(mHash.length != _SHA1_LEN || mHash.length != 0) ) {
            throw new DO_Exception("Hash value must be " + _SHA1_LEN + " bytes in length!");
        }

        stream.write(getTag());

        if( mHash == null ) {
            stream.write(0x00);
        } else {
            try {
                stream.write(mHash.length);
                stream.write(mHash);
            } catch( IOException ioe ){
                throw new DO_Exception("Hash could not be written!");
            }
        }
    }

    @Override
    public boolean equals( Object obj ){
        boolean equals = false;

        if( obj instanceof Hash_REF_DO ){
            equals = super.equals(obj);

            if( equals ){
                Hash_REF_DO hash_ref_do = (Hash_REF_DO)obj;
                if( this.mHash == null && hash_ref_do.mHash == null )
                    equals &= true;
                else {
                    if( this.mHash == null && hash_ref_do.mHash != null ){
                        equals &= (hash_ref_do.mHash.length == 0);
                    } else if( this.mHash != null && hash_ref_do.mHash == null ){
                        equals &= (this.mHash.length == 0);
                    } else {
                        //equals &= this.mHash.equals(hash_ref_do.mHash);
                        equals &= Arrays.equals(mHash, hash_ref_do.mHash);
                    }
                }
            }
        }
        return equals;
    }
}
