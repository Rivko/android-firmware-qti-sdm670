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

import java.util.ArrayList;

/**
 * Response-ALL-AR-DO
 * All access rules stored in the Secure Element have to be returned by the ARA-M
 * after a GET DATA (All) command in the response data field within a Response-ALL-AR-DO.
 * The GET DATA command can also be applied iteratively with subsequent GET DATA (Next) commands
 * if the Response-ALL-AR-DO is too large for the GET DATA (All) command.
 * The length field of the Response-ALL-AR-DO shall always contain the full length
 * of the DOs value to determine on device side if a subsequent GET DATA (Next) command
 * is needed.
 *
 *
 *
 */
public class Response_ALL_AR_DO extends BerTlv {

    public final static int _TAG = 0xFF40;

    private ArrayList<REF_AR_DO> mRefArDos = new ArrayList<REF_AR_DO>();

    public Response_ALL_AR_DO(byte[] rawData, int valueIndex,
            int valueLength) {
        super(rawData, _TAG, valueIndex, valueLength);
    }

    public ArrayList<REF_AR_DO> getRefArDos(){
        return mRefArDos;
    }

    @Override
    /**
     * Tag: FF 40
     *
     * Length: n or 0
     * If n is equal to zero, then there are no rules to fetch.
     *
     * Value:
     * REF-AR-DO 1..n or empty
     * An REF-AR-DO if access rules exist.
     * REF-AR-DOs can occur several times in a concatenated DO chain if several REF-AR-DO exist
     * on the SE.
     * The value is empty if access rules do not exist.
     */
    public void interpret()
        throws ParserException {

        mRefArDos.clear();

        byte[] data = getRawData();
        int index = getValueIndex();

        if( getValueLength() == 0 ){
            // No Access rule available for the requested reference.
            return;
        }

        if( index + getValueLength() > data.length){
            throw new ParserException( "Not enough data for Response_AR_DO!");
        }

        BerTlv temp;
        int currentPos = index;
        int endPos = index + getValueLength();
        do {
            temp = BerTlv.decode(data, currentPos);

            REF_AR_DO tempRefArDo;

            if( temp.getTag() == REF_AR_DO._TAG) { // REF-AR-DO tag
                tempRefArDo = new REF_AR_DO( data, temp.getValueIndex(), temp.getValueLength());
                tempRefArDo.interpret();
                mRefArDos.add(tempRefArDo);
            } else {
                // uncomment following line if a more restrictive
                // behavior is necessary.
                //throw new ParserException("Invalid DO in Response-ALL-AR-DO!");
            }
            // get REF-AR-DOs as long as data is available.
            currentPos = temp.getValueIndex() + temp.getValueLength();
        } while( currentPos < endPos );
    }
}
