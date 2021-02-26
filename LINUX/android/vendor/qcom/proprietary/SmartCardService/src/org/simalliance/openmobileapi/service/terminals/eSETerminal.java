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
import android.os.Binder;
import android.os.Bundle;
import android.util.Log;
import android.os.AsyncTask;
import android.os.RemoteException;

import java.util.Arrays;
import java.util.MissingResourceException;
import java.util.NoSuchElementException;

import com.android.qti.qpay.QPayAdapter;

import org.simalliance.openmobileapi.internal.ByteArrayConverter;
import org.simalliance.openmobileapi.service.CardException;
import org.simalliance.openmobileapi.service.SmartcardService;
import org.simalliance.openmobileapi.service.Terminal;


public class eSETerminal extends Terminal {

    private final String TAG;

    private Binder binder = new Binder();

    private final int mSeId;

    private Context mContext;
    private QPayAdapter mQPayAdapter;
    GetQPayAdapterTask mGetQPayAdapterTask;
    byte[] NOK = {(byte)0xFF, (byte) 0xFF};
    byte[] OK = {0x00};
    byte[] SW_OK = new byte[] {(byte)0x90, 0x00};
    boolean isPending = false;
    private final int TEE_SEServiceGetReaders = 43;
    private final int TEE_SEReaderGetProperties = 51;
    private final int TEE_SEReaderGetName = 52;
    private final int TEE_SEReaderOpenSession = 53;
    private final int TEE_SEReaderCloseSessions = 54;
    private final int TEE_SESessionGetATR = 61;
    private final int TEE_SESessionIsClosed = 62;
    private final int TEE_SESessionClose = 63;
    private final int TEE_SESessionCloseChannels = 64;
    private final int TEE_SESessionOpenBasicChannel = 65;
    private final int TEE_SESessionOpenLogicalChannel = 66;
    private final int TEE_SEChannelClose = 71;
    private final int TEE_SEChannelSelectNext = 72;
    private final int TEE_SEChannelGetSelectResponse = 73;
    private final int TEE_SEChannelTransmit  = 74;
    public int sHandle = 0;
    public int cNumber = 0;

    private class GetQPayAdapterTask extends AsyncTask<Void, Void, Void > {

        Context context;

        @Override
        protected void onPreExecute() {
            super.onPreExecute();
        }

        public GetQPayAdapterTask (Context context){
            this.context = context;
        }

        @Override
        protected Void doInBackground(Void... unused) {
            try {
                mQPayAdapter = new QPayAdapter(mContext);
                if (mQPayAdapter == null) {
                    Log.e (TAG, "mQPayAdapter is NULL");
                } else {
                    if (!mQPayAdapter.getNfcState()){
                        Log.e (TAG, "NFC is off");
                        return null;
                    }
                    if (mQPayAdapter.open()){
                        Log.i (TAG, "acquired QPayAdapter, TA APP connected");
                        return null;
                        }
                    else {
                        Log.e (TAG, "acquired QPayAdapter, TA APP NOT connected");
                    }
                }
            } catch (Exception e) {
                    String errorMsg = "eSETerminal() gracefully failing to acquire QPayAdapter" + e;
                    Log.e(TAG, errorMsg);
            }
            return null;
        }


        @Override
        protected void onPostExecute(Void result) {
            super.onPostExecute(result);
            mGetQPayAdapterTask = null;
        }
    }

    public eSETerminal(Context context, int seId) {
        super(SmartcardService._eSE_TERMINAL + SmartcardService._eSE_TERMINAL_EXT[seId], context);
        mSeId = seId;
        mContext = context;
        mGetQPayAdapterTask = new GetQPayAdapterTask(context);
        mGetQPayAdapterTask.execute();
        TAG = SmartcardService._TAG + " - " + SmartcardService._eSE_TERMINAL + SmartcardService._eSE_TERMINAL_EXT[seId];
    }

    public int getsHandle(){
        return sHandle;
    }

