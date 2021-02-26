/* ExtendedRemoteDisplay.java
 * Implements RemoteDisplay APIs to use WFDService from Android framework
 *
 *
 * Copyright (c) 2013 - 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (C) 2012 The Android Open Source Project
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

package com.qualcomm.wfd;
import android.content.Intent;

import dalvik.system.CloseGuard;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.RemoteException;
import android.os.IBinder;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;

import android.os.SystemProperties;
import android.view.Surface;

import com.qualcomm.wfd.service.IWfdActionListener;
import com.qualcomm.wfd.service.ISessionManagerService;

import android.content.ComponentName;
import android.content.Context;
import android.content.ServiceConnection;
import android.media.RemoteDisplay;

import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

enum WFDState {
    UNINITIALIZED,
    BINDING,
    BOUND,
    INITIALIZED,
    ESTABLISHING,
    ESTABLISHED,
    PLAY,
    PLAYING,
    TEARDOWN,
    DEINIT,
}

enum ERDConstants {
    /*
    Callbacks from WFD service
     */
    PLAY_CALLBACK(0),
    PAUSE_CALLBACK(1),
    STANDBY_CALLBACK(2),
    UIBC_ACTION_COMPLETED_CALLBACK(3),
    TEARDOWN_CALLBACK(4),
    SERVICE_BOUND_CALLBACK(5),
    ESTABLISHED_CALLBACK(6),
    INIT_CALLBACK(7),
    MM_STREAM_STARTED_CALLBACK(8),
    INVALID_STATE_CALLBACK(9),
    TEARDOWN_START_CALLBACK(10),
    /*
    Commands from framework
     */
    START_CMD(11),
    END_CMD(12),
    /*
    Sentinel end value
     */
    INVALID_ERD_CONSTANT(-1);

    private final int value;

    private static final ERDConstants[] values = ERDConstants.values();

    private ERDConstants(int value) {
        this.value = value;
    }

    public final int value() {
        return value;
    }

    public static final ERDConstants getConstant(int value) {
    /*
    *
    * Why this try catch stuff? Having cached the values() method
    * (for optimization purposes because values() is an expensive call since
    * arrays are mutable and compiler would generate it every time)
    * client might pass in some devious value in which case an out of bounds
    * exception might be triggered (essentially when value >
    * ERDCmds.values().length()). Be compassionate and give it INVALID_CMD
     */
        try{
            return values[value];
        } catch (ArrayIndexOutOfBoundsException e) {
            return INVALID_ERD_CONSTANT;
        }
    }
};

class ServiceUtil {

    private static final String TAG = "ExtendedRemoteDisplay.ServiceUtil";
    private static ISessionManagerService uniqueInstance = null;
    private static boolean mServiceAlreadyBound = false;
    private static Handler eventHandler = null;

    protected static boolean getmServiceAlreadyBound() {
        return mServiceAlreadyBound;
    }

    public static void bindService(Context context, Handler inEventHandler)
        throws ServiceUtil.ServiceFailedToBindException {
            if (!mServiceAlreadyBound  || uniqueInstance == null) {
                Log.d(TAG,"Binding to WFD Service");

                Intent serviceIntent =
                    new Intent("com.qualcomm.wfd.service.WfdService");
                serviceIntent.setPackage("com.qualcomm.wfd.service");
                eventHandler = inEventHandler;
                if (!context.bindService(serviceIntent,
                                         mConnection, Context.BIND_AUTO_CREATE)) {
                    Log.e(TAG,"Failed to connect to Provider service");
                    throw new ServiceUtil.ServiceFailedToBindException(
                                         "Failed to connect to Provider service");
                }
            }
        }

    public static void unbindService(Context context) {
        if (mServiceAlreadyBound) {
            try {
                context.unbindService(mConnection);
            } catch (IllegalArgumentException e) {
                Log.e(TAG,"IllegalArgumentException: " + e);
            }
            mServiceAlreadyBound = false;
            uniqueInstance = null;
        }
    }

    public synchronized static ISessionManagerService getInstance() {
        while (uniqueInstance == null) {
            Log.d(TAG, "Waiting for service to bind ...");
            try {
                ServiceUtil.class.wait();
            } catch (InterruptedException e) {
                Log.e(TAG, "InterruptedException: " + e);
            }
        }
        return uniqueInstance;
    }

