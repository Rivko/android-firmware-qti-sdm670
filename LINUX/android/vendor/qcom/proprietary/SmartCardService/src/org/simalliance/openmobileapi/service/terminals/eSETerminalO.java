/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (C) 2011, The Android Open Source Project
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

package org.simalliance.openmobileapi.service.terminals;

import android.content.Context;
import android.util.Log;
import android.os.AsyncTask;

import java.util.Arrays;
import java.util.MissingResourceException;
import java.util.NoSuchElementException;


import org.simalliance.openmobileapi.internal.ByteArrayConverter;
import org.simalliance.openmobileapi.service.CardException;
import org.simalliance.openmobileapi.service.SmartcardService;
import org.simalliance.openmobileapi.service.Terminal;

import com.qualcomm.qti.libesewrapper.eSEWrapper;

public class eSETerminalO extends Terminal {

    private final String TAG;

    private Context mContext;
    private eSEWrapper meSEWrapper;
    GeteSEWrapperTask mGeteSEWrapperTask;

    public int sHandle = 0;
    public int cNumber = 0;
    byte[] command;
    final byte[] MC_OPEN = new byte[] {0x00, 0x70, 0x00, 0x00, 0x01};
    final byte[] SELECT = new byte[] {0x00, (byte)0xA4, 0x04, 0x00, 0x00};
    byte[] MC_CLOSE = new byte[] {0x01, 0x70, (byte)0x80, 0x01};

    public eSETerminalO(Context context, int seId) {
        super(SmartcardService._eSE_TERMINAL + SmartcardService._eSE_TERMINAL_EXT[seId], context);
        mContext = context;
        mGeteSEWrapperTask = new GeteSEWrapperTask(context);
        mGeteSEWrapperTask.execute();
        TAG = SmartcardService._TAG + " - " + SmartcardService._eSE_TERMINAL + "O" + SmartcardService._eSE_TERMINAL_EXT[seId];
        Log.e (TAG, "eSETerminalO Constructor...");
    }

        private class GeteSEWrapperTask extends AsyncTask<Void, Void, Void > {

        Context context;
            @Override
            protected void onPreExecute() {
                super.onPreExecute();
            }

            public GeteSEWrapperTask (Context context){
                this.context = context;
            }

            @Override
            protected Void doInBackground(Void... unused) {
                try {
                    Log.e (TAG, "meSEWrapper init...");
                    meSEWrapper = new eSEWrapper(mContext);
                    if (meSEWrapper == null)
                        Log.e (TAG, "meSEWrapper is NULL");
                } catch (Exception e) {
                        String errorMsg = "eSETerminal() gracefully failing to acquire meSEWrapper" + e;
                        Log.e(TAG, errorMsg);
                }
                return null;
            }


            @Override
            protected void onPostExecute(Void result) {
                super.onPostExecute(result);
                mGeteSEWrapperTask = null;
            }
    }

    public int getsHandle(){
        return sHandle;
    }

    public void setsHandle(int sessionHandle){}

    public boolean isCardPresent() throws CardException {
        if (meSEWrapper.openConnection()) {
            mIsConnected = true;
            internalDisconnect();
            return true;
        }
        return false;
    }

    @Override
    protected void internalOpenSession() {
        sHandle++;
        if (!mIsConnected) {
            try {
                internalConnect();
            } catch (CardException e){
                Log.e(TAG, "internal disconnect failed :" + e);
            }
        }
        Log.i(TAG, "internalOpenSession - number of sessions opened : " + sHandle);
    }

    @Override
    protected void internalConnect() throws CardException {
        if (mIsConnected)
            return;
        if (meSEWrapper.openConnection()) {
            mIsConnected = true;
            mDefaultApplicationSelectedOnBasicChannel = true;
        } else {
            Log.e(TAG, "internalConnect / openConnection failed");
        }
    }

    @Override
    protected void internalDisconnect() throws CardException {
        if (mIsConnected && (sHandle == 0)) {
            meSEWrapper.closeConnection();
            mIsConnected = false;
        }
    }

    @Override
    public void internalCloseSession(int sessionHandle){
        sHandle--;
        Log.i(TAG, "internalCloseSession - number of sessions left : " + sHandle);
        try {
            internalDisconnect();
        } catch (CardException e){
            Log.e(TAG, "internal disconnect failed :" + e);
        }

    }

    @Override
    protected byte[] internalTransmit(byte[] rApdu) throws CardException {
        return meSEWrapper.transceive(rApdu);
    }

