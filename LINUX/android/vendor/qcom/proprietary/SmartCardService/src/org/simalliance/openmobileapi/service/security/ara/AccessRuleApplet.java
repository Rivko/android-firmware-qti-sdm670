/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright 2010 Giesecke & Devrient GmbH.
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


package org.simalliance.openmobileapi.service.security.ara;


import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.security.AccessControlException;

import org.simalliance.openmobileapi.service.CardException;
import org.simalliance.openmobileapi.service.IChannel;
import org.simalliance.openmobileapi.internal.Util;
import org.simalliance.openmobileapi.service.security.CommandApdu;
import org.simalliance.openmobileapi.service.security.ResponseApdu;
import org.simalliance.openmobileapi.service.security.gpac.dataobjects.BerTlv;
import org.simalliance.openmobileapi.service.security.gpac.dataobjects.ParserException;
import org.simalliance.openmobileapi.service.security.gpac.dataobjects.Response_DO_Factory;
import org.simalliance.openmobileapi.service.security.gpac.dataobjects.Response_RefreshTag_DO;

public class AccessRuleApplet {

    final private static String ACCESS_RULE_APPLET_TAG = "AccessRuleApplet";

    final private static int _MAX_LEN = 0x00; // should be adapted by OEM, this is a defensive value since some devices/modems have problems with Le=0x00 or 0xFF.

    final private static CommandApdu mGetAll = new CommandApdu(0x80, 0xCA, 0xFF, 0x40, _MAX_LEN);

    final private static CommandApdu mGetSpecific = new CommandApdu(0x80, 0xCA, 0xFF, 0x50, _MAX_LEN);

    final private static CommandApdu mGetNext = new CommandApdu(0x80, 0xCA, 0xFF, 0x60, _MAX_LEN);

    final private static CommandApdu mGetRefreshTag = new CommandApdu(0x80, 0xCA, 0xDF, 0x20, _MAX_LEN );


    private IChannel mChannel = null;

    public AccessRuleApplet(IChannel channel) {
        mChannel = channel;
    }

    public byte[] readSpecificAccessRule( byte[] aid_ref_do ) throws AccessControlException, CardException {

        if( aid_ref_do == null ){
            throw new AccessControlException("GET DATA (specific): Reference data object must not be null.");
        }

        ByteArrayOutputStream stream = new ByteArrayOutputStream();
        int overallLen = 0;

        // send GET DATA (specific)
        CommandApdu apdu = (CommandApdu) mGetSpecific.clone();
        apdu.setData(aid_ref_do);
        ResponseApdu response = send(apdu);

        // OK
        if( response.isStatus( 0x9000 ) ) {
            // check if more data has to be fetched
            BerTlv tempTlv = null;
            try {
                tempTlv = BerTlv.decode(response.getData(), 0, false);
            } catch (ParserException e) {
                throw new AccessControlException("GET DATA (specific) not successfull. Tlv encoding wrong.");
            }

            // the first data block contain the length of the TLV + Tag bytes + length bytes.
            overallLen = tempTlv.getValueLength() + tempTlv.getValueIndex();
            try {
                stream.write(response.getData());
            } catch (IOException e) {
                throw new AccessControlException("GET DATA (specific) IO problem. " + e.getMessage() );
            }

            int le;
            // send subsequent GET DATA (next) commands
            while( stream.size() < overallLen ){
                le = overallLen - stream.size();
                if( le > _MAX_LEN ){
                    le = _MAX_LEN;
                }
                // send GET DATA (next)
                apdu = (CommandApdu) mGetNext.clone();
                apdu.setLe(le);
                response = send(apdu);

                // OK
                if( response.isStatus( 0x9000 ) ){
                    try {
                        stream.write(response.getData());
                    } catch (IOException e) {
                        throw new AccessControlException("GET DATA (next) IO problem. " + e.getMessage() );
                    }
                } else {
                    throw new AccessControlException( "GET DATA (next) not successfull, . SW1SW2=" + response.getSW1SW2());
                }
            }

            return stream.toByteArray();
            // referenced data not found
        } else if( response.isStatus( 0x6A88 )){
            return null;
        } else {
            throw new AccessControlException("GET DATA (specific) not successfull. SW1SW2=" + response.getSW1SW2());
        }
    }

    public byte[] readAllAccessRules() throws AccessControlException, CardException {

        ByteArrayOutputStream stream = new ByteArrayOutputStream();
        int overallLen = 0;

        // send GET DATA (specific)
        CommandApdu apdu = (CommandApdu) mGetAll.clone();
        ResponseApdu response = send(apdu);

        // OK
        if( response.isStatus( 0x9000 ) ){

            // check if more data has to be fetched
            BerTlv tempTlv = null;
            try {
                tempTlv = BerTlv.decode(response.getData(), 0, false);
            } catch (ParserException e) {
                throw new AccessControlException("GET DATA (all) not successfull. Tlv encoding wrong.");
            }

            // the first data block contain the length of the TLV + Tag bytes + length bytes.
            overallLen = tempTlv.getValueLength() + tempTlv.getValueIndex();


            try {
                stream.write(response.getData());
            } catch (IOException e) {
                throw new AccessControlException("GET DATA (all) IO problem. " + e.getMessage() );
            }

            int le;
            // send subsequent GET DATA (next) commands
            while( stream.size() < overallLen ){
                le = overallLen - stream.size();

                if( le > _MAX_LEN ){
                    le = _MAX_LEN;
                }
                // send GET DATA (next)
                apdu = (CommandApdu) mGetNext.clone();
                apdu.setLe(le);

                response = send(apdu);
                // OK
                if( response.isStatus( 0x9000 ) ) {
                    try {
                        stream.write(response.getData());
                    } catch (IOException e) {
                        throw new AccessControlException("GET DATA (next) IO problem. " + e.getMessage() );
                    }
                } else {
                    throw new AccessControlException( "GET DATA (next) not successfull, . SW1SW2=" + response.getSW1SW2());
                }
            }

            return stream.toByteArray();
            // referenced data not found
        } else if( response.isStatus( 0x6A88 )){
            return null;
        } else {
            Log.e("SmartcardService.ARA", "GET DATA (all) not successfull. SW1SW2=" + response.getSW1SW2());
            return null;
        }
    }

    public byte[] readRefreshTag() throws AccessControlException, CardException {

        // send GET DATA (specific)
        CommandApdu apdu = (CommandApdu) mGetRefreshTag.clone();
        ResponseApdu response = send(apdu);

        // OK
        if( response.isStatus( 0x9000 ) ){

            // check if more data has to be fetched
            BerTlv tempTlv = null;
            Response_RefreshTag_DO refreshDo;
            try {
                tempTlv = Response_DO_Factory.createDO(response.getData());
                if( tempTlv instanceof Response_RefreshTag_DO ) {
                    refreshDo = (Response_RefreshTag_DO)tempTlv;
                    return refreshDo.getRefreshTagArray();
                } else {
                    throw new AccessControlException("GET REFRESH TAG returned invalid Tlv.");
                }
            } catch (ParserException e) {
                throw new AccessControlException("GET REFRESH TAG not successfull. Tlv encoding wrong.");
            }
        }
        throw new AccessControlException("GET REFRESH TAG not successfull.");
    }

    private ResponseApdu send(CommandApdu cmdApdu) throws CardException {

        byte[] response = null;
        try {
            response = mChannel.transmit(cmdApdu.toBytes());
        } catch (IOException e) {
            //Do nothing
        }

        ResponseApdu resApdu = new ResponseApdu(response);
        return resApdu;
    }
}
