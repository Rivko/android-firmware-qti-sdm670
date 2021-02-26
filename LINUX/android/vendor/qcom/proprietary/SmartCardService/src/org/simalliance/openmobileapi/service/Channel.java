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

package org.simalliance.openmobileapi.service;
import android.os.Binder;
import android.os.IBinder;
import android.os.RemoteException;
import org.simalliance.openmobileapi.service.ISmartcardServiceCallback;
import org.simalliance.openmobileapi.service.SmartcardService.SmartcardServiceSession;

import android.util.Log;


import java.security.AccessControlException;
import java.util.NoSuchElementException;
import java.io.IOException;

import org.simalliance.openmobileapi.service.security.AccessControlEnforcer;
import org.simalliance.openmobileapi.service.security.ChannelAccess;


/**
 * Smartcard service base class for channel resources.
 */
class Channel implements IChannel, IBinder.DeathRecipient {

    protected final int mChannelNumber;

    protected boolean mIsClosed;

    protected long mHandle;

    protected ISmartcardServiceSession mSession;
    protected Terminal mTerminal;

    protected byte[] mSelectResponse;

    protected final IBinder mBinder;


    protected ChannelAccess mChannelAccess = null;
    protected int mCallingPid = 0;


    protected ISmartcardServiceCallback mCallback;

    protected boolean mHasSelectedAid = false;
    protected byte[] mAid = null;

    private final Object mLock = new Object();

    public static final String _TAG = "SmartcardService - IChannel";


    Channel(SmartcardServiceSession session, Terminal terminal, int channelNumber, ISmartcardServiceCallback callback) {
        this.mChannelNumber = channelNumber;
        this.mSession = session;
        this.mTerminal = terminal;
        this.mCallback = callback;
        this.mBinder = callback.asBinder();
        try {
            this.mSelectResponse = terminal.getSelectResponse();
        } catch (Exception e){
            Log.e(_TAG, "Failed to get the select Response " +e);
        }
        this.mIsClosed = false;
        try {
            mBinder.linkToDeath(this, 0);
        } catch (RemoteException e) {
            Log.e(_TAG, "Failed to register client callback");
        }
    }

    private String ByteArrayToString(byte[] b, int start) {
        StringBuffer s = new StringBuffer();
        for (int i = start; i < b.length; i++) {
            s.append(Integer.toHexString(0x100 + (b[i] & 0xff)).substring(1));
        }
        return s.toString();
    }

    public void binderDied() {
        // Close this channel if the client died.
        try {
            Log.e(_TAG, Thread.currentThread().getName()
                    + " Client " + mBinder.toString() + " died");
            close();
        } catch (Exception ignore) {
        }
    }

    public synchronized void close() throws CardException {


        Terminal terminal = getTerminal();
        if( terminal == null ){
            Log.e(_TAG, "close(): throw IllegalStateException");
            throw new IllegalStateException( "channel is not attached to a terminal");
        }

        synchronized (mLock) {
            if (isBasicChannel() && hasSelectedAid()) {
                try {
                    Log.i(_TAG, "Close basic channel - Select with out AID ...");
                    terminal.select();
                } catch (NoSuchElementException exp) {
                    // Selection of the default application fails
                    try {
                        Log.i(_TAG, "Close basic channel - Exception : " + exp.getLocalizedMessage());
                        AccessControlEnforcer access = terminal.getAccessControlEnforcer();
                        if( access != null ){
                            terminal.select(AccessControlEnforcer.getDefaultAccessControlAid());
                        }
                    } catch (NoSuchElementException exp2) {
                        // Access Control Applet not available => Nothing to do.
                    }
                }
            }

            try {
                terminal.closeChannel(this);
                this.mIsClosed = true;
            } finally {
                mBinder.unlinkToDeath(this, 0);
            }
        }
    }

    public int getChannelNumber() {
        return mChannelNumber;
    }