    public void setsHandle(int sessionHandle){
        sHandle = sessionHandle;
    }

    public boolean isCardPresent() throws CardException {
        try {
            if (!mQPayAdapter.getNfcState()){
                Log.e(TAG, "NFC is OFF");
                return false;
            }
            if (mQPayAdapter != null) {
                byte[] SW = mQPayAdapter.transceive(TEE_SEServiceGetReaders, null, 0, 0);
                if (Arrays.equals(SW,OK)) {
                    mDefaultApplicationSelectedOnBasicChannel = true;
                    return true;
                }
            } else {
                Log.e (TAG, "cannot get QpayAdapter");
                return false;
            }
        } catch (Exception e) {
            Log.e (TAG, e.getMessage());
            return false;
        }
        return false;
    }

    @Override
    protected void internalOpenSession() {
        try {
            if (mQPayAdapter != null) {
                if (mQPayAdapter.open()){
                    byte[] SW = mQPayAdapter.transceive(TEE_SEReaderOpenSession, null, 0, 0);
                    setsHandle(mQPayAdapter.getSessionHandle());
                    Log.i (TAG, "TEE_SEReaderOpenSession - Session Handle is : " + sHandle);
                    mIsConnected = ( (Arrays.equals(SW,OK)) && (sHandle > 0));
                }
                if (!mIsConnected) {
                    Log.e (TAG, "TEE_SEReaderOpenSession FAILED");
                }
            }
        } catch (Exception e) {
            Log.e (TAG, e.getMessage());
            mIsConnected = false;
        }
        mIsConnected = true;

    }

    @Override
    protected void internalConnect() throws CardException {
        mIsConnected = true;
    }

    @Override
    protected void internalDisconnect() throws CardException {
        mIsConnected = false;
    }

    @Override
    public void internalCloseSession(int sessionHandle){
        try {
            if (mQPayAdapter != null) {
                Log.i (TAG, "TEE_SEReaderCloseSession - handle is " + sessionHandle);
                byte[] SW = mQPayAdapter.transceive(TEE_SESessionClose, null, sessionHandle, 0);
                mIsConnected = !Arrays.equals(SW,OK);
                if (mIsConnected) {
                    Log.e (TAG, "TEE_SEReaderCloseSession FAILED");
                }
            }
        } catch (Exception e) {
            Log.e (TAG, e.getMessage());
        }
    }

    @Override
    protected byte[] internalTransmit(byte[] command) throws CardException {
        byte[] answer;
        try {
            if (mQPayAdapter != null) {
                if ((command[1] == (byte)0xA4) && (command[2] == 0x04) && (command[3] == 0x02)) {
                    //the wallet is using the SelectNext() API
                    Log.i(TAG, "TEE_SEChannelSelectNext detected");
                    cNumber = command[0] & 0xFF;
                    answer = mQPayAdapter.transceive(TEE_SEChannelSelectNext, null, 0, cNumber);
                } else {
                    answer = mQPayAdapter.transceive(TEE_SEChannelTransmit, command, 0, 0);
                }
                if (answer == null) {
                    throw new CardException("Unable to communicate with the TA.");
                } else {
                    return answer;
                }
            }
        } catch (Exception e) {
            Log.e (TAG, e.getMessage());
            return null;
        }
        return null;
    }

    public byte[] getAtr() {
        byte[] SW;
        try {
            if (mQPayAdapter != null) {
                SW = mQPayAdapter.transceive(TEE_SESessionGetATR, null, 0, 0);

                if (Arrays.equals(SW,NOK)) {
                    Log.i (TAG, "TEE_SESessionGetATR failed: " + ByteArrayConverter.byteArrayToHexString(SW));
                    return null;
                } else {
                    return SW;
                }
            }
        } catch (Exception e) {
            Log.e (TAG, e.getMessage());
            return null;
        }
        return null;
    }

