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
 * Copyright (C) 2011 Deutsche Telekom, A.G.
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

/*
 * Contributed by: Giesecke & Devrient GmbH.
 */

package org.simalliance.openmobileapi.service.security.arf.PKCS15;

import android.util.Log;

import java.util.Vector;

import org.simalliance.openmobileapi.service.security.ApduFilter;
import org.simalliance.openmobileapi.service.security.ChannelAccess;
import org.simalliance.openmobileapi.service.security.arf.ASN1;
import org.simalliance.openmobileapi.service.security.arf.DERParser;
import org.simalliance.openmobileapi.service.security.arf.SecureElement;
import org.simalliance.openmobileapi.internal.Util;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.PKCS15Exception;
import org.simalliance.openmobileapi.service.security.gpac.dataobjects.AID_REF_DO;
import org.simalliance.openmobileapi.service.security.gpac.dataobjects.Hash_REF_DO;

/**
 * EF_ACConditions related features
 ***************************************************/
public class EFACConditions extends EF {

    public static final String TAG = "ACE ARF EF_ACConditions";

    // Identification of the cardlet
    private AID_REF_DO mAid_Ref_Do=null;

    private byte[] mData = null;


    /**
     * Constructor
     * @param secureElement SE on which ISO7816 commands are applied
     * @param AID Identification of the applet
     */
    public EFACConditions(SecureElement handle,AID_REF_DO Aid_Ref_Do) {
         super( handle );

         mAid_Ref_Do=Aid_Ref_Do;
    }