    /**
     * Returns if this channel is a basic channel
     *
     * @return true if this channel is a basic channel
     */
    public boolean isBasicChannel() {
        return (mChannelNumber == 0) ? true : false;
    }

    public ISmartcardServiceCallback getCallback() {
        return mCallback;
    }

    /**
     * Returns the handle assigned to this channel.
     *
     * @return the handle assigned to this channel.
     */
    long getHandle() {
        return mHandle;
    }

    /**
     * Returns the associated terminal.
     *
     * @return the associated terminal.
     */
    public Terminal getTerminal() {
        return mTerminal;
    }

    /**
     * Assigns the channel handle.
     *
     * @param handle the channel handle to be assigned.
     */
    void setHandle(long handle) {
        this.mHandle = handle;
    }

    public byte[] transmit(byte[] command) throws CardException, IOException {
        boolean isNeedToCheckIntegrity = true;
        byte[] rsp = null;
        if( mChannelAccess == null ){
            Log.e(_TAG, "transmit(): throw AccessControlException(Channel access not set)");
            throw new AccessControlException( " Channel access not set.");
        }
        if (mChannelAccess.getCallingPid() !=  mCallingPid) {
            Log.e(_TAG, "transmit(): throw AccessControlException(Wrong Caller PID)");
            throw new AccessControlException(" Wrong Caller PID. ");
        }

        if (command.length < 4) {
            Log.e(_TAG, "transmit(): throw IllegalArgumentException(command must not be smaller than 4 bytes)");
            throw new IllegalArgumentException(
                    " command must not be smaller than 4 bytes");
        }

        if (((command[0] & (byte) 0x80) == 0)
                && ((byte) (command[0] & (byte) 0x60) != (byte) 0x20)) {
            // ISO command
            if (command[1] == (byte) 0x70) {
                Log.e(_TAG, "transmit(): throw IllegalArgumentException(MANAGE CHANNEL command not allowed)");
                throw new SecurityException(
                        "MANAGE CHANNEL command not allowed");
            }
            if ((command[1] == (byte) 0xA4) && (command[2] == (byte) 0x04)) {
                Log.e(_TAG, "transmit(): throw IllegalArgumentException(SELECT command not allowed)");
                throw new SecurityException(
                        "SELECT by DF name command not allowed");
            }
        } else if (command[0] == (byte)0xFF) {
            Log.e(_TAG, "transmit(): throw IllegalArgumentException(CLA(0xFF) not allowed)");
            throw new IllegalArgumentException("CLA(0xFF) not allowed");
        } else if ((command[1] == (byte) 0x9F) || (command[1] == (byte) 0x6F)) {
            throw new IllegalArgumentException("the command INS is invalid (INS = 0x6F or INS = 0x9F)");
        } else if (command[0] != (byte) 0x80) {
            isNeedToCheckIntegrity = false;
        }

        if ((!SmartcardService.mIsisConfig.equals("none")) &&
            ((byte) (command[0] & (byte) 0x60) != (byte) 0x20)) {
            isNeedToCheckIntegrity = true;
        }

        if (isNeedToCheckIntegrity) {
            if (command.length > 4) {
                int sizeOfLcField = 0;
                int commandDataLength = 0;
                int startIndexLeField = 0;
                int maxExpectedResponseDataLength = 0;

                if (command.length == 5) {
                    maxExpectedResponseDataLength = command[4] & 0xff;
                    if (maxExpectedResponseDataLength == 0)
                        maxExpectedResponseDataLength = 256;
                } else if ((command.length == 7) &&
                         (command[4] == (byte) 0x00)) { // only with extended Le (3 bytes)

                    maxExpectedResponseDataLength = command[startIndexLeField + 1] & 0xff;
                    maxExpectedResponseDataLength = maxExpectedResponseDataLength * 256;
                    maxExpectedResponseDataLength += command[startIndexLeField + 2] & 0xff;

                    if (maxExpectedResponseDataLength == 0)
                        maxExpectedResponseDataLength = 65536;

                } else if (command[4] == (byte) 0x00) { // if this has extended Lc field
                    if ((command.length > 7) &&
                        ((command[5] != 0) || (command[6] != 0))) {  // extended Lc must not be zero
                        commandDataLength = command[5] & 0xff;
                        commandDataLength = commandDataLength * 256;
                        commandDataLength += command[6] & 0xff;

                        if ((command.length != commandDataLength + 7) &&  // without Le
                            (command.length != commandDataLength + 8) &&  // with Le
                            (command.length != commandDataLength + 9)) {  // with extended Le (2 bytes)
                            Log.e(_TAG, "transmit(): throw IllegalArgumentException(Invalid command data field with extended Lc)");
                            throw new IllegalArgumentException("Invalid command data field with extended Lc");
                        }
                        else {
                            startIndexLeField = 7 + commandDataLength;
                            sizeOfLcField = 3;
                        }
                    }
                    else {
                        Log.e(_TAG, "transmit(): throw IllegalArgumentException(Invalid extended Lc field)");
                        throw new IllegalArgumentException("Invalid extended Lc field");
                    }
                } else {
                    commandDataLength = command[4] & 0xff;

                    if ((command.length != commandDataLength + 5) &&  // without Le
                        (command.length != commandDataLength + 6) &&  // with Le
                        (command.length != commandDataLength + 7)) {  // with extended Le (2 bytes)
                        Log.e(_TAG, "transmit(): throw IllegalArgumentException(Invalid command data field)");
                        throw new IllegalArgumentException("Invalid command data field");
                    }
                    else {
                        startIndexLeField = 5 + commandDataLength;
                        sizeOfLcField = 1;
                    }
                }

                if (startIndexLeField > 0) {
                    if (command.length != startIndexLeField) {
                        if ((sizeOfLcField == 3) &&
                            (command.length != startIndexLeField + 2)) {
                            Log.e(_TAG, "transmit(): throw IllegalArgumentException(Invalid Le field with extended Lc field)");
                            throw new IllegalArgumentException("Invalid Le field with extended Lc field");
                        }

                        // if this has extended Le field with Lc present
                        if (command.length == startIndexLeField + 2) {
                            maxExpectedResponseDataLength = command[startIndexLeField] & 0xff;
                            maxExpectedResponseDataLength = maxExpectedResponseDataLength * 256;
                            maxExpectedResponseDataLength += command[startIndexLeField + 1] & 0xff;

                            if (maxExpectedResponseDataLength == 0) {
                                maxExpectedResponseDataLength = 65536;
                            } else if (command.length == startIndexLeField + 1) { // short Le
                                maxExpectedResponseDataLength = command[startIndexLeField] & 0xff;
                            } else {
                                Log.e(_TAG, "transmit(): throw IllegalArgumentException(Invalid Le field)");
                                throw new IllegalArgumentException("Invalid Le field");
                            }
                        }
                    }
                }
            }
        }

        checkCommand(command);

        synchronized (mLock) {
            // set channel number bits
            command[0] = setChannelToClassByte(command[0], mChannelNumber);
            rsp = getTerminal().transmit(command, 2, 0, 0, null);
        }
       if (rsp.length == 2 && rsp [0] == 0x00 && rsp[1] == 0x00)
           throw new IOException("Communication error");
        return rsp;
    }