    public byte[] getAtr() {

        return null;
    }

    @Override
    protected int internalOpenLogicalChannel() throws CardException,
        NoSuchElementException, MissingResourceException  {

        //Manage channel open
        mSelectResponse = internalTransmit(MC_OPEN);
        if (mSelectResponse == null)
            throw new CardException ("Manage channel Open failed - mSelectResponse is null ");
        int channelNumber = mSelectResponse[0] & 0xFF;

        command = new byte[SELECT.length]; // + Le
        System.arraycopy(SELECT, 0, command, 0, SELECT.length);
        //CLA = Channel number
        command[0]= mSelectResponse[0];

        mSelectResponse = internalTransmit(command);
        if (mSelectResponse == null)
            throw new CardException ("Select default applet failed - mSelectResponse is null ");
        int sw1 = mSelectResponse[mSelectResponse.length - 2] & 0xFF;
        int sw2 = mSelectResponse[mSelectResponse.length - 1] & 0xFF;
        int sw = (sw1 << 8) | sw2;

        Log.i(TAG, "openLogicalChannel(null): " + ByteArrayConverter.byteArrayToHexString(mSelectResponse));
        if ((sw == 0x6a82) || (sw == 0x6999)){
            internalCloseLogicalChannel(channelNumber);
            throw new NoSuchElementException("applet not found");
        } else if (sw == 0x6985) {
            internalCloseLogicalChannel(channelNumber);
            throw new NoSuchElementException("Conditions of use not satisfied");
        } else if ((mSelectResponse[0] & 0xFF)== 0xFF) {
            if ((sw == 0x6a81) || (sw == 0x6881)){
                throw new MissingResourceException("no free channel available", "", "");
            } else {
                throw new CardException ("openLogicalchannel(null) failed with TA answer " + ByteArrayConverter.byteArrayToHexString(mSelectResponse));
            }
        } else {
            if (channelNumber == 0 || channelNumber > 19) {
                throw new CardException("invalid logical channel number returned");
            } else {
                cNumber = channelNumber;
            }
            Log.i(TAG, "openLogicalChannel success : " + ByteArrayConverter.byteArrayToHexString(mSelectResponse));
            //In this case, we did not select any applet, so the "Select Response" shall be null
            mSelectResponse = null;
            return cNumber;
        }
    }

    @Override
    protected int internalOpenLogicalChannel(byte[] aid, byte p2) throws CardException,
        NullPointerException, NoSuchElementException, MissingResourceException {

        //Manage channel open
        mSelectResponse = internalTransmit(MC_OPEN);
        if (mSelectResponse == null)
            throw new CardException ("Manage channel Open failed - mSelectResponse is null ");
        int channelNumber = mSelectResponse[0] & 0xFF;

        command = new byte[SELECT.length + aid.length + 1]; // + Le
        System.arraycopy(SELECT, 0, command, 0, SELECT.length);
        System.arraycopy(aid, 0, command, SELECT.length, aid.length);
        command[0]= mSelectResponse[0];
        command[3] = p2;
        command[4] = (byte)(aid.length & 0xFF);
        command[command.length-1] = 0x00; // Le
        mSelectResponse = internalTransmit(command);
        if (mSelectResponse == null)
            throw new CardException ("select applet failed - mSelectResponse is null ");
        int sw1 = mSelectResponse[mSelectResponse.length - 2] & 0xFF;
        int sw2 = mSelectResponse[mSelectResponse.length - 1] & 0xFF;
        int sw = (sw1 << 8) | sw2;

        Log.i(TAG, "openLogicalChannel: " + ByteArrayConverter.byteArrayToHexString(mSelectResponse));
        if ((sw == 0x6a82) || (sw == 0x6999)){
            internalCloseLogicalChannel(channelNumber);
            throw new NoSuchElementException("applet not found");
        } else if (sw == 0x6985) {
            internalCloseLogicalChannel(channelNumber);
            throw new NoSuchElementException("Conditions of use not satisfied");
        } else if ((mSelectResponse[0] & 0xFF)== 0xFF) {
            if ((sw == 0x6a81) || (sw == 0x6881)){
                throw new MissingResourceException("no free channel available", "", "");
            } else {
                throw new CardException ("openLogical channel failed with TA answer " + ByteArrayConverter.byteArrayToHexString(mSelectResponse));
            }
        } else {
            if (channelNumber == 0 || channelNumber > 19) {
                throw new CardException("invalid logical channel number returned");
            } else {
                cNumber = channelNumber;
            }
            Log.i(TAG, "openLogicalChannel success : " + ByteArrayConverter.byteArrayToHexString(mSelectResponse));
            return cNumber;
        }
    }

