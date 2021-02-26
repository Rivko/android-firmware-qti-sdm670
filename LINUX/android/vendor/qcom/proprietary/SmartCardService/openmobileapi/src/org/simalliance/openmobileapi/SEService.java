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
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;

import org.simalliance.openmobileapi.service.ISmartcardService;
import org.simalliance.openmobileapi.service.ISmartcardServiceCallback;
import org.simalliance.openmobileapi.service.ISmartcardServiceReader;
import org.simalliance.openmobileapi.service.SmartcardError;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

/**
 * The SEService realises the communication to available Secure Elements on the
 * device. This is the entry point of this API. It is used to connect to the
 * infrastructure and get access to a list of Secure Element Readers.
 *
 * @see <a href="http://simalliance.org">SIMalliance Open Mobile API  v3.0</a>
 */
public class SEService {

    private static final String SERVICE_TAG = "SmartcardService - SEService";

    private final Object mLock = new Object();

    /** The client context (e.g. activity). */
    private final Context mContext;

    /** The backend system. */
    private volatile ISmartcardService mSmartcardService;

    /**
     * Class for interacting with the main interface of the backend.
     */
    private ServiceConnection mConnection;

    /**
     * Collection of available readers
     */
    final private HashMap<String, Reader> mReaders = new HashMap<String,Reader>();

    /**
     * This implementation is used to receive callbacks from backend.
     */
    private final ISmartcardServiceCallback mCallback
        = new ISmartcardServiceCallback.Stub() {
    };

    /**
     * Callback object that allows the notification of the caller if this
     * SEService could be bound to the backend.
     */
    private CallBack mCallerCallback;

    /**
     * Interface to receive call-backs when the service is connected. If the
     * target language and environment allows it, then this shall be an inner
     * interface of the SEService class.
     */
    public interface CallBack {

        /**
         * Called by the framework when the service is connected.
         *
         * @param service
         *            the connected service.
         */
        void serviceConnected(SEService service);
    }

    /**
     * Establishes a new connection that can be used to connect to all the
     * Secure Elements available in the system. The connection process can be
     * quite long, so it happens in an asynchronous way. It is usable only if
     * the specified listener is called or if isConnected() returns
     * <code>true</code>. <br>
     * The call-back object passed as a parameter will have its
     * serviceConnected() method called when the connection actually happen.
     *
     * @param context
     *            the context of the calling application. Cannot be
     *            <code>null</code>.
     * @param listener
     *            a SEService.CallBack object. Can be <code>null</code>.
     */
    public SEService(Context context, SEService.CallBack listener) {

        if (context == null) {
            throw new NullPointerException("context must not be null");
        }

        mContext = context;
        mCallerCallback = listener;

        mConnection = new ServiceConnection() {

            public synchronized void onServiceConnected(
                    ComponentName className, IBinder service) {

                mSmartcardService = ISmartcardService.Stub.asInterface(service);
                if (mCallerCallback != null) {
                    mCallerCallback.serviceConnected(SEService.this);
                }
                Log.i(SERVICE_TAG, "Service onServiceConnected");
            }

            public void onServiceDisconnected(ComponentName className) {
                mSmartcardService = null;
                Log.i(SERVICE_TAG, "Service onServiceDisconnected");
            }
        };

        Intent intent = new Intent(ISmartcardService.class.getName());
        intent.setClassName("org.simalliance.openmobileapi.service",
                            "org.simalliance.openmobileapi.service.SmartcardService");
        boolean bindingSuccessful = mContext.bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
        if (bindingSuccessful) {
            Log.i(SERVICE_TAG, "bindService successful");
        }
    }

    /**
     * Tells whether or not the service is connected.
     *
     * @return <code>true</code> if the service is connected.
     */
    public boolean isConnected() {
        return mSmartcardService != null;
    }

    /**
     * Returns the list of available Secure Element readers.
     * There must be no duplicated objects in the returned list.
     * All available readers shall be listed even if no card is inserted.
     *
     * @return The readers list, as an array of Readers. If there are no readers the returned array is of length 0.
     */
    public Reader[] getReaders() {
        if (mSmartcardService == null) {
            throw new IllegalStateException("service not connected to system");
        }
        String[] readerNames;
        try {
            readerNames = mSmartcardService.getReaders();
        } catch (RemoteException e) {
            throw new RuntimeException(e);
        }

        Reader[] readers = new Reader[readerNames.length];
        int i = 0;
        for (String readerName : readerNames) {
            if(mReaders.get(readerName)==null) {
                mReaders.put(readerName, new Reader( this, readerName ));
                readers[i++] = mReaders.get(readerName);
            } else {
                readers[i++] = mReaders.get(readerName);
            }
        }
        return readers;
    }

    /**
     * Releases all Secure Elements resources allocated by this SEService
     * (including any binding to an underlying service).
     * As a result isConnected() will return false after shutdown() was called.
     * After this method call, the SEService object is not connected.
     * It is recommended to call this method in the termination method of the calling application
     * (or part of this application) which is bound to this SEService.
     */
    public void shutdown() {
        synchronized (mLock) {
            if (mSmartcardService != null ) {
                Collection<Reader> col = mReaders.values();
                Iterator<Reader> iter = col.iterator();
                while( iter.hasNext() ) {
                    try {
                        Reader reader = iter.next();
                        reader.closeSessions();
                    } catch (Exception ignore) {
                    }
                }
            }
            try {
                mContext.unbindService(mConnection);
            } catch (IllegalArgumentException e) {
                // Do nothing and fail silently since an error here indicates
                // that binding never succeeded in the first place.
            }
            mSmartcardService = null;
        }
    }

    /**
     * Returns the version of the OpenMobile API specification this
     * implementation is based on.
     *
     * @return String containing the OpenMobile API version (e.g. "3.0").
     */
    public String getVersion() {
        return "3.0";
    }

    // ******************************************************************
    // package private methods
    // ******************************************************************

    public ISmartcardServiceReader getReader(String name) throws IOException {
        try {
            SmartcardError error = new SmartcardError();
            ISmartcardServiceReader reader = mSmartcardService.getReader(name, error);
            if (error.isSet()) {
                error.throwException();
            }
            return reader;
        } catch (RemoteException e) {
            throw new IllegalStateException(e.getMessage());
        }
    }

    ISmartcardServiceCallback getCallback() {
        return mCallback;
    }
}