    public boolean selectNext() throws CardException, IOException {

        if( mChannelAccess == null ){
            throw new AccessControlException( " Channel access not set.");
        }
        if (mChannelAccess.getCallingPid() !=  mCallingPid) {
            throw new AccessControlException(" Wrong Caller PID. ");
        }

        if( mAid == null || mAid.length == 0){
            throw new CardException(" no aid given");
        }

        byte[] mBufferSelectResponse = null;
        byte[] selectCommand = new byte[5 + mAid.length];
        selectCommand[0] = 0x00;
        selectCommand[1] = (byte) 0xA4;
        selectCommand[2] = 0x04;
        selectCommand[3] = 0x02; // next occurrence
        selectCommand[4] = (byte)mAid.length;
        System.arraycopy(mAid, 0, selectCommand, 5, mAid.length);

        // set channel number bits
        selectCommand[0] = setChannelToClassByte(selectCommand[0], mChannelNumber);

        mBufferSelectResponse = getTerminal().transmit(selectCommand, 2, 0, 0, "SELECT NEXT");

        int sw1 = mBufferSelectResponse[mBufferSelectResponse.length - 2] & 0xFF;
        int sw2 = mBufferSelectResponse[mBufferSelectResponse.length - 1] & 0xFF;
        int sw = (sw1 << 8) | sw2;

        if ((mBufferSelectResponse.length == 2) && (sw==0x0000)) {
            Log.e(_TAG, "selectNext fail - IOERROR");
            throw new IOException("selectNext fail - IOERROR");
        }
        if (((sw & 0xF000) == 0x9000) || ((sw & 0xFF00) == 0x6200)
                 || ((sw & 0xFF00) == 0x6300)){

            if (!SmartcardService.mIsisConfig.equals("none")) {
                // check access control with selected AID
                byte[] selectedAid = getTerminal().getSelectedAid(mBufferSelectResponse);

                if (selectedAid == null) {
                    try {
                        getTerminal().closeChannel(this);
                        this.mIsClosed = true;
                    } finally {
                        mBinder.unlinkToDeath(this, 0);
                    }
                    throw new SecurityException("selected AID is not found");
                }

                try {
                    Log.i(_TAG, "check access control for selected AID");
                    this.mChannelAccess = getTerminal().setUpChannelAccess(
                                null,
                                selectedAid,
                                this.mChannelAccess.getPackageName(),
                                false,
                                this.mCallback );
                    Log.i(_TAG, "Access control successfully enabled for selected AID");
                    this.mChannelAccess.setCallingPid(Binder.getCallingPid());
                } catch (Exception e) {
                    try {
                        getTerminal().closeChannel(this);
                        this.mIsClosed = true;
                    } finally {
                        mBinder.unlinkToDeath(this, 0);
                    }
                    throw new SecurityException("selected AID is not allowed");
                }
            }

            mSelectResponse = mBufferSelectResponse.clone();
            mBufferSelectResponse = null;
            return true;
        } else if ((sw & 0xFF00) == 0x6A00) {
            mBufferSelectResponse = null;
            return false;
        } else {
            throw new UnsupportedOperationException(" unsupported operation");
        }
    }