    /**
     * Decodes EF_ACConditions file
     * @param buffer ASN.1 data
     */
    private void decodeDER(byte[] buffer)
        throws PKCS15Exception
    {
        byte[] certificateHash=null;
        DERParser DER=new DERParser(buffer);

        // the default channelAccess will deny every access.
        ChannelAccess channelAccess = new ChannelAccess();
        Hash_REF_DO hash_ref_do = new Hash_REF_DO();

        // empty condition file
        if (DER.isEndofBuffer()) {
            channelAccess.setAccess(ChannelAccess.ACCESS.DENIED, "Empty ACCondition");
            channelAccess.setNFCEventAccess(ChannelAccess.ACCESS.DENIED);
            channelAccess.setApduAccess(ChannelAccess.ACCESS.DENIED);
            channelAccess.setUseApduFilter(false);
            channelAccess.setApduFilter(null);
            Log.i(TAG, "Empty ACCondition: Access Deny for all apps");

            mSEHandle.putAccessRule(mAid_Ref_Do, hash_ref_do, channelAccess);
            return;
        }

        //----
        // 2012-04-16
        /*
           Condition ::= SEQUENCE {
                  cert   CertHash OPTIONAL,
                  accessRules    [0]AccessRules OPTIONAL
            }

            AccessRules ::= SEQUENCE OF AccessRule

            AccessRule ::=CHOICE {
                apduAccessRule    [0]APDUAccessRule,
                nfcAccessRule    [1]NFCAccessRule
            }

            APDUAccessRule ::= CHOICE {
                   apduPermission [0] APDUPermission,
                   apduFilter [1] APDUFilter
            }

            APDUFilters ::= SEQUENCE OF APDUFilter

            NFCAccessRule ::= CHOICE {
                   nfcPermission [0] NFCPermission
            }
         */
        while(!DER.isEndofBuffer()) {

            // if a hash value was found then access is allowed
            // even if NO more access rule is given.
            // missing APDU Permission will always allow APDU access
            // missing NFC Permission will always allow NFC event.
            // See GPAC Chapter 7.1.7
            // See Examples in Annex C of GPAC
            channelAccess = new ChannelAccess();
            channelAccess.setAccess(ChannelAccess.ACCESS.ALLOWED, "");
            channelAccess.setApduAccess(ChannelAccess.ACCESS.ALLOWED);
            channelAccess.setNFCEventAccess(ChannelAccess.ACCESS.UNDEFINED);
            channelAccess.setUseApduFilter(false);

            if ( DER.parseTLV(ASN1.TAG_Sequence) > 0 ) {
                    byte[] tempTLVData = DER.getTLVData();
                    DERParser derRule = new DERParser( tempTLVData);

                    if (tempTLVData[0] == ASN1.TAG_OctetString) {
                        derRule.parseTLV(ASN1.TAG_OctetString);
                        certificateHash=derRule.getTLVData();

                        if (certificateHash.length!=Hash_REF_DO._SHA1_LEN &&
                            certificateHash.length!=0) {
                            // other hash than SHA-1 hash values are not supported.
                            throw new PKCS15Exception("Invalid hash found!");
                        } else {
                            hash_ref_do =new Hash_REF_DO(certificateHash);
                        }
                    } else if (tempTLVData[0] == ASN1.TAG_Padding) {
                        throw new PKCS15Exception("Invalid hash found!");
                    }
                    else {
                        Log.i(TAG,"No hash included");
                        //Let's put a null hash, to prioritize any more specific rule.
                        hash_ref_do = new Hash_REF_DO(null);
                    }

                // 2012-04-16
                // parse optional Access Rule.
                if( !derRule.isEndofBuffer() ) {

                    if( derRule.parseTLV() == (byte)0xA0 ) {

                        DERParser derAccessRules = new DERParser(derRule.getTLVData());

                        while(!derAccessRules.isEndofBuffer()) {
                            switch( derAccessRules.parseTLV() ){
                                // APDU Access Rule
                                case (byte)0xA0:
                                    DERParser derApduRule = new DERParser( derAccessRules.getTLVData());
                                    byte tagApduAccessRule = derApduRule.parseTLV();

                                    if( tagApduAccessRule == (byte)0x80 ) { // APDU Permission  (primitive)

                                            channelAccess.setApduAccess(
                                                    derApduRule.getTLVData()[0] == 0x01 ? ChannelAccess.ACCESS.ALLOWED : ChannelAccess.ACCESS.DENIED);

                                    } else if( tagApduAccessRule == (byte)0xA1 ) { // APDU Filter (constructed)

                                        DERParser derApduFilter = new DERParser( derApduRule.getTLVData() );
                                        byte tag = derApduFilter.parseTLV();

                                        if( tag == ASN1.TAG_OctetString ) {

                                            Vector<ApduFilter> apduFilter = new Vector<ApduFilter>();

                                            // collect all apdu filter tlvs.
                                            apduFilter.add(new ApduFilter( derApduFilter.getTLVData()));

                                            while( !derApduFilter.isEndofBuffer()) {
                                                if( derApduFilter.parseTLV() == ASN1.TAG_OctetString ) {
                                                    apduFilter.add(new ApduFilter( derApduFilter.getTLVData()));
                                                }
                                            }
                                            channelAccess.setUseApduFilter(true);
                                            channelAccess.setApduFilter(apduFilter.toArray(new ApduFilter[apduFilter.size()]));
                                        } else {
                                               throw new PKCS15Exception("Invalid element found!");
                                        }

                                    } else {
                                           throw new PKCS15Exception("Invalid element found!");
                                    }
                                break;
                                // NFC Access Rule
                                case (byte)0xA1:
                                    DERParser derNfc = new DERParser(derAccessRules.getTLVData());

                                    if( derNfc.parseTLV() == (byte)0x80 ) { // NFC Permission (primitive)
                                        channelAccess.setNFCEventAccess(
                                                derNfc.getTLVData()[0] == (byte)0x01 ? ChannelAccess.ACCESS.ALLOWED : ChannelAccess.ACCESS.DENIED);
                                    } else {
                                        throw new PKCS15Exception("Invalid element found!");
                                    }
                                    break;
                                default:
                                    throw new PKCS15Exception("Invalid element found!");
                            }
                        }
                    } else {
                        // no explicit access rule given.
                    }
                }
            } else {
                // coding 30 00 -> empty hash value given (all applications)
                if((channelAccess.getNFCEventAccess() == ChannelAccess.ACCESS.UNDEFINED) &&
                        (channelAccess.getApduAccess() != ChannelAccess.ACCESS.UNDEFINED)) {
                    channelAccess.setNFCEventAccess(channelAccess.getApduAccess());
                }
            }
            //----
            mSEHandle.putAccessRule(mAid_Ref_Do, hash_ref_do, channelAccess);
        } ;
    }


    /**
     * Stores a restricted list of certificate hashes
     * @param path Path of the "EF_ACConditions" file
     */
    public void addRestrictedHashes(byte[] path) throws PKCS15Exception {
        try {
            Log.i(TAG,"Reading and analysing EF_ACConditions...");
            if (selectFile(path) == APDU_SUCCESS) {
                mData = readBinary(0,Util.END);
                decodeDER(mData);
            } else {
                Log.e(TAG,"EF_ACConditions not found!");
            }
        } catch (Exception e) {
            throw new PKCS15Exception(e.getMessage());
        }
    }

    /**
     * Stores a restricted list of certificate hashes
     * @param path Path of the "EF_ACConditions" file
     */
    public void addRestrictedHashesFromData(byte[] data) throws PKCS15Exception {
        try {
            Log.i(TAG,"Analysing cached EF_ACConditions data...");
            if( data != null ) {
                mData = data;
                decodeDER(mData);
            } else {
                Log.e(TAG,"EF_ACConditions data not available!");
            }
        } catch (Exception e) {
            throw new PKCS15Exception(e.getMessage());
        }
    }

    public byte[] getData() {
        return mData;
    }

}
