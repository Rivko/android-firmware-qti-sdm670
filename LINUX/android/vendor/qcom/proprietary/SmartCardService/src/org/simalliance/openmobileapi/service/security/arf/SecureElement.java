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

package org.simalliance.openmobileapi.service.security.arf;

import android.util.Log;
import java.util.MissingResourceException;
import org.simalliance.openmobileapi.service.IChannel;
import org.simalliance.openmobileapi.service.ISmartcardServiceCallback;
import org.simalliance.openmobileapi.service.ITerminal;
import org.simalliance.openmobileapi.service.SmartcardService;
import org.simalliance.openmobileapi.internal.Util;
import org.simalliance.openmobileapi.service.security.ChannelAccess;
import org.simalliance.openmobileapi.service.security.arf.SecureElementException;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.EF;
import org.simalliance.openmobileapi.service.security.gpac.dataobjects.AID_REF_DO;
import org.simalliance.openmobileapi.service.security.gpac.dataobjects.Hash_REF_DO;
import org.simalliance.openmobileapi.service.security.gpac.dataobjects.REF_DO;

/**
 * Provides high-level functions for SE communication
 ***************************************************/
public class SecureElement {

    public static final String TAG = "SmartcardService ACE ARF";

    // Logical channel used for SE communication (optional)
    private IChannel mArfChannel=null;
    // Handle to a built-in "Secure Element"
    private ITerminal mTerminalHandle=null;
    // Arf Controller within the SCAPI handler
    private ArfController mArfHandler=null;
    // Callback used during "Secure Element" communication
    private final ISmartcardServiceCallback mCallback =
            new ISmartcardServiceCallback.Stub(){};

    public static final short SIM_IO = 1;
    public static final short SIM_ALLIANCE = 0;

    // Interface for exchanging APDU commands
    private short mSEInterface=SIM_ALLIANCE;

    /**
     * Constructor
     *
     * @param arfHandler - handle to the owning arf controller object
     * @param handle - handle to the SE terminal to be accessed.
     */
    public SecureElement(ArfController arfHandler,ITerminal handle) {
        mTerminalHandle=handle;
        mArfHandler=arfHandler;
    }

    public short getSeInterface(){
        return mSEInterface;
    }

    public void setSeInterface(short seInterface){
        mSEInterface = seInterface;
    }

    /**
     * Transmits ADPU commands
     * @param cmd APDU command
     * @return Data returned by the APDU command
     */
    public byte[] exchangeAPDU(EF ef, byte[] cmd)
        throws SecureElementException {
        try {
            if (mSEInterface==SIM_IO) {

                return mTerminalHandle.simIOExchange(ef.getFileId(),ef.getFilePath(),cmd);
            } else {

                return mArfChannel.transmit(cmd);
            }
        } catch (Exception e) {
                throw new SecureElementException("Secure Element access error " + e.getLocalizedMessage());
        }
    }

    /**
     * Opens a logical channel to ARF Applet or ADF
     * @param AID Applet identifier
     * @return Handle to "Logical Channel" allocated by the SE;
     *             <code>0</code> if error occurred
     */
    public IChannel openLogicalArfChannel(byte[] AID) {
        try {

            mArfChannel=mTerminalHandle.openLogicalChannel(null,AID, (byte)0x00, mCallback);
            setUpChannelAccess(mArfChannel);
            return mArfChannel;
        } catch(Exception e) {
            if( e instanceof MissingResourceException ){
                // this indicates that no channel is left for accessing the SE element
                Log.e(TAG, "no channels left to access ARF: " + e.getMessage() );
                throw (MissingResourceException)e;
            } else {
                Log.e(TAG,"Error opening logical channel " + e.getLocalizedMessage());
            }
            mArfChannel = null;
            return null;
        }
    }

    /**
     * Closes a logical channel previously allocated by the SE
     * @param handle Handle to open channel
     */
    public void closeArfChannel() {
        try {
            if( mArfChannel != null){

                mArfChannel.close();
                mArfChannel = null;
            } else {

            }

        } catch(Exception e) {
            Log.e(TAG,"Error closing channel " + e.getLocalizedMessage());
        }
    }

    /**
     * Set up channel access to allow,
     * so that PKCS15 files can be read.
     *
     * @param channel
     */
    private void setUpChannelAccess( IChannel channel ){
        // set access conditions to access ARF.
        ChannelAccess arfChannelAccess = new ChannelAccess();
        arfChannelAccess.setAccess(ChannelAccess.ACCESS.ALLOWED, "");
        arfChannelAccess.setApduAccess(ChannelAccess.ACCESS.ALLOWED);
        channel.setChannelAccess(arfChannelAccess);

    }

    public byte[] getRefreshTag() {
        if( mArfHandler != null ){
            return mArfHandler.getAccessRuleCache().getRefreshTag();
        }
        return null;
    }

    public void setRefreshTag(byte[] refreshTag) {
        if( mArfHandler != null ) {
            mArfHandler.getAccessRuleCache().setRefreshTag(refreshTag);
        }
    }

    public void putAccessRule( AID_REF_DO aid_ref_do, Hash_REF_DO hash_ref_do, ChannelAccess channelAccess ) {

        REF_DO ref_do = new REF_DO(aid_ref_do, hash_ref_do);
        mArfHandler.getAccessRuleCache().putWithMerge(ref_do, channelAccess);
    }

    public void resetAccessRules() {
        this.mArfHandler.getAccessRuleCache().reset();
    }
    public void clearAccessRuleCache() {
        this.mArfHandler.getAccessRuleCache().clearCache();
    }
}