    @Override
    protected int internalOpenLogicalChannel() throws CardException,
        NoSuchElementException, MissingResourceException  {

        mSelectResponse = null;
        int channelNumber = 0;
        int cHandle = 0;
        if (mQPayAdapter != null) {
            try {
                mSelectResponse = mQPayAdapter.transceive(TEE_SESessionOpenLogicalChannel, null, sHandle, 0);
                } catch (Exception e) {
                    Log.e (TAG, e.getMessage());
                    throw new NoSuchElementException(e.getMessage());
                }
                channelNumber = mSelectResponse[0] & 0xFF;

                int sw1 = mSelectResponse[mSelectResponse.length - 2] & 0xFF;
                int sw2 = mSelectResponse[mSelectResponse.length - 1] & 0xFF;
                int sw = (sw1 << 8) | sw2;

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
            else {
                throw new CardException("openLogical channel failed - QPayAdapter is null");
            }
    }

    @Override
    protected int internalOpenLogicalChannel(byte[] aid, byte p2) throws CardException,
        NullPointerException, NoSuchElementException, MissingResourceException {

        mSelectResponse = null;
        int channelNumber = 0;
        if (aid == null) {
            throw new NullPointerException("aid must not be null");
        }

            if (mQPayAdapter != null) {
                try {
                    mSelectResponse = mQPayAdapter.transceive(TEE_SESessionOpenLogicalChannel, aid, sHandle, 0);
                    Log.i(TAG, "TEE_SESessionOpenLogicalChannel(aid) - sHandle : " + sHandle + " mSelectResponse : "
                            + ByteArrayConverter.byteArrayToHexString(mSelectResponse));
                } catch (Exception e) {
                    Log.e (TAG, e.getMessage());
                    throw new NoSuchElementException(e.getMessage());
                }
                channelNumber = mSelectResponse[0] & 0xFF;

                int sw1 = mSelectResponse[mSelectResponse.length - 2] & 0xFF;
                int sw2 = mSelectResponse[mSelectResponse.length - 1] & 0xFF;
                int sw = (sw1 << 8) | sw2;
                // banking application returns 0x6283 to indicate this AID is locked
                if ((sw == 0x6a81) || (sw == 0x6881)){
                    throw new MissingResourceException("no free channel available", "", "");
                } else if ((sw == 0x6a82) || (sw == 0x6999)){
                    internalCloseLogicalChannel(channelNumber);
                    throw new NoSuchElementException("applet not found");
                } else if (sw == 0x6985) {
                    internalCloseLogicalChannel(channelNumber);
                    throw new NoSuchElementException("Conditions of use not satisfied");
                } else if ((mSelectResponse[0] & 0xFF)== 0xFF) {
                        throw new CardException ("openLogical channel failed with TA answer " +
                            ByteArrayConverter.byteArrayToHexString(mSelectResponse));
                } else {
                    if (channelNumber == 0 || channelNumber > 19) {
                        throw new CardException("invalid logical channel number returned");
                    } else {
                        cNumber = channelNumber;
                    }
                    Log.i(TAG, "openLogicalChannel success : " +
                        ByteArrayConverter.byteArrayToHexString(mSelectResponse));
                    return cNumber;
                }
            }
            else {
                throw new CardException ("openLogical channel failed - QPayAdapter is null");
            }
    }

    @Override
    protected void internalOpenBasicChannel(byte[] aid) throws Exception, CardException,
        NullPointerException, NoSuchElementException, MissingResourceException {

        mSelectResponse = null;
        if (aid == null) {
            throw new NullPointerException("aid must not be null");
        }
        try {
            if (mQPayAdapter != null) {
                mSelectResponse = mQPayAdapter.transceive(TEE_SESessionOpenBasicChannel, aid, sHandle, 0);
                if (Arrays.equals(mSelectResponse,NOK)){
                    throw new Exception ("TEE_SESessionOpenBasicChannel failed with TA answer " +
                            ByteArrayConverter.byteArrayToHexString(mSelectResponse));
                }
                Log.i(TAG, "TEE_SESessionOpenBasicChannel  sHandle : " + sHandle + " mSelectResponse : "
                            + ByteArrayConverter.byteArrayToHexString(mSelectResponse));
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

        } catch (Exception e) {
            throw new NoSuchElementException(e.getMessage());
        }

    }


    @Override
    protected void internalOpenBasicChannel() throws Exception, CardException,
        NoSuchElementException, MissingResourceException {

        mSelectResponse = null;
        try {
            if (mQPayAdapter != null) {
                Log.i(TAG, "TEE_SESessionOpenBasicChannel(aid) with session handle : " + Integer.toString(sHandle));
                mSelectResponse = mQPayAdapter.transceive(TEE_SESessionOpenBasicChannel, null, sHandle, 0);
                if (Arrays.equals(mSelectResponse,NOK)){
                    throw new Exception ("TEE_SESessionOpenBasicChannel failed with TA answer " +
                        ByteArrayConverter.byteArrayToHexString(mSelectResponse));
                }
                Log.i(TAG, "TEE_SESessionOpenBasicChannel  sHandle : " + sHandle + " mSelectResponse : "
                            + ByteArrayConverter.byteArrayToHexString(mSelectResponse));
                int sw1 = mSelectResponse[mSelectResponse.length - 2] & 0xFF;
                int sw2 = mSelectResponse[mSelectResponse.length - 1] & 0xFF;
                int sw = (sw1 << 8) | sw2;

                // banking application returns 0x6283 to indicate this AID is locked
                if ((sw == 0x6a82) || (sw == 0x6999)) {
                    internalCloseLogicalChannel(0);
                    throw new NoSuchElementException("applet not found");
                } else if (sw == 0x6985) {
                    internalCloseLogicalChannel(0);
                    throw new NoSuchElementException("Conditions of use not satisfied");
                } else if (sw == 0x6A81){
                    throw new MissingResourceException("no free channel available", "", "");
                } else if (sw == 0x0000) {
                    cNumber = 0;
                    mSelectResponse = null;
                } else if ((sw != 0x9000)&&(sw != 0x6283)) {
                    internalCloseLogicalChannel(0);
                    throw new NoSuchElementException("SELECT CMD failed - SW is not 0x9000/0x6283");
                } else {
                    cNumber = 0;
                }

            }
        } catch (Exception e) {
            throw new NoSuchElementException(e.getMessage());
        }
    }

    @Override
    protected void internalCloseLogicalChannel(int ichannelNumber) throws CardException {
        try {
            if (mQPayAdapter != null) {
                byte[] bchannel_Number = {((byte)(ichannelNumber & 0xFF))};
                byte[] SW = mQPayAdapter.transceive(TEE_SEChannelClose, bchannel_Number, 0, 0);
                if (Arrays.equals(SW,OK))
                    Log.i (TAG, "Channel " + ichannelNumber + " has been closed");
                else
                    Log.e (TAG, "Channel has NOT been closed : " +
                        ByteArrayConverter.byteArrayToHexString(SW));
            } else {
                Log.e (TAG, "cannot get QpayAdapter");
            }
        } catch (Exception e) {
            Log.e (TAG, e.getMessage());
        }
    }
    @Override
    public byte[] internalGetSelectResponse(){
        try {
            if (mQPayAdapter != null) {
                byte[] SW = mQPayAdapter.transceive(TEE_SEChannelGetSelectResponse, null, 0, cNumber);
                if (!(Arrays.equals(SW,NOK))) {
                    if (SW.length < 2){
                        return null;
                    } else {
                        return SW;
                    }
                } else {
                    Log.i (TAG, "getSelectResponse failed: " +
                        ByteArrayConverter.byteArrayToHexString(SW));
                    return null;
                }
            } else {
                Log.i (TAG, "cannot get QpayAdapter");
            }
        } catch (Exception e) {
            Log.e (TAG, e.getMessage());
        }
        return null;
    }
}