    @Override
    protected void internalOpenBasicChannel(byte[] aid) throws Exception, CardException,
        NullPointerException, NoSuchElementException, MissingResourceException {

        if (aid == null) {
            throw new NullPointerException("aid must not be null");
        }

        command = new byte[SELECT.length + aid.length + 1]; // + Le
        System.arraycopy(SELECT, 0, command, 0, SELECT.length);
        System.arraycopy(aid, 0, command, SELECT.length, aid.length);
        command[4] = (byte)(aid.length & 0xFF);
        command[command.length-1] = 0x00; // Le
        mSelectResponse = internalTransmit(command);
        if (mSelectResponse == null)
            throw new CardException ("Manage channel Open failed - mSelectResponse is null ");
        int sw1 = mSelectResponse[mSelectResponse.length - 2] & 0xFF;
        int sw2 = mSelectResponse[mSelectResponse.length - 1] & 0xFF;
        int sw = (sw1 << 8) | sw2;

        // banking application returns 0x6283 to indicate this AID is locked
        if (sw == 0x6A81){
            throw new MissingResourceException("no free channel available", "", "");
        } else if ((sw == 0x6a82) || (sw == 0x6999)) {
            internalCloseLogicalChannel(0);
            throw new NoSuchElementException("applet not found");
        } else if (sw == 0x6985) {
            internalCloseLogicalChannel(0);
            throw new NoSuchElementException("Conditions of use not satisfied");
        }  else if ((sw != 0x9000)&&(sw != 0x6283)) {
            internalCloseLogicalChannel(0);
            throw new NoSuchElementException("SELECT CMD failed - SW is not 0x9000/0x6283");
        } else {
            cNumber = 0;
        }

    }


    @Override
    protected void internalOpenBasicChannel() throws Exception, CardException,
        NoSuchElementException, MissingResourceException {

        mSelectResponse = internalTransmit(SELECT);
        if (mSelectResponse == null)
            throw new CardException ("Manage channel Open failed - mSelectResponse is null ");
        int sw1 = mSelectResponse[mSelectResponse.length - 2] & 0xFF;
        int sw2 = mSelectResponse[mSelectResponse.length - 1] & 0xFF;
        int sw = (sw1 << 8) | sw2;

        // banking application returns 0x6283 to indicate this AID is locked
        if (sw == 0x6A81){
            throw new MissingResourceException("no free channel available", "", "");
        } else if ((sw == 0x6a82) || (sw == 0x6999)) {
            internalCloseLogicalChannel(0);
            throw new NoSuchElementException("applet not found");
        } else if (sw == 0x6985) {
            internalCloseLogicalChannel(0);
            throw new NoSuchElementException("Conditions of use not satisfied");
        }  else if ((sw != 0x9000)&&(sw != 0x6283)) {
            internalCloseLogicalChannel(0);
            throw new NoSuchElementException("SELECT CMD failed - SW is not 0x9000/0x6283");
        } else {
            cNumber = 0;
        }

    }

    @Override
    protected void internalCloseLogicalChannel(int channelNumber) throws CardException {
        if (channelNumber == 0) {
            /*  we try to close the basic channel ,there's no such thing.
                we choose to send an empty select command, to deselect any applet
                which may have been selected on the basi channel instead. this is managed
                by the channel class.
             */
            return;
        } else {
            MC_CLOSE[0] = (byte)(channelNumber & 0xFF);
            MC_CLOSE[3] = (byte)(channelNumber & 0xFF);
            mSelectResponse = internalTransmit(MC_CLOSE);
        }
        if (mSelectResponse == null)
            throw new CardException ("Manage channel Close failed - mSelectResponse is null ");
        int sw1 = mSelectResponse[mSelectResponse.length - 2] & 0xFF;
        int sw2 = mSelectResponse[mSelectResponse.length - 1] & 0xFF;
        int sw = (sw1 << 8) | sw2;
        if (sw == 0x9000)
            Log.i(TAG, "Close successful : " + ByteArrayConverter.byteArrayToHexString(mSelectResponse));
        else
            Log.e(TAG, "Close failed : " + ByteArrayConverter.byteArrayToHexString(mSelectResponse));
    }

    @Override
    public byte[] internalGetSelectResponse(){
        return mSelectResponse;
    }
}