    public static class ServiceFailedToBindException extends Exception {
        public static final long serialVersionUID = 1L;

        private ServiceFailedToBindException(String inString)
        {
            super(inString);
        }
    }

    protected static ServiceConnection mConnection = new ServiceConnection() {
        public void onServiceConnected
                                  (ComponentName className, IBinder service) {
            Log.d(TAG, "Connection object created");
            mServiceAlreadyBound = true;
            uniqueInstance = ISessionManagerService.Stub.asInterface(service);
            synchronized(ServiceUtil.class) {
                ServiceUtil.class.notifyAll();
            }
            Message messageBound =
                        eventHandler.obtainMessage(ERDConstants.SERVICE_BOUND_CALLBACK.value());
            eventHandler.sendMessage(messageBound);
        }

        public void onServiceDisconnected(ComponentName className) {
            Log.d(TAG, "Remote service disconnected");
            mServiceAlreadyBound = false;

            Log.d(TAG, "Post TEARDOWN");
            Message messageTeardown =
                        eventHandler.obtainMessage(ERDConstants.TEARDOWN_CALLBACK.value());
            eventHandler.sendMessage(messageTeardown);

            Log.d(TAG, "Post INVALID");
            Message messageInvalid =
                        eventHandler.obtainMessage(ERDConstants.INVALID_STATE_CALLBACK.value());
            eventHandler.sendMessage(messageInvalid);
        }
    };

}


class WfdActionListenerImpl extends IWfdActionListener.Stub {
    Handler mCallbackHandler;
    private static final String TAG = "ExtendedRemoteDisplay.WfdActionListenerImpl";
    WfdActionListenerImpl(Handler handler) {
        super();
        mCallbackHandler = handler;
    }

    @Override
    public void onStateUpdate(int newState, int sessionId)
                                                   throws RemoteException {
        WfdEnums.SessionState state = WfdEnums.SessionState.values()[newState];
        switch (state) {
            case INITIALIZED:
                if (sessionId > 0) {
                    Log.d(TAG,state.toString() + " sessionId > 0");
                    Message messageTeardown =
                         mCallbackHandler.obtainMessage(ERDConstants.TEARDOWN_CALLBACK.value());
                    mCallbackHandler.sendMessage(messageTeardown);
                } else {
                    Log.d(TAG,state.toString() + " , Init callback");
                    Message messageInit =
                          mCallbackHandler.obtainMessage(ERDConstants.INIT_CALLBACK.value());
                    mCallbackHandler.sendMessage(messageInit);
                }
                break;

            case INVALID:
                Log.d(TAG,state.toString());
                Message messageInvalid = mCallbackHandler
                        .obtainMessage(ERDConstants.INVALID_STATE_CALLBACK.value());
                mCallbackHandler.sendMessage(messageInvalid);
                break;

            case IDLE:
                Log.d(TAG,state.toString());
                break;

            case PLAY:
                Log.d(TAG,state.toString());
                Message messagePlay =
                             mCallbackHandler.obtainMessage(ERDConstants.PLAY_CALLBACK.value());
                mCallbackHandler.sendMessage(messagePlay);
                break;

            case PAUSE:
                Log.d(TAG,state.toString());
                Message messagePause =
                            mCallbackHandler.obtainMessage(ERDConstants.PAUSE_CALLBACK.value());
                mCallbackHandler.sendMessage(messagePause);
                break;

            case STANDING_BY:
                Log.d(TAG,state.toString());
                Message messageStandby =
                          mCallbackHandler.obtainMessage(ERDConstants.STANDBY_CALLBACK.value());
                mCallbackHandler.sendMessage(messageStandby);
                break;

            case ESTABLISHED:
                Log.d(TAG,state.toString());
                Message messageEstablishedCallback =
                       mCallbackHandler.obtainMessage(ERDConstants.ESTABLISHED_CALLBACK.value());
                mCallbackHandler.sendMessage(messageEstablishedCallback);
                break;

            case TEARDOWN:
                Log.d(TAG,state.toString());
                Message messageTeardown =
                         mCallbackHandler.obtainMessage(ERDConstants.TEARDOWN_CALLBACK.value());
                mCallbackHandler.sendMessage(messageTeardown);
                break;
        }
    }

