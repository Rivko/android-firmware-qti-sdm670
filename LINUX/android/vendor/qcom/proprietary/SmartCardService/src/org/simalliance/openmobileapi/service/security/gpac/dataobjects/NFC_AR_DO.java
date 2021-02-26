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


/**
 * NFC-AR-DO:
 * In the NFC use case, mobile device application gather information
 * from their associated card application using the SE access API.
 * However, when the card application needs to trigger its associated mobile application,
 * it sends an HCI EVT_TRANSACTION according to ETSI TS 102 622 [102 622] over SWP to the device.
 * This event is handled by the NFC chipset stack which has to start
 * the corresponding device application. Disclosure of this event to malicious applications
 * can lead to phishing and denial of service attacks.
 * To prevent this, it shall be possible to use the applications signature to authorize
 * device applications to receive HCI events issued by the secure element application.
 * An NFC event data object defines an access rule for generating NFC events for
 * a specific terminal application. The NFC event access can be restricted by a rule based
 * on an event access is NEVER/ ALWAYS allowed policy.
 *
 *
 *
 */
public class NFC_AR_DO extends BerTlv {

    public final static int _TAG = 0xD1;

    private boolean mNfcAllowed = false;

    public NFC_AR_DO(byte[] rawData, int valueIndex, int valueLength) {
        super(rawData, _TAG, valueIndex, valueLength);
    }

    public NFC_AR_DO( boolean allowed ){
        super( null, _TAG, 0, 0);
        mNfcAllowed = allowed;
    }

    public boolean isNfcAllowed(){
        return mNfcAllowed;
    }

    @Override
    /**
     * Tag: D1
     * Length: 1
     * Value:
     * Contains a NFC event access rule:
     * NEVER (00): NFC event access is not allowed
     * ALWAYS(01): NFC event access is allowed
     *
     */
    public void interpret()
        throws ParserException {

        mNfcAllowed = false;

        byte[] data = getRawData();
        int index = getValueIndex();

        if( index + getValueLength() > data.length){
            throw new ParserException( "Not enough data for NFC_AR_DO!");
        }

        if( getValueLength() != 1 ){
            throw new ParserException( "Invalid length of NFC-AR-DO!" );
        } else if ((data[index] != 0x01) && (data[index] != 0x00))
            throw new ParserException( "Invalid value of NFC-AR-DO : " + String.format("%02x", data[index] & 0xff));

        mNfcAllowed = (data[index] == 0x01);
    }

    @Override
    /**
     * Tag: D1
     * Length: 1
     * Value:
     * Contains a NFC event access rule:
     * NEVER (00): NFC event access is not allowed
     * ALWAYS(01): NFC event access is allowed
     *
     */
    public void build( ByteArrayOutputStream stream )
        throws DO_Exception {

        // write tag
        stream.write(getTag());
        stream.write(0x01);
        stream.write(mNfcAllowed ? 0x01 : 0x00 );
    }

}
