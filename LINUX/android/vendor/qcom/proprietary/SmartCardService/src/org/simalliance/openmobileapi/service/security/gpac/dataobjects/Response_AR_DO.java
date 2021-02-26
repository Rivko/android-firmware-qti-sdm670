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
 * Response-AR-DO
 * If access rules can be found in the Secure Element which corresponds to the specified AR-DO
 * in the GET DATA (Specific) command these must be returned by the ARA-M
 * in the response data field within a Response-AR-DO.
 * The GET DATA command can also be applied iteratively with subsequent GET DATA (next) commands
 * if the Response-AR-DO is too large for the GET DATA (Specific) command.
 * The length field of the Response-AR-DO shall always contain the full length of the DOs value
 * to determine on device side if a subsequent GET DATA (Next) command is needed.
 *
 *
 *
 */
public class Response_AR_DO extends BerTlv {

    public final static int _TAG = 0xFF50;

    private AR_DO mArDo = null;

    public Response_AR_DO(byte[] rawData, int valueIndex,
            int valueLength) {
        super(rawData, _TAG, valueIndex, valueLength);
    }

    public AR_DO getArDo(){
        return mArDo;
    }

    @Override
    /**
     * Tag: FF 50
     *
     * Length: n or 0
     * If n is equal to zero, then there are no rules to fetch.
     *
     * Value:
     * An AR-DO if the referenced access rules exist.
     * The value is empty if access rules do not exist to the defined reference
     */
    public void interpret()
        throws ParserException {

        byte[] data = getRawData();
        int index = getValueIndex();

        if( getValueLength() == 0 ){
            // No Access rule available for the requested reference.
            return;
        }

        if( index + getValueLength() > data.length){
            throw new ParserException( "Not enough data for Response_AR_DO!");
        }

        int currentPos = index;
        int endPos = index + getValueLength();
        do {
            BerTlv temp = BerTlv.decode(data, currentPos);

            if( temp.getTag() == AR_DO._TAG) { // AR-DO tag
                mArDo = new AR_DO( data, temp.getValueIndex(), temp.getValueLength());
                mArDo.interpret();
            } else {
                // un-comment following line if a more restrictive
                // behavior is necessary.
                //throw new ParserException("Invalid DO in Response-AR-DO!");
            }
            // get REF-AR-DOs as long as data is available.
            currentPos = temp.getValueIndex() + temp.getValueLength();
        } while( currentPos < endPos );
    }
}
