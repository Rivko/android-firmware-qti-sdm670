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

/**
 * REF-DO:
 * The REF-DO contains a reference to uniquely assign
 * or identify an access rule for an SE application (with an AID reference)
 * and for a device application (with a hash reference).
 *
 *
 *
 */
public class REF_DO extends BerTlv {

    public final static int _TAG = 0xE1;

    private AID_REF_DO mAidDo = null;
    private Hash_REF_DO mHashDo = null;

    public REF_DO(byte[] rawData, int valueIndex, int valueLength) {
        super(rawData, _TAG, valueIndex, valueLength);
    }

    public REF_DO(AID_REF_DO aid_ref_do, Hash_REF_DO hash_ref_do ) {
        super(null, _TAG, 0, 0);
        mAidDo = aid_ref_do;
        mHashDo = hash_ref_do;
    }

    @Override
    public String toString(){
        StringBuilder b = new StringBuilder();
        b.append("REF_DO: ");
        if( mAidDo != null ){
            b.append(mAidDo.toString());
            b.append(' ' );
        }
        if( mHashDo != null ){
            b.append(mHashDo.toString());
        }
        return b.toString();
    }


    public AID_REF_DO getAidDo() {
        return mAidDo;
    }

    public Hash_REF_DO getHashDo() {
        return mHashDo;
    }

    /**
     * Interpret data.
     *
     * Tags: E1 -> Length: n
     *
     * Value:
     * AID-REF-DO | Hash-REF-DO: A concatenation of an AID-REF-DO and a Hash-REF-DO.
     *
     * Length:
     * n bytes.
      */
    @Override
    public void interpret()
        throws ParserException {

        mAidDo = null;
        mHashDo = null;

        byte[] data = getRawData();
        int index = getValueIndex();

        if( index + getValueLength() > data.length){
            throw new ParserException( "Not enough data for AR_DO!");
        }

        do {
            BerTlv temp = BerTlv.decode(data, index);

            if( temp.getTag() == AID_REF_DO._TAG || temp.getTag() == AID_REF_DO._TAG_DEFAULT_APPLICATION ) { // AID-REF-DO
                mAidDo = new AID_REF_DO( data, temp.getTag(), temp.getValueIndex(), temp.getValueLength());
                mAidDo.interpret();
            } else if( temp.getTag() == Hash_REF_DO._TAG ) { // Hash-REF-DO
                mHashDo = new Hash_REF_DO( data, temp.getValueIndex(), temp.getValueLength());
                mHashDo.interpret();
            } else {
                // uncomment following line if a more restrictive
                // behaviour is necessary.
                // throw new ParserException("Invalid DO in REF-DO!");
            }
            index = temp.getValueIndex() + temp.getValueLength();
        } while( getValueIndex() + getValueLength() > index );

        // check if there is a AID-REF-DO
        if( mAidDo == null ){
            throw new ParserException("Missing AID-REF-DO in REF-DO!");
        }
        // check if there is a Hash-REF-DO
        if( mHashDo == null ){
            throw new ParserException("Missing Hash-REF-DO in REF-DO!");
        }
    }

    /**
     * Tag: E1
     * Length: n
     * Value:
     * AID-REF-DO | Hash-REF-DO: A concatenation of an AID-REF-DO and a Hash-REF-DO.
     */
    @Override
    public void build(ByteArrayOutputStream stream )
        throws DO_Exception {
        ByteArrayOutputStream temp = new ByteArrayOutputStream();

        if( mAidDo == null || mHashDo == null ){
            throw new DO_Exception( "REF-DO: Required DO missing!");
        }

        mAidDo.build(temp);
        mHashDo.build(temp);

        byte[] data = temp.toByteArray();
        BerTlv tlv = new BerTlv( data, getTag(), 0, data.length );
        tlv.build(stream);
    }

    @Override
    public boolean equals(Object obj ){
        boolean equals = false;
        if( obj instanceof REF_DO ){
            equals = super.equals(obj);
            REF_DO ref_do = (REF_DO)obj;
            if( mAidDo == null && ref_do.mAidDo == null ){
                equals &= true;
            } else if( mAidDo != null && ref_do.mAidDo != null ){
                equals &= mAidDo.equals(ref_do.mAidDo);
            } else {
                equals = false;
            }
            if( mHashDo == null && ref_do.mHashDo == null ){
                equals &= true;
            } else if( mHashDo != null && ref_do.mHashDo != null ){
                equals &= mHashDo.equals(ref_do.mHashDo);
            } else {
                equals = false;
            }
        }
        return equals;
    }

    @Override
    public int hashCode () {
        ByteArrayOutputStream stream = new ByteArrayOutputStream();
        try {
            this.build(stream);
        } catch (DO_Exception e) {
            return 1;
        }
        byte[] data = stream.toByteArray();
        int hash = Arrays.hashCode(data);
        //int hash = data.hashCode();
        return hash;
    }
}