    @Override
    public void notifyEvent(int event, int sessionId) throws RemoteException {
        if (event == WfdEnums.WfdEvent.TEARDOWN_START.ordinal()) {
            Message msgTearDownStart = mCallbackHandler
                    .obtainMessage(ERDConstants.TEARDOWN_START_CALLBACK.value());
            mCallbackHandler.sendMessage(msgTearDownStart);
        }
    }

    @Override
    public void notify(Bundle b, int sessionId) throws RemoteException {
        if(b != null) {
            Log.d(TAG, "Notify from WFDService");
            String event = b.getString("event");
            if("MMStreamStarted".equalsIgnoreCase(event)) {
                Message messageEvent =
                     mCallbackHandler.obtainMessage(ERDConstants.MM_STREAM_STARTED_CALLBACK.value());
                messageEvent.setData(b);
                mCallbackHandler.sendMessage(messageEvent);
            }
        }
    }
}



/**
 * Listens for Wifi remote display connections managed by the media server.
 *
 * @hide
 */
public final class ExtendedRemoteDisplay {

    private static final String TAG = "ExtendedRemoteDisplay";

    /* these constants must be kept in sync with IRemoteDisplayClient.h */

    public static final int DISPLAY_FLAG_SECURE = 1 << 0;

    public static final int DISPLAY_ERROR_UNKOWN = 1;
    public static final int DISPLAY_ERROR_CONNECTION_DROPPED = 2;

    private static final int OP_TIMEOUT = 10000; // 10 seconds

    private static final int TEARDOWN_TIMEOUT = 9000; // 9 seconds
    private static final int UNINITIALIZED_TIMEOUT = 1000 + TEARDOWN_TIMEOUT;

    private CloseGuard mGuard = CloseGuard.get();
    private boolean mInvalid;
    private RemoteDisplay.Listener mListener;
    private Handler mDisplayHandler;
    private boolean mConnected;
    private ExtendedRemoteDisplayHandler mERDHandler;
    private HandlerThread mWorkerThread;
    private Context mContext;

    private Surface surface;
    private String mIface;

    private WfdDevice mLocalWfdDevice;
    private WfdDevice mPeerWfdDevice;
    private IWfdActionListener mActionListener;

    /*
    *
    * We can't afford to have a state associated with each ERD object since the
    * underlying WFD service is a single state machine
     */
    public static WFDState sState = WFDState.UNINITIALIZED;
    public static Object sERDLock = new Object();

    public static Lock      sLock = new ReentrantLock();
    public static Condition sConditionDeinit  = sLock.newCondition();
    public static Condition sConditionUninitialized  = sLock.newCondition();

    public ExtendedRemoteDisplay(RemoteDisplay.Listener listener,
                                 Handler handler,
                                 Context context) {
        mListener = listener;
        mDisplayHandler = handler;
        mContext = context;
        mConnected = true;
        mWorkerThread = new HandlerThread(TAG);
        mWorkerThread.setName("ERD_Worker_Thread");
        mWorkerThread.start();
        mERDHandler = new ExtendedRemoteDisplayHandler(mWorkerThread.getLooper());
        mInvalid = true;
    }

    @Override
    protected void finalize() throws Throwable {
        Log.i(TAG, "finalize called on " + this);
        if (!mInvalid) {
            Log.e(TAG, "!!!Finalized without being invalidated!!!");
            /*
            *
            * Plod on to teardown. Ideally this should be never be hit if state
            * machine works fine but something is pretty messed up here
            */
            processDispose(true);
        }
        mListener = null;
        super.finalize();
    }