    /**
     * Returns a copy of the given CLA byte where the channel number bits are
     * set as specified by the given channel number
     *
     * See GlobalPlatform Card Specification 2.2.0.7: 11.1.4 Class Byte Coding
     *
     * @param cla
     *            the CLA byte. Won't be modified
     * @param channelNumber
     *            within [0..3] (for first interindustry class byte coding) or
     *            [4..19] (for further interindustry class byte coding)
     * @return the CLA byte with set channel number bits. The seventh bit
     *         indicating the used coding (first/further interindustry class
     *         byte coding) might be modified
     */
    private byte setChannelToClassByte(byte cla, int channelNumber) {
        if (channelNumber < 4) {
            // b7 = 0 indicates the first interindustry class byte coding
            cla = (byte) ((cla & 0xBC) | channelNumber);
        } else if (channelNumber < 20) {
            // b7 = 1 indicates the further interindustry class byte coding
            boolean isSM = (cla & 0x0C) != 0;
            cla = (byte) ((cla & 0xB0) | 0x40 | (channelNumber - 4));
            if (isSM) cla |= 0x20;
        } else {
            throw new IllegalArgumentException(
                    "Channel number must be within [0..19]");
        }
        return cla;
    }


    public void setChannelAccess(ChannelAccess channelAccess) {
        this.mChannelAccess = channelAccess;
    }

    public ChannelAccess getChannelAccess(){
        return this.mChannelAccess;
    }

    public void setCallingPid( int pid) {



        mCallingPid = pid;
    }

