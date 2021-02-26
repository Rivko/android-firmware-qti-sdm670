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
import org.simalliance.openmobileapi.internal.ByteArrayConverter;
import org.simalliance.openmobileapi.internal.Util;
import org.simalliance.openmobileapi.service.CardException;
import org.simalliance.openmobileapi.service.security.arf.SecureElementException;
import org.simalliance.openmobileapi.service.SmartcardService;
import org.simalliance.openmobileapi.service.Terminal;

import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;

import android.util.Log;

import android.telephony.IccOpenLogicalChannelResponse;
import com.android.internal.telephony.ITelephony;
import com.android.internal.telephony.TelephonyProperties;
import android.telephony.SubscriptionManager;

import java.util.MissingResourceException;
import java.util.NoSuchElementException;
import java.io.IOException;

public class UiccTerminal extends Terminal {

    public final String _TAG;

    private ITelephony manager = null;

    private int[] channelId = new int[20];

    private String currentSelectedFilePath = "";

    private final int subId[];

    private final int mUiccSlot;

    public UiccTerminal(Context context, int slot) {
        super(SmartcardService._UICC_TERMINAL + SmartcardService._UICC_TERMINAL_EXT[slot], context);

        try {
            manager = ITelephony.Stub.asInterface(ServiceManager
                    .getService(Context.TELEPHONY_SERVICE));
        } catch (Exception ex) {
        }

        mUiccSlot = slot;
        SubscriptionManager mSubManager = SubscriptionManager.from(context);
        subId = mSubManager.getSubId (mUiccSlot);

        _TAG = SmartcardService._TAG + " - " + SmartcardService._UICC_TERMINAL + SmartcardService._UICC_TERMINAL_EXT[slot];

        Log.i(_TAG, "UiccTerminal(): mIsMultiSimEnabled = " + SmartcardService.mIsMultiSimEnabled);

        for (int i = 0; i < channelId.length; i++)
            channelId[i] = 0;
    }

    public boolean isCardPresent() throws CardException {
        String prop = SystemProperties
                .get(TelephonyProperties.PROPERTY_SIM_STATE);

        Log.i(_TAG, "isCardPresent(): PROPERTY_SIM_STATE=" + prop);

        String[] simStatusStr = prop.split(",");

        if ((simStatusStr.length > mUiccSlot)&&(
            (simStatusStr[mUiccSlot].equals("READY"))||(simStatusStr[mUiccSlot].equals("PIN_REQUIRED")))) {
            return true;
        } else {
            return false;
        }
    }

    @Override
    protected void internalConnect() throws CardException {
        if (manager == null) {
            Log.i(_TAG, "Try to rebind telephone service");
            manager = ITelephony.Stub.asInterface(ServiceManager
                    .getService(Context.TELEPHONY_SERVICE));
            if (manager == null) {
                Log.e(_TAG, "internalConnect(): throw CardException");
                throw new CardException("Cannot connect to Telephony Service");
            }
        }
        mIsConnected = true;
    }

    @Override
    protected void internalDisconnect() throws CardException {
    }

    /**
     * Clear the channel number
     *
     * @param cla
     *
     * @return the cla without channel number
     */
    private byte clearChannelNumber(byte cla) {
        // bit 7 determines which standard is used
        boolean isFirstInterindustryClassByteCoding = ((cla & 0x40) == 0x00);

        if(isFirstInterindustryClassByteCoding){
            // First Interindustry Class Byte Coding
            // see 11.1.4.1: channel number is encoded in the 2 rightmost bits
            return (byte)(cla & 0xFC);
        }else{
            // Further Interindustry Class Byte Coding
            // see 11.1.4.2: channel number is encoded in the 4 rightmost bits
            return (byte)(cla & 0xF0);
        }
    }