    /**
     * Starts listening for displays to be connected on the specified interface.
     *
     * @param iface    The interface address and port in the form "x.x.x.x:y".
     * @param listener The listener to invoke
     *                 when displays are connected or disconnected.
     * @param handler  The handler on which to invoke the listener.
     * @param context  The current service context
     */
    public static ExtendedRemoteDisplay listen(String iface, RemoteDisplay.Listener listener,
                                               Handler handler, Context context) {
        if (iface == null) {
            throw new IllegalArgumentException("iface must not be null");
        }
        if (listener == null) {
            throw new IllegalArgumentException("listener must not be null");
        }
        if (handler == null) {
            throw new IllegalArgumentException("handler must not be null");
        }

        ExtendedRemoteDisplay display =
                new ExtendedRemoteDisplay(listener, handler, context);

        /*
        *
        * Before starting the next session, we need to ensure that previous
        * session has moved to DEINIT state. Ideally this should never be
        * hit, since we try to ensure in disposeInternal that the state machine moves
        * to DEINIT on teardown when disposeInternal has been invoked.
         */
        sLock.lock();

        try
        {
            long timeOutNanos =
                TimeUnit.MILLISECONDS.toNanos(UNINITIALIZED_TIMEOUT);

            while (sState != WFDState.UNINITIALIZED) {
                Log.d(TAG, "Waiting to move to UNINITIALIZED timetowait=" +
                      timeOutNanos + " ns");

                if (timeOutNanos <= 0L) {
                    Log.d(TAG, "Waiting to move to UNINITIALIZED - timedout");
                    break;
                }

                timeOutNanos = sConditionUninitialized.awaitNanos(timeOutNanos);
            }
        }
        catch (InterruptedException e) {
            Log.e(TAG, "Exception when waiting to move to UNINITIALIZED" +
                  e.getMessage());
        }

        sLock.unlock();

        if (sState != WFDState.UNINITIALIZED) {
            /*
            *
            * Now, here we have created the resources like threads and handlers
            * for the ERD object. We need to clean them up from here rather than
            * waiting for the finalizer daemon and then invoking cleanup.
            *
            */
            Log.e(TAG, "Something has gone kaput in listen!");
            display.dispose();
            return null;
        }

        Log.d(TAG, "Waiting for RTSP server to start ");

        display.startListening(iface);

        /*
        *
        * Now wait for the worker thread to signal the start of RTSP
        * server.
        */
        display.blockOnLock();

        if (sState != WFDState.ESTABLISHING) {
            /*
            *
            * Now, here we have created the resources like threads and handlers
            * for the ERD object. We need to clean them up from here rather than
            * waiting for the finalizer daemon and then invoking cleanup.
            *
            */
            Log.d(TAG, "Failed to start RTSP Server!");
            display.dispose();
            return null;
        }

        /*
        * Everything's hunky-dory!
        */

        Log.i(TAG, "New ERD instance " + display);
        return display;
    }

    /**
     * Starts WFDService and waits for incoming RTSP connections
     */
    public void startListening(String iface) {
        mIface = iface;

        Message messageStart = mERDHandler.obtainMessage(ERDConstants.START_CMD.value());
        mERDHandler.sendMessage(messageStart);

        mGuard.open("dispose");
    }

    /**
     * Disconnects the remote display and stops listening for new connections.
     */
    public void dispose() {
        processDispose(false);
    }

