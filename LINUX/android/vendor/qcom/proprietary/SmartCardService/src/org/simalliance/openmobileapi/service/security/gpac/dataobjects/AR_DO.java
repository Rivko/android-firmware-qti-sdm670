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


/**
 * This class represents the Access rule data object (AR-DO), according
 * to GP Secure Element Control Access.
 *
 * The AR-DO contains one or two access rules of type APDU or NFC.
 *
 *
 *
 */
public class AR_DO extends BerTlv{

    public final static int _TAG = 0xE3;

    private APDU_AR_DO mApduAr = null;
    private NFC_AR_DO  mNfcAr = null;

    public AR_DO(byte[] rawData, int valueIndex, int valueLength) {
        super(rawData, _TAG, valueIndex, valueLength);
    }

    public AR_DO( APDU_AR_DO apdu_ar_do, NFC_AR_DO nfc_ar_do ){
        super( null, _TAG, 0, 0);
        mApduAr = apdu_ar_do;
        mNfcAr = nfc_ar_do;
    }

    public APDU_AR_DO getApduArDo(){
        return mApduAr;
    }

    public NFC_AR_DO getNfcArDo(){
        return mNfcAr;
    }

    @Override
    /**
     * Interpret value.
     *
     * Tag: E3
     *
     * Value:
     * Value can contain APDU-AR-DO or NFC-AR-DO or APDU-AR-DO | NFC-AR-DO
     * A concatenation of one or two AR-DO(s). If two AR-DO(s) are present
     * these must have different types.
     */
    public void interpret()
        throws ParserException {

        this.mApduAr = null;
        this.mNfcAr = null;

        byte[] data = getRawData();
        int index = getValueIndex();

        if( index + getValueLength() > data.length){
            throw new ParserException( "Not enough data for AR_DO!");
        }

        do {
            BerTlv temp = BerTlv.decode(data, index);

            if( temp.getTag() == APDU_AR_DO._TAG ) { // APDU-AR-DO
                mApduAr = new APDU_AR_DO( data, temp.getValueIndex(), temp.getValueLength());
                mApduAr.interpret();
            } else if( temp.getTag() == NFC_AR_DO._TAG ) { // NFC-AR-DO
                mNfcAr = new NFC_AR_DO( data, temp.getValueIndex(), temp.getValueLength());
                mNfcAr.interpret();
            } else {
                // un-comment following line if a more restrictive
                // behavior is necessary.
                //throw new ParserException("Invalid DO in AR-DO!");
            }
            index = temp.getValueIndex() + temp.getValueLength();
        } while ( getValueIndex() + getValueLength() > index  );

        if( mApduAr == null && mNfcAr == null ){
            throw new ParserException("No valid DO in AR-DO!");
        }
    }

    @Override
    /**
     * Interpret value.
     *
     * Tag: E3
     *
     * Value:
     * Value can contain APDU-AR-DO or NFC-AR-DO or APDU-AR-DO | NFC-AR-DO
     * A concatenation of one or two AR-DO(s). If two AR-DO(s) are present
     * these must have different types.
     */
    public void build( ByteArrayOutputStream stream )
        throws DO_Exception {

        // write tag
        stream.write(getTag());

        ByteArrayOutputStream temp = new ByteArrayOutputStream();
        if( mApduAr != null ){
            mApduAr.build(temp);
        }

        if( mNfcAr != null ){
            mNfcAr.build(temp);
        }

        BerTlv.encodeLength(temp.size(), stream);
        try {
            stream.write(temp.toByteArray());
        } catch (IOException e) {
            throw new DO_Exception("AR-DO Memory IO problem! " +  e.getMessage());
        }
    }
}
