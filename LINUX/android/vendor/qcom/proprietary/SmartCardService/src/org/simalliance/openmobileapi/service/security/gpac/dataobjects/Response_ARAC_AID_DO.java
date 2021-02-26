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
 * Response_ARAC_AID_DO
 *
 * A list of AIDs containing an AID for each ARA-C.
 *
 * In response to STORE DATA (Command-Get-ClientAIDs-DO),
 * the ARA-M shall return the AID of each of the ARA-Cs
 * currently registered within a Response-ARAC-AID-DO.
 *
 *
 *
 */
public class Response_ARAC_AID_DO extends BerTlv {

    public final static int _TAG = 0xFF70;

    private ArrayList<AID_REF_DO> mAidDos = new ArrayList<AID_REF_DO>();

    public Response_ARAC_AID_DO(byte[] rawData, int valueIndex,
            int valueLength) {
        super(rawData, _TAG, valueIndex, valueLength);
    }

    public ArrayList<AID_REF_DO> getAidRefDos(){
        return mAidDos;
    }

    @Override
    /**
     * Tag: FF 70
     *
     * Length: n or 0
     * If n is equal to zero, then there are no rules to fetch.
     *
     * Value:
     * AID-REF-DO 1..n or empty
     * AID-REF-DOs can occur several times in a concatenated DO chain if several ARA-C instances exist
     * on the SE.
     * The value is empty if no ARA-C instance exist.
     */
    public void interpret()
        throws ParserException {

        mAidDos.clear();

        byte[] data = getRawData();
        int index = getValueIndex();

        if( getValueLength() == 0 ){
            // No Access rule available for the requested reference.
            return;
        }

        if( index + getValueLength() > data.length){
            throw new ParserException( "Not enough data for Response_ARAC_AID_DO!");
        }

        BerTlv temp;
        int currentPos = index;
        int endPos = index + getValueLength();
        do {
            temp = BerTlv.decode(data, currentPos);

            AID_REF_DO tempAidDo;

            if( temp.getTag() == AID_REF_DO._TAG ||
                    temp.getTag() == AID_REF_DO._TAG_DEFAULT_APPLICATION ) {
                tempAidDo = new AID_REF_DO( data, temp.getTag(), temp.getValueIndex(), temp.getValueLength());
                tempAidDo.interpret();
                mAidDos.add(tempAidDo);
            } else {
                // uncomment following line if a more restrictive
                // behavior is necessary.
                //throw new ParserException("Invalid DO in Response_ARAC_AID_DO!");
            }
            // get AID-REF-DOs as long as data is available.
            currentPos = temp.getValueIndex() + temp.getValueLength();
        } while( currentPos < endPos );
    }
}
