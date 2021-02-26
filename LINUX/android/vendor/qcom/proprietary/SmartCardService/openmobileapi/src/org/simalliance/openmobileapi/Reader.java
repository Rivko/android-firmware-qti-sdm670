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

package org.simalliance.openmobileapi;

import java.io.IOException;
import org.simalliance.openmobileapi.service.ISmartcardServiceReader;
import org.simalliance.openmobileapi.service.ISmartcardServiceSession;
import org.simalliance.openmobileapi.service.SmartcardError;
import android.os.RemoteException;
import android.util.Log;


/**
 * Instances of this class represent Secure Element Readers supported to this
 * device. These Readers can be physical devices or virtual devices. They can be
 * removable or not. They can contain Secure Element that can or cannot be
 * removed.
 *
 * @see <a href="http://simalliance.org">SIMalliance Open Mobile API  v3.0</a>
 */
public class Reader {

    private final String mName;
    private final SEService mService;
    private ISmartcardServiceReader mReader;
    public static final String TAG = "SmartcardService - Reader";

    private final Object mLock = new Object();


    Reader(SEService service, String name ) {
        mName = name;
        mService = service;
        mReader = null;

    }

    /**
     * Return the name of this reader.
     * <ul>
     * <li>If this reader is a SIM reader, then its name must be "SIM[Slot]".</li>
     * <li>If the reader is a SD or micro SD reader, then its name must be "SD[Slot]"</li>
     * <li>If the reader is a embedded SE reader, then its name must be "eSE[Slot]"</li>
     * </ul>
     * Slot is a decimal number without leading zeros. The Numbering must start with 1
     * (e.g. SIM1, SIM2, ... or SD1, SD2, ... or eSE1, eSE2, ...).
     * The slot number “1” for a reader is optional
     * (SIM and SIM1 are both valid for the first SIM-reader,
     * but if there are two readers then the second reader must be named SIM2).
     * This applies also for other SD or SE readers.
     *
     * @return the reader name, as a String.
     */
    public String getName() {
        return mName;
    }

    /**
     * Connects to a Secure Element in this reader. <br>
     * This method prepares (initialises) the Secure Element for communication
     * before the Session object is returned (e.g. powers the Secure Element by
     * ICC ON if its not already on). There might be multiple sessions opened at
     * the same time on the same reader. The system ensures the interleaving of
     * APDUs between the respective sessions.
     *
     * @throws IOException if something went wrong with the communicating to the
     *             Secure Element or the reader.
     * @return a Session object to be used to create Channels.
     */
    public Session openSession() throws IOException {

        if( mService == null || !mService.isConnected()){
            throw new IllegalStateException("service is not connected");
        }
        if( mReader == null ){
            try {
                mReader = mService.getReader(mName);
            } catch (Exception e) {
                throw new IOException("service reader cannot be accessed.");
            }
        }

        synchronized (mLock) {
            SmartcardError error = new SmartcardError();
            ISmartcardServiceSession session;
            int handle = 0;
            try {
                session = mReader.openSession(error);
                if (error.isSet()) {
                    error.throwException();
                }
            } catch (RemoteException e) {
                throw new IOException( e.getMessage() );
            }

            if( session == null ){
                throw new IOException( "service session is null." );
            }

            try {
                handle = session.getHandle();
            } catch (Exception e) {
                throw new IOException( "Unable to get the Session handle" );
            }
            return new Session(mService, session, this, handle);
        }
    }

    /**
     * Check if a Secure Element is present in this reader.
     *
     * @return <code>true</code> if the SE is present, <code>false</code> otherwise.
     */
    public boolean isSecureElementPresent() {
        if (mService == null || !mService.isConnected()) {
            throw new IllegalStateException("service is not connected");
        }

        if( mReader == null) {
            try {
                mReader = mService.getReader(mName);
            } catch (Exception e) {
                throw new IllegalStateException("service reader cannot be accessed. " + e.getLocalizedMessage());
            }
        }

        try {
            return mReader.isSecureElementPresent();
        } catch (RemoteException e) {
            throw new IllegalStateException(e.getMessage());
        }
    }

    /**
     * Return the Secure Element service this reader is bound to.
     *
     * @return the SEService object.
     */
    public SEService getSEService() {
        return mService;
    }

    /**
     * Close all the sessions opened on this reader. All the channels opened by
     * all these sessions will be closed.
     */
    public void closeSessions() {
        if (mService == null || !mService.isConnected()) {
            throw new IllegalStateException("service is not connected");
        }
        if( mReader != null ) {
            synchronized (mLock) {
                SmartcardError error = new SmartcardError();
                try {
                    mReader.closeSessions(error);
                } catch (RemoteException e) {
                throw new IllegalStateException(e.getMessage());
                }
            }
        }
    }
}