    @Override
    protected byte[] internalTransmit(byte[] command) throws CardException {
        int cla = clearChannelNumber(command[0]) & 0xff;
        int ins = command[1] & 0xff;
        int p1 = command[2] & 0xff;
        int p2 = command[3] & 0xff;
        int p3 = -1;
        if (command.length > 4) {
            p3 = command[4] & 0xff;
        }
        String data = null;
        if (command.length > 5) {
            data = ByteArrayConverter.byteArrayToHexString(command, 5);
        }

        int channelNumber = parseChannelNumber(command[0]);

        if (channelNumber == 0) {

            try {
                    String response = manager.iccTransmitApduBasicChannel(subId[0], cla, ins, p1,
                            p2, p3, data);
                    return ByteArrayConverter.hexStringToByteArray(response);
            } catch (Exception ex) {
                Log.e(_TAG, "internalTransmit(): throw CardException");
                throw new CardException("transmit command failed");
            }

        } else {
            if ((channelNumber > 0) && (channelId[channelNumber] == 0)) {
                Log.e(_TAG, "internalTransmit(): throw CardException(channel not open)");
                throw new CardException("channel not open");
            }
            try {
                    String response = manager.iccTransmitApduLogicalChannel(
                            subId[0], channelId[channelNumber], cla, ins, p1, p2, p3, data);
                    return ByteArrayConverter.hexStringToByteArray(response);
            } catch (Exception ex) {
                Log.e(_TAG, "internalTransmit(): throw CardException(transmit command failed)");
                throw new CardException("transmit command failed");
            }
        }
    }

    /**
     * Returns the ATR of the connected card or null if the ATR is not
     * available.
     *
     * @return the ATR of the connected card or null if the ATR is not
     *         available.
     */
    @Override
    public byte[] getAtr() {
        try {
                byte[] atr = manager.getAtr(subId[0]);
                return atr;
        } catch (Exception e) {
            Log.e(_TAG, "getAtr(): throw IllegalStateException");
            throw new IllegalStateException("internal error: getAtr() execution: "
                    + e.getCause());
        }
    }

    /**
     * Exchanges APDU (SELECT, READ/WRITE) to the
     * given EF by File ID and file path via iccIO.
     *
     * The given command is checked and might be rejected.
     *
     * @param fileID
     * @param filePath
     * @param cmd
     * @return
     */
    @Override
    public byte[] simIOExchange(int fileID,String filePath,byte[] cmd)
            throws Exception {
        try {
            int ins = 0;
            int p1=cmd[2] & 0xff;
            int p2=cmd[3] & 0xff;
            int p3=cmd[4] & 0xff;
            switch(cmd[1]) {
                case (byte)0xB0: ins=176; break;
                case (byte)0xB2: ins=178; break;
                case (byte)0xA4: ins=192;  p1=0; p2=0; p3=15; break;
                default: {
                    Log.e(_TAG, "simIOExchange(): throw SecureElementException");
                    throw new SecureElementException("Unknown SIM_IO command");
                }
            }

            if(filePath != null && filePath.length() > 0) {
                currentSelectedFilePath = filePath;
            }

                byte[] ret = manager.iccExchangeSimIO(subId[0], fileID, ins, p1, p2, p3, currentSelectedFilePath);
                return ret;
        } catch (Exception e) {
            Log.e(_TAG, "simIOExchange(): throw Exception(SIM IO access error)");
            throw new Exception("SIM IO access error");
        }
    }


    /**
     * Extracts the channel number from a CLA byte. Specified in GlobalPlatform
     * Card Specification 2.2.0.7: 11.1.4 Class Byte Coding
     *
     * @param cla
     *            the command's CLA byte
     * @return the channel number within [0x00..0x0F]
     */
    private int parseChannelNumber(byte cla) {
        // bit 7 determines which standard is used
        boolean isFirstInterindustryClassByteCoding = ((cla & 0x40) == 0x00);

        if(isFirstInterindustryClassByteCoding){
            // First Interindustry Class Byte Coding
            // see 11.1.4.1: channel number is encoded in the 2 rightmost bits
            return cla & 0x03;
        }else{
            // Further Interindustry Class Byte Coding
            // see 11.1.4.2: channel number is encoded in the 4 rightmost bits
            return (cla & 0x0F) + 4;
        }
    }

    @Override
    protected int internalOpenLogicalChannel() throws Exception {

        mSelectResponse = null;
        Log.e(_TAG, "internalOpenLogicalChannel(): throw UnsupportedOperationException");
        throw new UnsupportedOperationException(
                "open channel without select AID is not supported by UICC");
    }