    private void checkCommand( byte[] command ) {
        if( getTerminal().getAccessControlEnforcer() != null ) {
            // check command if it complies to the access rules.
            // if not an exception is thrown
            getTerminal().getAccessControlEnforcer().checkCommand(this, command);
        } else {
            throw new AccessControlException( "FATAL: Access Controller not set for Terminal: " + getTerminal().getName());
        }
    }


    /**
     * true if aid during open xxx channel coud be selected.
     * false if aid could not be or was not selected.
     * @return boolean.
     */
    public boolean hasSelectedAid() {
        return mHasSelectedAid;
    }

    /**
     * set selected aid flag and aid (may be null)
     */
    public void hasSelectedAid(boolean has, byte[] aid) {
        mHasSelectedAid = has;
        mAid = aid;
    }

    /**
     * Returns the data as received from the application select command inclusively the status word.
     * The returned byte array contains the data bytes in the following order:
     * [<first data byte>, ..., <last data byte>, <sw1>, <sw2>]
     * @return The data as returned by the application select command inclusively the status word.
     * @return Only the status word if the application select command has no returned data.
     * @return null if an application select command has not been performed or the selection response can not
     * be retrieved by the reader implementation.
     */
    public byte[] getSelectResponse()
    {
        return mSelectResponse;
    }

    boolean isClosed(){

        return mIsClosed;
    }

    void setClosed(){

        mIsClosed = true;
    }

    /**
     * Implementation of the SmartcardService Channel interface according to OMAPI.
     *
     *
     */
    final class SmartcardServiceChannel extends ISmartcardServiceChannel.Stub {

        private final SmartcardServiceSession mSession;
        public SmartcardServiceChannel( SmartcardServiceSession session ){
            mSession = session;
        }

        @Override
        public void close(SmartcardError error) throws RemoteException {

            try {
                Channel.this.close();
            } catch (Exception e) {
                SmartcardService.setError(error, e);
            } finally {
                if( mSession != null )
                    mSession.removeChannel(Channel.this);
            }
        }

        public void setClosed(){
            Channel.this.setClosed();
        }

        @Override
        public boolean isClosed() throws RemoteException {
            return Channel.this.isClosed();
        }

        @Override
        public boolean isBasicChannel()
                throws RemoteException {
            return Channel.this.isBasicChannel();
        }

        @Override
        public byte[] getSelectResponse()
                throws RemoteException {
            return Channel.this.getSelectResponse();
        }

        @Override
        public byte[] transmit(byte[] command, SmartcardError error)
                throws RemoteException {

            try {
                if (isClosed()) {
                    Log.e(_TAG, "transmit(): setError IllegalStateException");
                    SmartcardService.setError(error, IllegalStateException.class, "channel is closed");
                    return null;
                }

                if (command == null) {
                    Log.e(_TAG, "transmit(): setError NullPointerException");
                    SmartcardService.setError(error, NullPointerException.class, "command must not be null");
                    return null;
                }
                if (command.length < 4) {
                    Log.e(_TAG, "transmit(): setError IllegalArgumentException");
                    SmartcardService.setError(error, IllegalArgumentException.class,
                            "command must have at least 4 bytes");
                    return null;
                }

                Channel.this.setCallingPid(Binder.getCallingPid());


                byte[] response = Channel.this.transmit(command);

                return response;
            } catch (Exception e) {
                SmartcardService.setError(error, e);
                return null;
            }
        }

        @Override
        public boolean selectNext(SmartcardError error)
                throws RemoteException {

            try {
                if (isClosed()) {
                    Log.e(_TAG, "selectNext(): setError IllegalStateException");
                    SmartcardService.setError(error, IllegalStateException.class, "channel is closed");
                    return false;
                }


                Channel.this.setCallingPid(Binder.getCallingPid());

                boolean response = Channel.this.selectNext();
                return response;
            } catch (Exception e) {
                SmartcardService.setError(error, e);
                return false;
            }
        }
    };
}
