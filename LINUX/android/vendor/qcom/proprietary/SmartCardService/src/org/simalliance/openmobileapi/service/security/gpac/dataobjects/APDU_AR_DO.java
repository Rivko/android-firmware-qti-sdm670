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
import java.util.ArrayList;


/**
 * APDU-AR-DO:
 * An APDU access rule data object defines an access rule for APDU access.
 * The APDU access can either be restricted by a general rule
 * based on an access is NEVER/ ALWAYS allowed policy or
 * by a specific rule based on APDU filters which defines the range
 * of allowed APDUs more precisely.
 *
 *
 *
 */
public class APDU_AR_DO extends BerTlv {

    public final static int _TAG = 0xD0;

    private boolean mApduAllowed = false;
    private ArrayList<byte[]> mApduHeader = new ArrayList<byte[]>();
    private ArrayList<byte[]> mFilterMask = new ArrayList<byte[]>();

    public APDU_AR_DO(byte[] rawData, int valueIndex, int valueLength) {
        super(rawData, _TAG, valueIndex, valueLength);
    }

    public APDU_AR_DO( boolean allowed ){
        super( null, _TAG, 0, 0);
        mApduAllowed = allowed;
    }

    public APDU_AR_DO( ArrayList<byte[]> apduHeader, ArrayList<byte[]> filterMask ){
        super( null, _TAG, 0, 0);
        mApduHeader = apduHeader;
        mFilterMask = filterMask;
    }

    public boolean isApduAllowed(){
        return mApduAllowed;
    }

    public ArrayList<byte[]> getApduHeaderList(){
        return mApduHeader;
    }

    public ArrayList<byte[]> getFilterMaskList(){
        return mFilterMask;
    }

    @Override
    /**
     * Tag: D0
     * Length: 1 or n*8
     *     1 if value contains a general APDU access rule.
     *     n*8 if value contains a specific APDU access rule.

     * Value:
     * Contains a general APDU access rule:
     *     NEVER (00): APDU access is not allowed
     *  ALWAYS(01): APDU access is allowed
     *  or
     *  contains a specific APDU access rule based on one or more APDU filter(s):
     *  APDU filter: 8 bytes APDU filter mask consists of:
     *  4 bytes APDU header (defines the header of allowed APDUs)
     *  4 bytes APDU mask (bit set defines the bits which shall be considered
     *  for the APDU header comparison)
     *  An APDU filter has to be applied as follows:
     *      if((APDUHeader & FilterMask) == FilterAPDUHeader)
     *                 then allow APDU
     */
    public void interpret()
        throws ParserException {

        mApduAllowed = false;
        mApduHeader.clear();
        mFilterMask.clear();

        byte[] data = getRawData();
        int index = getValueIndex();

        if( index + getValueLength() > data.length){
            throw new ParserException( "Not enough data for APDU_AR_DO!");
        }

        // APDU-AR-DO contains either a flag which allows/disallows APDU communication
        // or
        // it contains APDU filter (APDUHeader | FilterMask) which should have length n*8.
        if( getValueLength() == 1 ){
            mApduAllowed = (data[index] == 0x01);
        } else if( (getValueLength() % 8 == 0) && (getValueLength() != 0) ) {
            mApduAllowed = true;

            for( int i = index; i < index + getValueLength(); i +=8 ){
                byte[] apduHeader = new byte[4];
                byte[] filterMask = new byte[4];

                apduHeader[0] = data[i+0];
                apduHeader[1] = data[i+1];
                apduHeader[2] = data[i+2];
                apduHeader[3] = data[i+3];
                filterMask[0] = data[i+4];
                filterMask[1] = data[i+5];
                filterMask[2] = data[i+6];
                filterMask[3] = data[i+7];

                mApduHeader.add(apduHeader);
                mFilterMask.add(filterMask);
            }
        } else if (getValueLength() == 0) {
            mApduAllowed = false;
        } else  {
            throw new ParserException( "Invalid length of APDU-AR-DO!" );
        }
    }

    @Override
    /**
     * Tag: D0
     * Length: 1 or n*8
     *     1 if value contains a general APDU access rule.
     *     n*8 if value contains a specific APDU access rule.

     * Value:
     * Contains a general APDU access rule:
     *     NEVER (00): APDU access is not allowed
     *  ALWAYS(01): APDU access is allowed
     *  or
     *  contains a specific APDU access rule based on one or more APDU filter(s):
     *  APDU filter: 8 bytes APDU filter mask consists of:
     *  4 bytes APDU header (defines the header of allowed APDUs)
     *  4 bytes APDU mask (bit set defines the bits which shall be considered
     *  for the APDU header comparison)
     *  An APDU filter has to be applied as follows:
     *      if((APDUHeader & FilterMask) == FilterAPDUHeader)
     *                 then allow APDU
     */
    public void build( ByteArrayOutputStream stream )
        throws DO_Exception {

        // APDU header and filter mask has to have the same size
        // even if they are not used (then size() == 0 ).
        if(mApduHeader.size() !=  this.mFilterMask.size()){
            throw new DO_Exception( "APDU filter is invalid");
        }

        // write tag
        stream.write(getTag());

        // check if APDU Flag shall be written
        if( mApduHeader.size() == 0){
            stream.write(0x01);
            stream.write(this.mApduAllowed ? 0x01 : 0x00 );
        } else {
            ByteArrayOutputStream temp = new ByteArrayOutputStream();
            for( int i = 0; i < mApduHeader.size(); i++ ){
                byte[] apduHeader = mApduHeader.get(i);
                byte[] filterMask = mFilterMask.get(i);

                if( apduHeader.length != 4 || filterMask.length != 4 ){
                    throw new DO_Exception("APDU filter is invalid!");
                }

                try {
                    temp.write(apduHeader);
                    temp.write(filterMask);
                } catch (IOException e) {
                    throw new DO_Exception("APDU Filter Memory IO problem! " +  e.getMessage());
                }
            }

            BerTlv.encodeLength(temp.size(), stream);
            try {
                stream.write(temp.toByteArray());
            } catch (IOException e) {
                throw new DO_Exception("APDU Filter Memory IO problem! " +  e.getMessage());
            }
        }
    }
}