    @Override
    protected int internalOpenLogicalChannel(byte[] aid, byte p2) throws CardException,
        NullPointerException, NoSuchElementException, MissingResourceException, IOException{
        IccOpenLogicalChannelResponse response;
        if (aid == null) {
            Log.e(_TAG, "internalOpenLogicalChannel(aid): throw NullPointerException");
            throw new NullPointerException("aid must not be null");
        }
        mSelectResponse = null;
        for (int i = 1; i < channelId.length; i++) {
            if (channelId[i] == 0) {
                try {
                        response = manager.iccOpenLogicalChannelWithP2(subId[0], ByteArrayConverter.byteArrayToHexString(aid), p2);
                } catch (RemoteException e) {
                    throw new CardException("iccOpenLogicalChannel failed, RemoteException:"
                                            + e.getMessage());
                }

                channelId[i] = response.getChannel();

                if (!(channelId[i] > 0)) { // channelId[i] == 0
                    channelId[i] = 0;
                    int status = response.getStatus();
                    mSelectResponse = response.getSelectResponse();

                if ((status == IccOpenLogicalChannelResponse.STATUS_MISSING_RESOURCE) &&
                        (mSelectResponse != null)) {
                        Log.e(_TAG, "internalOpenLogicalChannel(aid): throw MissingResourceException");
                        throw new MissingResourceException(
                                "all channels are used", "", "");
                    }
                if ((status == IccOpenLogicalChannelResponse.STATUS_MISSING_RESOURCE) &&
                        (mSelectResponse == null)) {
                        Log.e(_TAG, "internalOpenLogicalChannel(aid): throw IOException");
                        throw new IOException("IO Error");
                    }
                   if (status == IccOpenLogicalChannelResponse.STATUS_NO_SUCH_ELEMENT) {
                        Log.e(_TAG, "internalOpenLogicalChannel(aid): throw NoSuchElementException");
                        throw new NoSuchElementException("applet not found");
                    }
                    Log.e(_TAG, "internalOpenLogicalChannel(aid): throw CardException");
                    throw new CardException("open channel failed");
                }
                else{
                    byte[] getResponseCmd = new byte[] { 0x00, (byte) 0xC0, 0x00, 0x00, 0x00 };

                    if (i < 4) {
                        // b7 = 0 indicates the first interindustry class byte coding
                        getResponseCmd[0] = (byte) (i);
                    } else if (i < 20) {
                        // b7 = 1 indicates the further interindustry class byte coding
                        getResponseCmd[0] = (byte) (0x40 | (byte)(i - 4));
                    } else {
                        channelId[i] = 0;
                        Log.e(_TAG, "internalOpenLogicalChannel(aid): throw CardException(Channel number index must be within [1..19])");
                        throw new CardException( "Channel number index must be within [1..19]");
                    }
                    mSelectResponse = internalTransmit(getResponseCmd);
                }

                return i;
            }
        }
        Log.e(_TAG, "internalOpenLogicalChannel(aid): throw MissingResourceException(out of channels)");
        throw new MissingResourceException("out of channels", "","");
    }

    @Override
    protected void internalCloseLogicalChannel(int channelNumber)
            throws CardException {
        if (channelNumber == 0) {
            return;
        }
        if (channelId[channelNumber] == 0) {
            Log.e(_TAG, "internalCloseLogicalChannel(): throw CardException(channel not open)");
            throw new CardException("channel not open");
        }
        try {
                if (manager.iccCloseLogicalChannel(subId[0], channelId[channelNumber]) == false) {
                    Log.e(_TAG, "internalCloseLogicalChannel(): throw CardException");
                    throw new CardException("close channel failed");
                }
        } catch (Exception ex) {
            Log.e(_TAG, "internalCloseLogicalChannel(): throw CardException");
            throw new CardException("close channel failed");
        }
        channelId[channelNumber] = 0;
    }

    @Override
    protected void internalOpenBasicChannel() throws Exception {

        mSelectResponse = null;
        Log.e(_TAG, "internalOpenbasicChannel(): throw UnsupportedOperationException");
        throw new UnsupportedOperationException(
                "open basic channel is not supported by UICC");
    }
    @Override
    protected void internalOpenBasicChannel(byte[] aid) throws Exception {

        mSelectResponse = null;
        Log.e(_TAG, "internalOpenLogicalChannel(): throw UnsupportedOperationException");
        throw new UnsupportedOperationException(
                "open channel without select AID is not supported by UICC");
    }

    public int getsHandle(){
        return 0;
    }

    public void setsHandle(int sessionHandle){
    }

    public void internalCloseSession(int sessionHandle){}

    public void internalOpenSession(){}

    @Override
    protected byte[] internalGetSelectResponse() throws Exception {
        return mSelectResponse;
    }
}