    /**
     * This method creates an anonymous thread to in order to unblock the caller.
     * This anonymous thread synchronously attempts to bring down the entire ERD
     * framework
     *
     * @param finalized Indicates whether GC has invoked finalize on the instance
     */
    private void processDispose(final boolean finalized) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                Thread.currentThread().setName("Dispose_Thread");
                Log.d(TAG, "Processing dispose with " + finalized + " from "
                        + Thread.currentThread().getName());
                disposeInternal(finalized);
            }
        }).start();
    }

    /**
     * @param finalized
     */
    private void disposeInternal(boolean finalized) {

        if (mGuard != null) {
            if (finalized) {
                mGuard.warnIfOpen();
            } else {
                mGuard.close();
            }
            mGuard = null;
        }

        if (mERDHandler != null) {
            Message messageEnd = mERDHandler.obtainMessage(ERDConstants.END_CMD.value());
            mERDHandler.sendMessage(messageEnd);
        }

        /*
        *
        * Block the disposeInternal call till entire TEARDOWN is complete, to avoid the
        * WFD stack being short changed if the caller is allowed to progress
        * further since the very next step from framework is to sever the P2P
        * connection and this leads to RTSP message timeouts, network failures
        * when it should be cleanly tearing down.
        */

        Log.d(TAG, "Waiting for teardown to complete");

        sLock.lock();

        try
        {
            long timeOutNanos = TimeUnit.MILLISECONDS.toNanos(TEARDOWN_TIMEOUT);

            while (sState != WFDState.DEINIT) {
                Log.d(TAG, "Waiting to move to DEINIT timetowait=" +
                           timeOutNanos + " ns");

                if (timeOutNanos <= 0L) {
                    Log.d(TAG, "Waiting to move to DEINIT - timedout");
                    break;
                }
                timeOutNanos = sConditionDeinit.awaitNanos(timeOutNanos);
            }

        } catch(InterruptedException e) {
            Log.e(TAG, "Exception when waiting to move to DEINIT" +
                  e.getMessage());
        }

        sLock.unlock();

        if (sState != WFDState.DEINIT) {
            Log.e(TAG, "Something has gone kaput! in teardown");
            /*
             * Since worker thread has failed to notify framework of a display
             * disconnect, we initiate it from here, only in cases of failure,
             * else, worker thread should do it normally. This also tries to
             * ensure that we leave ERD in a valid DEINIT cleaned up state.
             * Concerns of thread concurrency i.e. when the message is processed
             * should be taken care of in Handler API quitSafely()
             */
            Message messageInvalid = mERDHandler.obtainMessage
            (ERDConstants.INVALID_STATE_CALLBACK.value());
            mERDHandler.sendMessage(messageInvalid);
        }

        cleanupResources();
        Log.d(TAG, "Done with teardown");
        sLock.lock();
        setState(WFDState.UNINITIALIZED);
        Log.d(TAG, "Signal sConditionUninitialized");
        sConditionUninitialized.signal();
        sLock.unlock();
    }

    /**
     * @param surface
     * @param width
     * @param height
     * @param flags
     * @
     */
    private void notifyDisplayConnected(final Surface surface,
                                        final int width, final int height, final int flags) {
        Log.d(TAG, "notifyDisplayConnected");
        mDisplayHandler.post(new Runnable() {
            @Override
            public void run() {
                if (mListener != null) {
                    Log.d(TAG, "notifyDisplayConnected");
                    mListener.onDisplayConnected(surface, width, height, flags, 0);
                    mConnected = true;
                }
            }
        });
    }

    private void notifyDisplayDisconnected() {
        mDisplayHandler.post(new Runnable() {
            @Override
            public void run() {
                if (mConnected && mListener != null) {
                    Log.d(TAG, "notifyDisplayDisconnected");
                    mListener.onDisplayDisconnected();
                    mConnected = false;
                }
            }
        });
    }

    private void notifyDisplayError(final int error) {
        mDisplayHandler.post(new Runnable() {
            @Override
            public void run() {
                if (mListener != null) {
                    Log.d(TAG, "notifyDisplayError");
                    mListener.onDisplayError(error);
                }
            }
        });
    }

    /**
     * Class for Command handling.
     */
    class ExtendedRemoteDisplayHandler extends Handler {

        public ExtendedRemoteDisplayHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            ERDConstants cmd = ERDConstants.getConstant(msg.what);
            Log.d(TAG, "ERD handler received: " + cmd);
            switch (cmd) {

                case START_CMD: {
                    Log.d(TAG, "Starting......");

                    mLocalWfdDevice = createWFDDevice(mIface, WfdEnums.WFDDeviceType.SOURCE);
                    mPeerWfdDevice = createWFDDevice(mIface, WfdEnums.WFDDeviceType.PRIMARY_SINK);

                    if (mLocalWfdDevice == null || mPeerWfdDevice == null) {
                        Log.i(TAG, "Invalid WFD devices for iface: " + mIface);
                        unblockWaiters();
                        break;
                    }

                    try {
                        setState(WFDState.BINDING);
                        ServiceUtil.bindService(mContext, mERDHandler);
                    } catch (ServiceUtil.ServiceFailedToBindException e) {
                        Log.e(TAG, "ServiceFailedToBindException received");
                        unblockWaiters();
                    }
                }
                break;

                case END_CMD: {
                    Log.d(TAG, "Ending.........");
                    if (sState == WFDState.DEINIT) {
                        return;
                    }
                    if (sState == WFDState.BOUND) {
                        try {
                            if (ServiceUtil.getmServiceAlreadyBound()) {
                                Log.d(TAG, "Teardown WFD Session");
                                ServiceUtil.getInstance().teardown();
                            }
                        } catch (RemoteException e) {
                            Log.e(TAG, "RemoteException in teardown");
                        }
                        Message messageInvalid = mERDHandler
                                .obtainMessage(ERDConstants.INVALID_STATE_CALLBACK.value());
                        mERDHandler.sendMessage(messageInvalid);
                        return;
                    }
                    if (sState == WFDState.INITIALIZED) {
                        try {
                            ServiceUtil.getInstance().deinit();
                        } catch (RemoteException e) {
                            Log.e(TAG, "RemoteException in deInit");
                        }
                        return;
                    }

                    try {
                        if (ServiceUtil.getmServiceAlreadyBound()) {
                            Log.d(TAG, "Teardown WFD Session");
                            ServiceUtil.getInstance().teardown();
                        }
                    } catch (RemoteException e) {
                        Log.e(TAG, "RemoteException in teardown");
                    }
                }
                break;

                case PLAY_CALLBACK: {
                    setState(WFDState.PLAY);
                    Log.d(TAG, "WFDService in PLAY state");
                }
                break;

                case PAUSE_CALLBACK: {
                    Log.d(TAG, "WFDService in PAUSE state");
                }
                break;

                case STANDBY_CALLBACK: {
                    Log.d(TAG, "WFDService in STANDBY state");
                }
                break;

                case SERVICE_BOUND_CALLBACK: {
                    mActionListener = new WfdActionListenerImpl(mERDHandler);
                    setState(WFDState.BOUND);
                    try {
                        //set local device type
                        int setDeviceTypeRet =
                                ServiceUtil.getInstance().
                                        setDeviceType(WfdEnums.WFDDeviceType.SOURCE.getCode());
                        if (setDeviceTypeRet != 0) {
                            Log.d(TAG, "setDeviceType failed : " + setDeviceTypeRet);
                            unblockWaiters();
                            return;
                        }

                        int ret = ServiceUtil.getInstance().
                                initSys(ExtendedRemoteDisplay.this.mActionListener,
                                        ExtendedRemoteDisplay.this.mLocalWfdDevice);
                        if (ret != 0) {
                            Log.e(TAG, "Init failed");
                            unblockWaiters();
                            return;
                        }
                        setState(WFDState.INITIALIZED);
                        mInvalid = false;
                    } catch (RemoteException e) {
                        Log.e(TAG, "WfdService init() failed", e);
                        return;
                    }
                }
                break;

                case TEARDOWN_CALLBACK: {
                    try {
                        setState(WFDState.TEARDOWN);
                        if (ServiceUtil.getmServiceAlreadyBound()) {
                            ServiceUtil.getInstance().deinit();
                        }
                        if (surface != null) {
                            if (surface.isValid()) {
                                surface.release();
                                Log.e(TAG, "Released surface successfully");
                            } else {
                                Log.e(TAG, "surface not valid");
                            }
                        } else {
                            Log.e(TAG, "Why on earth is surface null??");
                        }
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    }

                }
                break;

                case MM_STREAM_STARTED_CALLBACK: {
                    setState(WFDState.PLAYING);
                    Bundle b = msg.getData();
                    int width = b.getInt("width");
                    int height = b.getInt("height");

                    /* Note that secure here doesn't Indicate the session is
                       always secure. It just means that link is protected and
                       WFDService can switch to secure if a secure content
                       is played
                    */
                    int secure = b.getInt("hdcp");

                    surface = (Surface) (b.getParcelable("surface"));
                    if (surface != null) {
                        Log.d(TAG,
                                "MM Stream Started Width, Height and Secure: "
                                        + " " + width + " " + height + " "
                                        + secure);
                        if (secure != 0) {
                            notifyDisplayConnected(surface, width, height,
                                    DISPLAY_FLAG_SECURE);
                        } else {
                            notifyDisplayConnected(surface, width, height, 0);
                        }
                    }
                }
                break;

                case INIT_CALLBACK: {
                    try {
                        int ret = ServiceUtil.getInstance().
                                startWfdSession(mPeerWfdDevice);
                        Log.d(TAG, "EventHandler: startWfdSession returned " + ret);
                        if (ret == 0) {
                            setState(WFDState.ESTABLISHING);
                        }
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    } finally {
                        unblockWaiters();
                    }
                }
                break;

                case ESTABLISHED_CALLBACK: {
                    setState(WFDState.ESTABLISHED);
                    Log.d(TAG, "EventHandler: startSession- completed");
                }
                break;

                case UIBC_ACTION_COMPLETED_CALLBACK: {
                    Log.d(TAG, "EventHandler: uibcActionCompleted- completed");
                }
                break;

                case INVALID_STATE_CALLBACK: {
                    notifyDisplayDisconnected();
                    mInvalid = true;
                    if (ServiceUtil.getmServiceAlreadyBound()) {
                        try {
                            ServiceUtil.getInstance().unregisterListener(
                                    ExtendedRemoteDisplay.this.mActionListener);
                        } catch (RemoteException e) {
                            Log.e(TAG, "RemoteException in un-registering listener" + e);
                        }
                    }
                    Log.d(TAG, "Unbind the WFD service");
                    ServiceUtil.unbindService(mContext);

                    sLock.lock();
                    setState(WFDState.DEINIT);
                    Log.d(TAG, "Signal sConditionDeinit");
                    sConditionDeinit.signal();
                    sLock.unlock();

                    Log.d(TAG, "ERD instance invalidated");
                }
                break;

                case TEARDOWN_START_CALLBACK: {
                    Log.d(TAG, "Received TEARDOWN_START_CALLBACK");
                    notifyDisplayDisconnected();// To trigger display disconnect
                }
                break;

                default:
                    Log.wtf(TAG, "Unknown cmd received: " + msg.what);
            }
        }
    }

    /**
     * Helper method to create a WFDDevice from iface and device type
     *
     * @param iface IPAddress:Port
     * @param type  Device type
     * @return WfdDevice forged from the input parameters
     */
    private WfdDevice createWFDDevice(String iface, WfdEnums.WFDDeviceType type) {
        WfdDevice wfdDevice = new WfdDevice();
        wfdDevice.deviceType = type.getCode();
        wfdDevice.macAddress = null;
        wfdDevice.isAvailableForSession = false;//since init is to be called
        wfdDevice.addressOfAP = null;
        wfdDevice.coupledSinkStatus = 0;
        wfdDevice.preferredConnectivity = 0;
        if (type == WfdEnums.WFDDeviceType.SOURCE) {
            wfdDevice.deviceName = "Source_device";
            if (iface != null) {
                int index = iface.indexOf(':', 0);
                if (index > 0) {
                    wfdDevice.ipAddress = iface.substring(0, index);
                    wfdDevice.rtspPort =
                            Integer.parseInt(iface.substring(index + 1, iface.length()));
                    if (wfdDevice.ipAddress == null ||
                            wfdDevice.rtspPort < 1 ||
                            wfdDevice.rtspPort > 65535) {
                        Log.e(TAG, "Invalid RTSP port received or no valid IP");
                        return null;
                    }
                }
            }
        } else if (type == WfdEnums.WFDDeviceType.PRIMARY_SINK) {
            wfdDevice.deviceName = "Sink_device";
            wfdDevice.rtspPort = 0;
            wfdDevice.ipAddress = null;
        } else {
            wfdDevice.deviceName = "WFD_device";
            wfdDevice.rtspPort = 0;
            wfdDevice.ipAddress = null;
        }
        return wfdDevice;
    }

    private void blockOnLock() {
        blockOnLock(OP_TIMEOUT);
    }

    /**
     * Helper method to block on sERDLock object monitor
     */
    private void blockOnLock(final int timeout) {
        synchronized (sERDLock) {
            try {
                sERDLock.wait(timeout);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }


    /**
     * Helper method to unblock threads waiting on the seRDLock object monitor
     */
    private void unblockWaiters() {
        synchronized (sERDLock) {
            sERDLock.notifyAll();
        }
    }

    /**
     * Helper method to clean up the resources of the ERD object
     */
    private void cleanupResources() {
        Log.d(TAG, "Initiating resource cleanup");
        if (mWorkerThread != null) {
            mWorkerThread.quitSafely();
            try {
                mWorkerThread.join();
                mWorkerThread = null;
            } catch (InterruptedException e) {
                Log.e(TAG, "Failed to join on mWorkerThread");
            }
        }
        //mListener = null;//Release later when being finalized
        mDisplayHandler = null;
        mERDHandler = null;
        mContext = null;
        surface = null;
        mIface = null;
        mLocalWfdDevice = null;
        mPeerWfdDevice = null;
        mActionListener = null;
        Log.d(TAG, "Done with resource cleanup");
    }

    /**
     * This method should always be invoked from Worker thread to
     * stride over concurrency issues
     *
     * @param s The new state to move to
     */
    private void setState(WFDState s) {
        Log.d(TAG, "Moving from " + sState + " --> " + s);
        sState = s;
    }
}
